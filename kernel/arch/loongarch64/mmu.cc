// Copyright 2016 The Fuchsia Authors
// Copyright (c) 2014 Google Inc. All rights reserved
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT
#include "arch/loongarch64/mmu.h"

#include <arch/aspace.h>
#include <arch/mmu.h>
#include <assert.h>
#include <bitmap/raw-bitmap.h>
#include <bitmap/storage.h>
#include <bits.h>
#include <debug.h>
#include <err.h>
#include <fbl/auto_call.h>
#include <fbl/auto_lock.h>
#include <inttypes.h>
#include <kernel/mutex.h>
#include <lib/heap.h>
#include <lib/ktrace.h>
#include <rand.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>
#include <vm/arch_vm_aspace.h>
#include <vm/physmap.h>
#include <vm/pmm.h>
#include <vm/vm.h>
#include <vm/vm_aspace.h>
#include <zircon/types.h>

#define LOCAL_TRACE 0
#define TRACE_CONTEXT_SWITCH 0

/* ktraces just local to this file */
#define LOCAL_KTRACE_ENABLE 0

#define LOCAL_KTRACE(string, args...)                                \
    ktrace_probe(LocalTrace<LOCAL_KTRACE_ENABLE>, TraceContext::Cpu, \
                 KTRACE_STRING_REF(string), ##args)

// static_assert(((long)KERNEL_BASE >> MMU_KERNEL_SIZE_SHIFT) == -1, "");
// static_assert(((long)KERNEL_ASPACE_BASE >> MMU_KERNEL_SIZE_SHIFT) == -1, "");
// static_assert(MMU_KERNEL_SIZE_SHIFT <= 48, "");
// static_assert(MMU_KERNEL_SIZE_SHIFT >= 25, "");

// Static relocated base to prepare for KASLR. Used at early boot and by gdb
// script to know the target relocated address.
// TODO(SEC-31): Choose it randomly.
#if DISABLE_KASLR
uint64_t kernel_relocated_base = KERNEL_BASE;
#else
uint64_t kernel_relocated_base = 0xffffffff10000000;
#endif

// The main translation table.
pte_t loongarch64_kernel_translation_table[MMU_KERNEL_PAGE_TABLE_ENTRIES_TOP] __ALIGNED(MMU_KERNEL_PAGE_TABLE_ENTRIES_TOP * 8);

pte_t* loongarch64_get_kernel_ptable() {
    return loongarch64_kernel_translation_table;
    return 0;
}

namespace {

class AsidAllocator {
public:
    AsidAllocator() { bitmap_.Reset(MMU_LOONGARCH64_MAX_USER_ASID + 1); }
    ~AsidAllocator() = default;

    zx_status_t Alloc(uint16_t* asid);
    zx_status_t Free(uint16_t asid);

private:
    DISALLOW_COPY_ASSIGN_AND_MOVE(AsidAllocator);

    DECLARE_MUTEX(AsidAllocator) lock_;
    uint16_t last_ TA_GUARDED(lock_) = MMU_LOONGARCH64_FIRST_USER_ASID - 1;

    bitmap::RawBitmapGeneric<bitmap::FixedStorage<MMU_LOONGARCH64_MAX_USER_ASID + 1>> bitmap_ TA_GUARDED(lock_);

    static_assert(MMU_LOONGARCH64_ASID_BITS <= 10, "");
};

zx_status_t AsidAllocator::Alloc(uint16_t* asid) {
    uint16_t new_asid;

    // use the bitmap allocator to allocate ids in the range of
    // [MMU_LOONGARCH64_FIRST_USER_ASID, MMU_LOONGARCH64_MAX_USER_ASID]
    // start the search from the last found id + 1 and wrap when hitting the end of the range
    {
        Guard<Mutex> al{&lock_};

        size_t val;
        bool notfound = bitmap_.Get(last_ + 1, MMU_LOONGARCH64_MAX_USER_ASID + 1, &val);
        if (unlikely(notfound)) {
            // search again from the start
            notfound = bitmap_.Get(MMU_LOONGARCH64_FIRST_USER_ASID, MMU_LOONGARCH64_MAX_USER_ASID + 1, &val);
            if (unlikely(notfound)) {
                TRACEF("LOONGARCH64: out of ASIDs\n");
                return ZX_ERR_NO_MEMORY;
            }
        }
        bitmap_.SetOne(val);

        DEBUG_ASSERT(val <= MMU_LOONGARCH64_GLOBAL_ASID);

        new_asid = (uint16_t)val;
        last_ = new_asid;
    }

    LTRACEF("new asid %#x\n", new_asid);

    *asid = new_asid;

    return ZX_OK;
}

zx_status_t AsidAllocator::Free(uint16_t asid) {
    LTRACEF("free asid %#x\n", asid);

    Guard<Mutex> al{&lock_};

    bitmap_.ClearOne(asid);

    return ZX_OK;
}

AsidAllocator asid;

} // namespace

// Convert user level mmu flags to flags that go in L1 descriptors.
static pte_t mmu_flags_to_pte_attr(uint flags) {
    // TODO: remove dirty flag after finishing irq handler
    pte_t attr = _PAGE_VALID;

    switch (flags & ARCH_MMU_FLAG_CACHE_MASK) {
    case ARCH_MMU_FLAG_CACHED:
        attr |= _CACHE_CC;
        break;
    case ARCH_MMU_FLAG_UNCACHED:
        attr |= _CACHE_SUC;
        break;
    default:
        PANIC_UNIMPLEMENTED;
    }

    if (flags & ARCH_MMU_FLAG_PERM_USER) {
        attr |= _PAGE_USER;
    } else {
        attr |= _PAGE_KERN;
    }

    if (!(flags & ARCH_MMU_FLAG_PERM_READ)) {
        attr |= _PAGE_NO_READ;
    }
    if (flags & ARCH_MMU_FLAG_PERM_WRITE) {
        attr |= _PAGE_WRITE | _PAGE_DIRTY;
    }
    if (!(flags & ARCH_MMU_FLAG_PERM_EXECUTE)) {
        attr |= _PAGE_NO_EXEC;
    }

    return attr;
}

static void pte_attr_to_mmu_flags(pte_t pte, uint* mmu_flags) {
    switch (pte & _CACHE_MASK) {
    case _CACHE_SUC:
        *mmu_flags |= ARCH_MMU_FLAG_UNCACHED;
        break;
    case _CACHE_CC:
        *mmu_flags |= ARCH_MMU_FLAG_CACHED;
        break;
    default:
        PANIC_UNIMPLEMENTED;
    }

    if (!(pte & _PAGE_NO_READ)) {
        *mmu_flags |= ARCH_MMU_FLAG_PERM_READ;
    }
    if (pte & (_PAGE_WRITE | _PAGE_DIRTY)) {
        *mmu_flags |= ARCH_MMU_FLAG_PERM_WRITE;
    }
    if (!(pte & _PAGE_NO_EXEC)) {
        *mmu_flags |= ARCH_MMU_FLAG_PERM_EXECUTE;
    }
    switch (pte & _PAGE_PLV) {
    case _PAGE_KERN:
        break;
    case _PAGE_USER:
        *mmu_flags |= ARCH_MMU_FLAG_PERM_USER;
        break;
    }
}

zx_status_t LoongarchArchVmAspace::Query(vaddr_t vaddr, paddr_t* paddr, uint* mmu_flags) {
    Guard<Mutex> al{&lock_};
    return QueryLocked(vaddr, paddr, mmu_flags);
}

zx_status_t LoongarchArchVmAspace::QueryLocked(vaddr_t vaddr, paddr_t* paddr, uint* mmu_flags) {
    ulong index;
    uint index_shift;
    uint page_size_shift;
    pte_t pte;
    pte_t pte_addr;
    volatile pte_t* page_table;
    vaddr_t vaddr_rem;

    canary_.Assert();
    LTRACEF("aspace %p, vaddr 0x%lx\n", this, vaddr);

    DEBUG_ASSERT(tt_virt_);

    DEBUG_ASSERT(IsValidVaddr(vaddr));
    if (!IsValidVaddr(vaddr))
        return ZX_ERR_OUT_OF_RANGE;

    // Compute shift values based on if this address space is for kernel or user space.
    // TODO: Do we need different shift for K/U/G aspace?
    if (flags_ & ARCH_ASPACE_FLAG_KERNEL) {
        index_shift = MMU_KERNEL_TOP_SHIFT;
        page_size_shift = MMU_KERNEL_PAGE_SIZE_SHIFT;

        vaddr_t kernel_base = ~0UL << MMU_KERNEL_SIZE_SHIFT;
        vaddr_rem = vaddr - kernel_base;

        index = vaddr_rem >> index_shift;
        ASSERT(index < MMU_KERNEL_PAGE_TABLE_ENTRIES_TOP);
    } else if (flags_ & ARCH_ASPACE_FLAG_GUEST) {
        index_shift = MMU_GUEST_TOP_SHIFT;
        page_size_shift = MMU_GUEST_PAGE_SIZE_SHIFT;

        vaddr_rem = vaddr;
        index = vaddr_rem >> index_shift;
        ASSERT(index < MMU_GUEST_PAGE_TABLE_ENTRIES_TOP);
    } else {
        index_shift = MMU_USER_TOP_SHIFT;
        page_size_shift = MMU_USER_PAGE_SIZE_SHIFT;

        vaddr_rem = vaddr;
        index = vaddr_rem >> index_shift;
        ASSERT(index < MMU_USER_PAGE_TABLE_ENTRIES_TOP);
    }

    page_table = tt_virt_;

    for (int i = 2; i >= 0; --i) {
        index = vaddr_rem >> index_shift;
        vaddr_rem -= (vaddr_t)index << index_shift;
        pte = page_table[index];
        // descriptor_type = pte & MMU_PTE_DESCRIPTOR_MASK;
        pte_addr = pte & MMU_PTE_OUTPUT_ADDR_MASK;

        LTRACEF("va %#" PRIxPTR ", index %lx, index_shift %u, rem %#" PRIxPTR
                ", pte %#" PRIx64 "\n",
                vaddr, index, index_shift, vaddr_rem, pte);

        if (pte == MMU_PTE_INVALID)
            return ZX_ERR_NOT_FOUND;

        if (i == 0)
            break;

        page_table = static_cast<volatile pte_t*>(paddr_to_physmap(pte_addr));
        index_shift -= page_size_shift - 3;
    }

    LTRACEF("pte_addr %016lx vaddr_rem %016lx\n", pte_addr, vaddr_rem);
    if (paddr != NULL)
        *paddr = pte_addr + vaddr_rem;
    if (mmu_flags != NULL) {
        *mmu_flags = 0;
        pte_attr_to_mmu_flags(pte, mmu_flags);
    }
    LTRACEF("va 0x%lx, paddr 0x%lx, flags 0x%x\n",
            vaddr, paddr ? *paddr : ~0UL, mmu_flags ? *mmu_flags : ~0U);
    return 0;
}

zx_status_t LoongarchArchVmAspace::AllocPageTable(paddr_t* paddrp, uint page_size_shift) {
    LTRACEF("page_size_shift %u\n", page_size_shift);

    // currently we only support allocating a single page
    DEBUG_ASSERT(page_size_shift == PAGE_SIZE_SHIFT);

    vm_page_t* page;
    zx_status_t status = pmm_alloc_page(0, &page, paddrp);
    if (status != ZX_OK) {
        return status;
    }

    page->set_state(VM_PAGE_STATE_MMU);
    pt_pages_++;

    LOCAL_KTRACE("page table alloc");

    LTRACEF("allocated 0x%lx\n", *paddrp);
    return 0;
}

void LoongarchArchVmAspace::FreePageTable(void* vaddr, paddr_t paddr, uint page_size_shift) {
    LTRACEF("vaddr %p paddr 0x%lx page_size_shift %u\n", vaddr, paddr, page_size_shift);

    // currently we only support freeing a single page
    DEBUG_ASSERT(page_size_shift == PAGE_SIZE_SHIFT);

    vm_page_t* page;

    LOCAL_KTRACE("page table free");

    page = paddr_to_vm_page(paddr);
    if (!page) {
        panic("bad page table paddr 0x%lx\n", paddr);
    }
    pmm_free_page(page);

    pt_pages_--;
}

volatile pte_t* LoongarchArchVmAspace::GetPageTable(uint page_size_shift,
                                              vaddr_t pt_index, volatile pte_t* page_table) {
    pte_t pte;
    paddr_t paddr;
    void* vaddr;

    DEBUG_ASSERT(page_size_shift <= MMU_MAX_PAGE_SIZE_SHIFT);

    pte = page_table[pt_index];
    if (pte == MMU_PTE_INVALID) {
        zx_status_t ret = AllocPageTable(&paddr, page_size_shift);
        if (ret) {
            TRACEF("failed to allocate page table\n");
            return NULL;
        }
        vaddr = paddr_to_physmap(paddr);

        LTRACEF("allocated page table, vaddr %p, paddr 0x%lx\n", vaddr, paddr);
        memset(vaddr, MMU_PTE_DESCRIPTOR_INVALID, 1U << page_size_shift);

        // ensure that the zeroing is observable from hardware page table walkers
        wmb();

        pte = paddr;
        page_table[pt_index] = pte;
        LTRACEF("pte %p[%#" PRIxPTR "] = %#" PRIx64 "\n",
                page_table, pt_index, pte);
        return static_cast<volatile pte_t*>(vaddr);
    } else if ((pte & MMU_PTE_DESCRIPTOR_MASK) == MMU_PTE_INVALID) {
        // Non-leaf PTE
        paddr = pte & MMU_PTE_OUTPUT_ADDR_MASK;
        LTRACEF("found page table %#" PRIxPTR "\n", paddr);
        return static_cast<volatile pte_t*>(paddr_to_physmap(paddr));
    } else if ((pte & _PAGE_VALID)) {
        // Leaf PTE
        paddr = pte & MMU_PTE_OUTPUT_ADDR_MASK;
        LTRACEF("assuming leaf pte, paddr %#" PRIxPTR "\n", paddr);
        return NULL;
    } else {
        PANIC_UNIMPLEMENTED;
    }

    return NULL;
}

static bool page_table_is_clear(volatile pte_t* page_table, uint page_size_shift) {
    int i;
    int count = 1U << (page_size_shift - 3);
    pte_t pte;

    for (i = 0; i < count; i++) {
        pte = page_table[i];
        if (pte != MMU_PTE_DESCRIPTOR_INVALID) {
            LTRACEF("page_table at %p still in use, index %d is %#" PRIx64 "\n",
                    page_table, i, pte);
            return false;
        }
    }

    LTRACEF("page table at %p is clear\n", page_table);
    return true;
}

// use the appropriate TLB flush instruction to globally flush the modified entry
// terminal is set when flushing at the final level of the page table.
void LoongarchArchVmAspace::FlushTLBEntry(vaddr_t vaddr, bool terminal) {
    if (flags_ & ARCH_ASPACE_FLAG_GUEST) {
        TODO();
        // paddr_t vttbr = arm64_vttbr(asid_, tt_phys_);
        // __UNUSED zx_status_t status = arm64_el2_tlbi_ipa(vttbr, vaddr, terminal);
        // DEBUG_ASSERT(status == ZX_OK);
    } else if (asid_ == MMU_LOONGARCH64_GLOBAL_ASID) {
        // flush this address on global asid
        __asm__ __volatile__("invtlb 0x05, %0, %1" : : "r"(asid_), "r"(vaddr));
    } else {
        // flush this address for the specific asid
        __asm__ __volatile__("invtlb 0x05, %0, %1" : : "r"(asid_), "r"(vaddr) :);
    }
}

// NOTE: caller must DSB afterwards to ensure TLB entries are flushed
ssize_t LoongarchArchVmAspace::UnmapPageTable(vaddr_t vaddr, vaddr_t vaddr_rel,
                                        size_t size, uint index_shift,
                                        uint page_size_shift,
                                        volatile pte_t* page_table) {
    volatile pte_t* next_page_table;
    vaddr_t index;
    size_t chunk_size;
    vaddr_t vaddr_rem;
    vaddr_t block_size;
    vaddr_t block_mask;
    pte_t pte;
    paddr_t page_table_paddr;
    size_t unmap_size;

    LTRACEF("vaddr 0x%lx, vaddr_rel 0x%lx, size 0x%lx, index shift %u, page_size_shift %u, page_table %p\n",
            vaddr, vaddr_rel, size, index_shift, page_size_shift, page_table);

    unmap_size = 0;
    while (size) {
        block_size = 1UL << index_shift;
        block_mask = block_size - 1;
        vaddr_rem = vaddr_rel & block_mask;
        chunk_size = MIN(size, block_size - vaddr_rem);
        index = vaddr_rel >> index_shift;

        pte = page_table[index];

        if (index_shift > page_size_shift &&
            pte != MMU_PTE_INVALID &&
            (pte & MMU_PTE_DESCRIPTOR_MASK) == MMU_PTE_INVALID) {
            page_table_paddr = pte & MMU_PTE_OUTPUT_ADDR_MASK;
            next_page_table = static_cast<volatile pte_t*>(paddr_to_physmap(page_table_paddr));
            UnmapPageTable(vaddr, vaddr_rem, chunk_size,
                           index_shift - (page_size_shift - 3),
                           page_size_shift, next_page_table);
            if (chunk_size == block_size ||
                page_table_is_clear(next_page_table, page_size_shift)) {
                LTRACEF("pte %p[0x%lx] = 0 (was page table)\n", page_table, index);
                page_table[index] = MMU_PTE_INVALID;

                // ensure that the update is observable from hardware page table walkers
                wmb();

                // flush the non terminal TLB entry
                FlushTLBEntry(vaddr, false);

                FreePageTable(const_cast<pte_t*>(next_page_table), page_table_paddr,
                              page_size_shift);
            }
        } else if (pte != MMU_PTE_INVALID) {
            LTRACEF("pte %p[0x%lx] = 0\n", page_table, index);
            page_table[index] = MMU_PTE_INVALID;

            // ensure that the update is observable from hardware page table walkers
            wmb();

            // flush the terminal TLB entry
            FlushTLBEntry(vaddr, true);
        } else {
            LTRACEF("pte %p[0x%lx] already clear\n", page_table, index);
        }
        vaddr += chunk_size;
        vaddr_rel += chunk_size;
        size -= chunk_size;
        unmap_size += chunk_size;
    }

    return unmap_size;
}

// NOTE: caller must DSB afterwards to ensure TLB entries are flushed
ssize_t LoongarchArchVmAspace::MapPageTable(vaddr_t vaddr_in, vaddr_t vaddr_rel_in,
                                      paddr_t paddr_in, size_t size_in,
                                      pte_t attrs, uint index_shift,
                                      uint page_size_shift,
                                      volatile pte_t* page_table) {
    ssize_t ret;
    volatile pte_t* next_page_table;
    vaddr_t index;
    vaddr_t vaddr = vaddr_in;
    vaddr_t vaddr_rel = vaddr_rel_in;
    paddr_t paddr = paddr_in;
    size_t size = size_in;
    size_t chunk_size;
    vaddr_t vaddr_rem;
    vaddr_t block_size;
    vaddr_t block_mask;
    pte_t pte;
    size_t mapped_size;

    LTRACEF("vaddr %#" PRIxPTR ", vaddr_rel %#" PRIxPTR ", paddr %#" PRIxPTR
            ", size %#zx, attrs %#" PRIx64
            ", index shift %u, page_size_shift %u, page_table %p\n",
            vaddr, vaddr_rel, paddr, size, attrs,
            index_shift, page_size_shift, page_table);

    if ((vaddr_rel | paddr | size) & ((1UL << page_size_shift) - 1)) {
        TRACEF("not page aligned\n");
        return ZX_ERR_INVALID_ARGS;
    }

    mapped_size = 0;
    while (size) {
        block_size = 1UL << index_shift;
        block_mask = block_size - 1;
        vaddr_rem = vaddr_rel & block_mask;
        chunk_size = MIN(size, block_size - vaddr_rem);
        index = vaddr_rel >> index_shift;

        if (((vaddr_rel | paddr) & block_mask) ||
            (chunk_size != block_size) ||
            (index_shift > PAGE_SIZE_SHIFT)) {
            next_page_table = GetPageTable(page_size_shift, index, page_table);
            if (!next_page_table)
                goto err;

            ret = MapPageTable(vaddr, vaddr_rem, paddr, chunk_size, attrs,
                               index_shift - (page_size_shift - 3),
                               page_size_shift, next_page_table);
            if (ret < 0)
                goto err;
        } else {
            pte = page_table[index];
            if (pte) {
                TRACEF("page table entry already in use, index %#" PRIxPTR ", %#" PRIx64 "\n",
                       index, pte);
                goto err;
            }

            pte = paddr | attrs;
            if (index_shift == page_size_shift)
                pte |= _PAGE_VALID;
            if (flags_ & ARCH_ASPACE_FLAG_GUEST) {
                // TODO: what is guest aspace?
                // pte |= _PAGE_GLOBAL;
                TODO();
            }
            LTRACEF("pte %p[%#" PRIxPTR "] = %#" PRIx64 "\n",
                    page_table, index, pte);
            page_table[index] = pte;
        }
        vaddr += chunk_size;
        vaddr_rel += chunk_size;
        paddr += chunk_size;
        size -= chunk_size;
        mapped_size += chunk_size;
    }

    return mapped_size;

err:
    UnmapPageTable(vaddr_in, vaddr_rel_in, size_in - size, index_shift,
                   page_size_shift, page_table);
    return ZX_ERR_INTERNAL;
}

// NOTE: caller must DSB afterwards to ensure TLB entries are flushed
zx_status_t LoongarchArchVmAspace::ProtectPageTable(vaddr_t vaddr_in, vaddr_t vaddr_rel_in,
                                              size_t size_in, pte_t attrs,
                                              uint index_shift, uint page_size_shift,
                                              volatile pte_t* page_table) {
    volatile pte_t* next_page_table;
    vaddr_t index;
    vaddr_t vaddr = vaddr_in;
    vaddr_t vaddr_rel = vaddr_rel_in;
    size_t size = size_in;
    size_t chunk_size;
    vaddr_t vaddr_rem;
    vaddr_t block_size;
    vaddr_t block_mask;
    paddr_t page_table_paddr;
    pte_t pte;

    LTRACEF("vaddr %#" PRIxPTR ", vaddr_rel %#" PRIxPTR ", size %#" PRIxPTR
            ", attrs %#" PRIx64
            ", index shift %u, page_size_shift %u, page_table %p\n",
            vaddr, vaddr_rel, size, attrs,
            index_shift, page_size_shift, page_table);

    // vaddr_rel and size must be page aligned
    DEBUG_ASSERT(((vaddr_rel | size) & ((1UL << page_size_shift) - 1)) == 0);

    while (size) {
        block_size = 1UL << index_shift;
        block_mask = block_size - 1;
        vaddr_rem = vaddr_rel & block_mask;
        chunk_size = MIN(size, block_size - vaddr_rem);
        index = vaddr_rel >> index_shift;
        pte = page_table[index];

        if (index_shift > page_size_shift &&
            (pte & MMU_PTE_DESCRIPTOR_MASK) == MMU_PTE_DESCRIPTOR_TABLE) {
            page_table_paddr = pte & MMU_PTE_OUTPUT_ADDR_MASK;
            next_page_table = static_cast<volatile pte_t*>(paddr_to_physmap(page_table_paddr));
            ProtectPageTable(vaddr, vaddr_rem, chunk_size, attrs,
                             index_shift - (page_size_shift - 3),
                             page_size_shift, next_page_table);
        } else if (pte) {
            LTRACEF("pte %p[%#" PRIxPTR "] = %#" PRIx64 "\n",
                    page_table, index, pte);
            pte = (pte & ~MMU_PTE_PERMISSION_MASK) | attrs;
            LTRACEF("pte %p[%#" PRIxPTR "] = %#" PRIx64 "\n",
                    page_table, index, pte);
            page_table[index] = pte;

            // ensure that the update is observable from hardware page table walkers
            wmb();

            // flush the terminal TLB entry
            FlushTLBEntry(vaddr, true);
        } else {
            LTRACEF("page table entry does not exist, index %#" PRIxPTR
                    ", %#" PRIx64 "\n",
                    index, pte);
        }
        vaddr += chunk_size;
        vaddr_rel += chunk_size;
        size -= chunk_size;
    }

    return ZX_OK;
}

// internal routine to map a run of pages
ssize_t LoongarchArchVmAspace::MapPages(vaddr_t vaddr, paddr_t paddr, size_t size,
                                  pte_t attrs, vaddr_t vaddr_base, uint top_size_shift,
                                  uint top_index_shift, uint page_size_shift) {
    vaddr_t vaddr_rel = vaddr - vaddr_base;
    vaddr_t vaddr_rel_max = 1UL << top_size_shift;

    LTRACEF("vaddr %#" PRIxPTR ", paddr %#" PRIxPTR ", size %#" PRIxPTR
            ", attrs %#" PRIx64 ", asid %#x\n",
            vaddr, paddr, size, attrs, asid_);

    if (vaddr_rel > vaddr_rel_max - size || size > vaddr_rel_max) {
        TRACEF("vaddr %#" PRIxPTR ", size %#" PRIxPTR " out of range vaddr %#" PRIxPTR ", size %#" PRIxPTR "\n",
               vaddr, size, vaddr_base, vaddr_rel_max);
        return ZX_ERR_INVALID_ARGS;
    }

    LOCAL_KTRACE("mmu map", (vaddr & ~PAGE_MASK) | ((size >> PAGE_SIZE_SHIFT) & PAGE_MASK));
    ssize_t ret = MapPageTable(vaddr, vaddr_rel, paddr, size, attrs,
                               top_index_shift, page_size_shift, tt_virt_);
    smp_mb();
    return ret;
}

ssize_t LoongarchArchVmAspace::UnmapPages(vaddr_t vaddr, size_t size,
                                    vaddr_t vaddr_base,
                                    uint top_size_shift,
                                    uint top_index_shift,
                                    uint page_size_shift) {
    vaddr_t vaddr_rel = vaddr - vaddr_base;
    vaddr_t vaddr_rel_max = 1UL << top_size_shift;

    LTRACEF("vaddr 0x%lx, size 0x%lx, asid 0x%x\n", vaddr, size, asid_);

    if (vaddr_rel > vaddr_rel_max - size || size > vaddr_rel_max) {
        TRACEF("vaddr 0x%lx, size 0x%lx out of range vaddr 0x%lx, size 0x%lx\n",
               vaddr, size, vaddr_base, vaddr_rel_max);
        return ZX_ERR_INVALID_ARGS;
    }

    LOCAL_KTRACE("mmu unmap", (vaddr & ~PAGE_MASK) | ((size >> PAGE_SIZE_SHIFT) & PAGE_MASK));

    ssize_t ret = UnmapPageTable(vaddr, vaddr_rel, size, top_index_shift,
                                 page_size_shift, tt_virt_);
    smp_mb();
    return ret;
        return ZX_OK;
}

zx_status_t LoongarchArchVmAspace::ProtectPages(vaddr_t vaddr, size_t size, pte_t attrs,
                                          vaddr_t vaddr_base, uint top_size_shift,
                                          uint top_index_shift, uint page_size_shift) {
    vaddr_t vaddr_rel = vaddr - vaddr_base;
    vaddr_t vaddr_rel_max = 1UL << top_size_shift;

    LTRACEF("vaddr %#" PRIxPTR ", size %#" PRIxPTR ", attrs %#" PRIx64
            ", asid %#x\n",
            vaddr, size, attrs, asid_);

    if (vaddr_rel > vaddr_rel_max - size || size > vaddr_rel_max) {
        TRACEF("vaddr %#" PRIxPTR ", size %#" PRIxPTR " out of range vaddr %#" PRIxPTR ", size %#" PRIxPTR "\n",
               vaddr, size, vaddr_base, vaddr_rel_max);
        return ZX_ERR_INVALID_ARGS;
    }

    LOCAL_KTRACE("mmu protect", (vaddr & ~PAGE_MASK) | ((size >> PAGE_SIZE_SHIFT) & PAGE_MASK));

    zx_status_t ret = ProtectPageTable(vaddr, vaddr_rel, size, attrs,
                                       top_index_shift, page_size_shift,
                                       tt_virt_);
    smp_mb();
    return ret;
}

void LoongarchArchVmAspace::MmuParamsFromFlags(uint mmu_flags,
                                         pte_t* attrs, vaddr_t* vaddr_base,
                                         uint* top_size_shift, uint* top_index_shift,
                                         uint* page_size_shift) {

    if (flags_ & ARCH_ASPACE_FLAG_KERNEL) {
        if (attrs) {
            *attrs = mmu_flags_to_pte_attr(mmu_flags);
        }
        *vaddr_base = ~0UL << MMU_KERNEL_SIZE_SHIFT;
        *top_size_shift = MMU_KERNEL_SIZE_SHIFT;
        *top_index_shift = MMU_KERNEL_TOP_SHIFT;
        *page_size_shift = MMU_KERNEL_PAGE_SIZE_SHIFT;
        LTRACEF("kernel mmu params: base %016lx top size %u shift %u pgshift %u\n",
                *vaddr_base, *top_size_shift, *top_index_shift, *page_size_shift);
    } else if (flags_ & ARCH_ASPACE_FLAG_GUEST) {
        if (attrs) {
            *attrs = mmu_flags_to_pte_attr(mmu_flags);
        }
        *vaddr_base = 0;
        *top_size_shift = MMU_GUEST_SIZE_SHIFT;
        *top_index_shift = MMU_GUEST_TOP_SHIFT;
        *page_size_shift = MMU_GUEST_PAGE_SIZE_SHIFT;
        LTRACEF("guest mmu params: base %016lx top size %u shift %u pgshift %u\n",
                *vaddr_base, *top_size_shift, *top_index_shift, *page_size_shift);
    } else {
        if (attrs) {
            *attrs = mmu_flags_to_pte_attr(mmu_flags);
        }
        *vaddr_base = 0;
        *top_size_shift = MMU_USER_SIZE_SHIFT;
        *top_index_shift = MMU_USER_TOP_SHIFT;
        *page_size_shift = MMU_USER_PAGE_SIZE_SHIFT;
        LTRACEF("user mmu params: base %016lx top size %u shift %u pgshift %u\n",
                *vaddr_base, *top_size_shift, *top_index_shift, *page_size_shift);
    }
}

zx_status_t LoongarchArchVmAspace::MapContiguous(vaddr_t vaddr, paddr_t paddr, size_t count,
                                           uint mmu_flags, size_t* mapped) {
    canary_.Assert();
    LTRACEF("vaddr %#" PRIxPTR " paddr %#" PRIxPTR " count %zu flags %#x\n",
            vaddr, paddr, count, mmu_flags);

    DEBUG_ASSERT(tt_virt_);

    DEBUG_ASSERT(IsValidVaddr(vaddr));
    if (!IsValidVaddr(vaddr))
        return ZX_ERR_OUT_OF_RANGE;

    if (!(mmu_flags & ARCH_MMU_FLAG_PERM_READ))
        return ZX_ERR_INVALID_ARGS;

    // paddr and vaddr must be aligned.
    DEBUG_ASSERT(IS_PAGE_ALIGNED(vaddr));
    DEBUG_ASSERT(IS_PAGE_ALIGNED(paddr));
    if (!IS_PAGE_ALIGNED(vaddr) || !IS_PAGE_ALIGNED(paddr))
        return ZX_ERR_INVALID_ARGS;

    if (count == 0)
        return ZX_OK;

    ssize_t ret = 0;
    {
        Guard<Mutex> a{&lock_};
        pte_t attrs;
        vaddr_t vaddr_base;
        uint top_size_shift, top_index_shift, page_size_shift;
        MmuParamsFromFlags(mmu_flags, &attrs, &vaddr_base, &top_size_shift, &top_index_shift,
                           &page_size_shift);
        ret = MapPages(vaddr, paddr, count * PAGE_SIZE,
                       attrs, vaddr_base, top_size_shift,
                       top_index_shift, page_size_shift);
    }

    if (mapped) {
        *mapped = (ret > 0) ? (ret / PAGE_SIZE) : 0u;
        DEBUG_ASSERT(*mapped <= count);
    }

    return (ret < 0) ? (zx_status_t)ret : ZX_OK;
}

zx_status_t LoongarchArchVmAspace::Map(vaddr_t vaddr, paddr_t* phys, size_t count, uint mmu_flags,
                                 size_t* mapped) {
    canary_.Assert();
    LTRACEF("vaddr %#" PRIxPTR " count %zu flags %#x\n",
            vaddr, count, mmu_flags);

    DEBUG_ASSERT(tt_virt_);

    DEBUG_ASSERT(IsValidVaddr(vaddr));
    if (!IsValidVaddr(vaddr))
        return ZX_ERR_OUT_OF_RANGE;
    for (size_t i = 0; i < count; ++i) {
        DEBUG_ASSERT(IS_PAGE_ALIGNED(phys[i]));
        if (!IS_PAGE_ALIGNED(phys[i]))
            return ZX_ERR_INVALID_ARGS;
    }

    if (!(mmu_flags & ARCH_MMU_FLAG_PERM_READ))
        return ZX_ERR_INVALID_ARGS;

    // vaddr must be aligned.
    DEBUG_ASSERT(IS_PAGE_ALIGNED(vaddr));
    if (!IS_PAGE_ALIGNED(vaddr))
        return ZX_ERR_INVALID_ARGS;

    if (count == 0)
        return ZX_OK;

    size_t total_mapped = 0;
    {
        Guard<Mutex> a{&lock_};
        pte_t attrs;
        vaddr_t vaddr_base;
        uint top_size_shift, top_index_shift, page_size_shift;
        MmuParamsFromFlags(mmu_flags, &attrs, &vaddr_base, &top_size_shift, &top_index_shift,
                           &page_size_shift);

        ssize_t ret;
        size_t idx = 0;
        auto undo = fbl::MakeAutoCall([&]() TA_NO_THREAD_SAFETY_ANALYSIS {
            if (idx > 0) {
                UnmapPages(vaddr, idx * PAGE_SIZE, vaddr_base, top_size_shift,
                           top_index_shift, page_size_shift);
            }
        });

        vaddr_t v = vaddr;
        for (; idx < count; ++idx) {
            paddr_t paddr = phys[idx];
            DEBUG_ASSERT(IS_PAGE_ALIGNED(paddr));
            // TODO: optimize by not DSBing inside each of these calls
            ret = MapPages(v, paddr, PAGE_SIZE,
                           attrs, vaddr_base, top_size_shift,
                           top_index_shift, page_size_shift);
            if (ret < 0) {
                return static_cast<zx_status_t>(ret);
            }

            v += PAGE_SIZE;
            total_mapped += ret / PAGE_SIZE;
        }
        undo.cancel();
    }
    DEBUG_ASSERT(total_mapped <= count);

    if (mapped) {
        *mapped = total_mapped;
    }

    return ZX_OK;
}

zx_status_t LoongarchArchVmAspace::Unmap(vaddr_t vaddr, size_t count, size_t* unmapped) {
    canary_.Assert();
    LTRACEF("vaddr %#" PRIxPTR " count %zu\n", vaddr, count);

    DEBUG_ASSERT(tt_virt_);

    DEBUG_ASSERT(IsValidVaddr(vaddr));

    if (!IsValidVaddr(vaddr))
        return ZX_ERR_OUT_OF_RANGE;

    DEBUG_ASSERT(IS_PAGE_ALIGNED(vaddr));
    if (!IS_PAGE_ALIGNED(vaddr))
        return ZX_ERR_INVALID_ARGS;

    Guard<Mutex> a{&lock_};

    ssize_t ret = 0;
    {
        vaddr_t vaddr_base;
        uint top_size_shift, top_index_shift, page_size_shift;
        MmuParamsFromFlags(0, nullptr, &vaddr_base, &top_size_shift, &top_index_shift,
                           &page_size_shift);

        ret = UnmapPages(vaddr, count * PAGE_SIZE,
                         vaddr_base, top_size_shift,
                         top_index_shift, page_size_shift);
    }

    if (unmapped) {
        *unmapped = (ret > 0) ? (ret / PAGE_SIZE) : 0u;
        DEBUG_ASSERT(*unmapped <= count);
    }

    return (ret < 0) ? (zx_status_t)ret : 0;
}

zx_status_t LoongarchArchVmAspace::Protect(vaddr_t vaddr, size_t count, uint mmu_flags) {
    canary_.Assert();

    if (!IsValidVaddr(vaddr))
        return ZX_ERR_INVALID_ARGS;

    if (!IS_PAGE_ALIGNED(vaddr))
        return ZX_ERR_INVALID_ARGS;

    if (!(mmu_flags & ARCH_MMU_FLAG_PERM_READ))
        return ZX_ERR_INVALID_ARGS;

    Guard<Mutex> a{&lock_};

    int ret = 0;
    {
        pte_t attrs;
        vaddr_t vaddr_base;
        uint top_size_shift, top_index_shift, page_size_shift;
        MmuParamsFromFlags(mmu_flags, &attrs, &vaddr_base, &top_size_shift, &top_index_shift,
                           &page_size_shift);

        ret = ProtectPages(vaddr, count * PAGE_SIZE,
                           attrs, vaddr_base,
                           top_size_shift, top_index_shift, page_size_shift);
    }

    return ret;
}

zx_status_t LoongarchArchVmAspace::Init(vaddr_t base, size_t size, uint flags) {
    canary_.Assert();
    LTRACEF("aspace %p, base %#" PRIxPTR ", size 0x%zx, flags 0x%x\n",
            this, base, size, flags);

    Guard<Mutex> a{&lock_};

    // Validate that the base + size is sane and doesn't wrap.
    DEBUG_ASSERT(size > PAGE_SIZE);
    DEBUG_ASSERT(base + size - 1 > base);

    flags_ = flags;
    if (flags & ARCH_ASPACE_FLAG_KERNEL) {
        // At the moment we can only deal with address spaces as globally defined.
        DEBUG_ASSERT(base == ~0UL << MMU_KERNEL_SIZE_SHIFT);
        DEBUG_ASSERT(size == 1UL << MMU_KERNEL_SIZE_SHIFT);

        base_ = base;
        size_ = size;
        tt_virt_ = loongarch64_kernel_translation_table;
        tt_phys_ = vaddr_to_paddr(const_cast<pte_t*>(tt_virt_));
        asid_ = (uint16_t)MMU_LOONGARCH64_GLOBAL_ASID;
    } else {
        if (flags & ARCH_ASPACE_FLAG_GUEST) {
            DEBUG_ASSERT(base + size <= 1UL << MMU_GUEST_SIZE_SHIFT);
        } else {
            DEBUG_ASSERT(base + size <= 1UL << MMU_USER_SIZE_SHIFT);
            if (asid.Alloc(&asid_) != ZX_OK)
                return ZX_ERR_NO_MEMORY;
        }

        base_ = base;
        size_ = size;

        paddr_t pa;
        vm_page_t* page;
        zx_status_t status = pmm_alloc_page(0, &page, &pa);
        if (status != ZX_OK) {
            return status;
        }
        page->set_state(VM_PAGE_STATE_MMU);

        volatile pte_t* va = static_cast<volatile pte_t*>(paddr_to_physmap(pa));

        tt_virt_ = va;
        tt_phys_ = pa;

        // zero the top level translation table.
        // XXX remove when PMM starts returning pre-zeroed pages.
        arch_zero_page(const_cast<pte_t*>(tt_virt_));
    }
    pt_pages_ = 1;

    LTRACEF("tt_phys %#" PRIxPTR " tt_virt %p\n", tt_phys_, tt_virt_);

    return ZX_OK;
}

zx_status_t LoongarchArchVmAspace::Destroy() {
    canary_.Assert();
    LTRACEF("aspace %p\n", this);

    Guard<Mutex> a{&lock_};

    DEBUG_ASSERT((flags_ & ARCH_ASPACE_FLAG_KERNEL) == 0);

    // XXX make sure it's not mapped

    vm_page_t* page = paddr_to_vm_page(tt_phys_);
    DEBUG_ASSERT(page);
    pmm_free_page(page);

    if (flags_ & ARCH_ASPACE_FLAG_GUEST) {
        TODO();
        // paddr_t vttbr = arm64_vttbr(asid_, tt_phys_);
        // __UNUSED zx_status_t status = arm64_el2_tlbi_vmid(vttbr);
        // DEBUG_ASSERT(status == ZX_OK);
    } else {
        __asm__ __volatile__("invtlb 0x00, $r0, $r0" ::: "memory");
        asid.Free(asid_);
        asid_ = MMU_LOONGARCH64_UNUSED_ASID;
    }

    return ZX_OK;
}

void LoongarchArchVmAspace::ContextSwitch(LoongarchArchVmAspace* old_aspace, LoongarchArchVmAspace* aspace) {
    if (TRACE_CONTEXT_SWITCH)
        TRACEF("aspace %p\n", aspace);

    LoongarchArchVmAspace *kernel_aspace = &VmAspace::kernel_aspace()->arch_aspace();

    if (aspace) {
        // switch to specified user aspace
        DEBUG_ASSERT(aspace != kernel_aspace);
        aspace->canary_.Assert();
        DEBUG_ASSERT((aspace->flags_ & (ARCH_ASPACE_FLAG_KERNEL | ARCH_ASPACE_FLAG_GUEST)) == 0);

        uint16_t asid = aspace->asid_;
        DEBUG_ASSERT(asid < MMU_LOONGARCH64_MAX_USER_ASID);
        csr_writel(asid, LOONGARCH_CSR_ASID);

        paddr_t tt_phys = aspace->tt_phys_;
        DEBUG_ASSERT(tt_phys != 0);
        csr_writeq(tt_phys, LOONGARCH_CSR_PGDL);

        // TODO: do we need any sync here?
        // __isb(ARM_MB_SY);
        
        if (TRACE_CONTEXT_SWITCH)
            TRACEF("asid %#" PRIx64 ", tt_phys %#" PRIx64 "\n", asid, tt_phys);

    } else {
        // TODO: switch to kernel aspace?
        uint16_t asid = kernel_aspace->asid_;
        csr_writel(asid, LOONGARCH_CSR_ASID);

        pte_t* tt_virt = loongarch64_kernel_translation_table;
        paddr_t tt_phys = vaddr_to_paddr(const_cast<pte_t*>(tt_virt));
        DEBUG_ASSERT(tt_phys != 0);
        // we really don't need to write PGDH
        // csr_writeq(tt_phys, LOONGARCH_CSR_PGDH);

        if (TRACE_CONTEXT_SWITCH)
            TRACEF("asid %#" PRIx64 ", tt_phys %#" PRIx64 "\n", asid, tt_phys);
    }

    // TODO: do we need any sync here?
    // __isb(ARM_MB_SY);
}

void arch_zero_page(void* _ptr) {
    memset(_ptr, 0, PAGE_SIZE);
}

zx_status_t arm64_mmu_translate(vaddr_t va, paddr_t* pa, bool user, bool write) {
    TODO();
//     // disable interrupts around this operation to make the at/par instruction combination atomic
//     spin_lock_saved_state_t state;
//     arch_interrupt_save(&state, ARCH_DEFAULT_SPIN_LOCK_FLAG_INTERRUPTS);

//     if (user) {
//         if (write) {
//             __asm__ volatile("at s1e0w, %0" ::"r"(va)
//                              : "memory");
//         } else {
//             __asm__ volatile("at s1e0r, %0" ::"r"(va)
//                              : "memory");
//         }
//     } else {
//         if (write) {
//             __asm__ volatile("at s1e1w, %0" ::"r"(va)
//                              : "memory");
//         } else {
//             __asm__ volatile("at s1e1r, %0" ::"r"(va)
//                              : "memory");
//         }
//     }

//     uint64_t par = __arm_rsr64("par_el1");

//     arch_interrupt_restore(state, ARCH_DEFAULT_SPIN_LOCK_FLAG_INTERRUPTS);

//     // if bit 0 is clear, the translation succeeded
//     if (BIT(par, 0))
//         return ZX_ERR_NO_MEMORY;

//     // physical address is stored in bits [51..12], naturally aligned
//     *pa = BITS(par, 51, 12) | (va & (PAGE_SIZE - 1));

    return ZX_OK;
}

// LoongarchArchVmAspace::LoongarchArchVmAspace() {}

// LoongarchArchVmAspace::~LoongarchArchVmAspace() {
//     // TODO: check that we've destroyed the aspace
// }

 vaddr_t LoongarchArchVmAspace::PickSpot(vaddr_t base, uint prev_region_mmu_flags,
                                   vaddr_t end, uint next_region_mmu_flags,
                                   vaddr_t align, size_t size, uint mmu_flags) {
    canary_.Assert();
     return PAGE_ALIGN(base);
 }
