// Copyright 2017 The Fuchsia Authors
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#include <arch/loongarch64/mmu.h>
#include <inttypes.h>
#include <string.h>
#include <sys/types.h>
#include <zircon/types.h>

#include <vm/bootalloc.h>
#include <vm/physmap.h>

// // Early boot time page table creation code, called from start.S while running in physical address space
// // with the mmu disabled. This code should be position independent as long as it sticks to basic code.

// // this code only works on a 4K page granule, 48 bits of kernel address space
// static_assert(MMU_KERNEL_PAGE_SIZE_SHIFT == 12, "");
// static_assert(MMU_KERNEL_SIZE_SHIFT == 48, "");

// 1GB pages
const uintptr_t l0_large_page_size = 1UL << MMU_LX_X(MMU_KERNEL_PAGE_SIZE_SHIFT, 1);
const uintptr_t l0_large_page_size_mask = l0_large_page_size - 1;

// 2MB pages
const uintptr_t l1_large_page_size = 1UL << MMU_LX_X(MMU_KERNEL_PAGE_SIZE_SHIFT, 2);
const uintptr_t l1_large_page_size_mask = l1_large_page_size - 1;

static size_t vaddr_to_l0_index(uintptr_t addr) {
    return (addr >> MMU_LX_X(MMU_KERNEL_PAGE_SIZE_SHIFT, 1)) & (MMU_KERNEL_PAGE_TABLE_ENTRIES - 1);
}

static size_t vaddr_to_l1_index(uintptr_t addr) {
    return (addr >> MMU_LX_X(MMU_KERNEL_PAGE_SIZE_SHIFT, 2)) & (MMU_KERNEL_PAGE_TABLE_ENTRIES - 1);
}

static size_t vaddr_to_l2_index(uintptr_t addr) {
    return (addr >> MMU_LX_X(MMU_KERNEL_PAGE_SIZE_SHIFT, 3)) & (MMU_KERNEL_PAGE_TABLE_ENTRIES - 1);
}

// called from start.S to grab another page to back a page table from the boot allocator
__NO_SAFESTACK
extern "C" pte_t* boot_alloc_ptable() {
    // allocate a page out of the boot allocator, asking for a physical address
    pte_t* ptr = reinterpret_cast<pte_t*>(boot_alloc_page_phys());

    // avoid using memset, since this relies on dc zva instruction, which isn't set up at
    // this point in the boot process
    // use a volatile pointer to make sure
    volatile pte_t* vptr = ptr;
    for (auto i = 0; i < MMU_KERNEL_PAGE_TABLE_ENTRIES; i++)
        vptr[i] = 0;

    return ptr;
}

/**
 * LoongArch normal Page Table Entry diagram
 * +------+----+----+--------+------------+----+---+---+---+-----+-----+---+---+
 * | RPLV | NX | NR |        |     PA     |    | W | P | G | MAT | PLV | D | V |
 * +------+----+----+--------+------------+----+---+---+---+-----+-----+---+---+
 * |    63|  62|  61|   PALEN|          12|   9|  8|  7|  6|    4|    2|  1|  0|
 * 
 */

// inner mapping routine passed two helper routines
__NO_SAFESTACK
static inline zx_status_t _loongarch64_boot_map(pte_t* kernel_table0,
                                          const vaddr_t vaddr,
                                          const paddr_t paddr,
                                          const size_t len,
                                          const pte_t flags,
                                          paddr_t (*alloc_func)(),
                                          pte_t* phys_to_virt(paddr_t)) {

    // loop through the virtual range and map each physical page, using the largest
    // page size supported. Allocates necessar page tables along the way.
    size_t off = 0;
    while (off < len) {
        // make sure the level 0 pointer is valid
        size_t index0 = vaddr_to_l0_index(vaddr + off);

        if (kernel_table0[index0] == 0) {
            // invalid/unused entry
            paddr_t pa = alloc_func();
            kernel_table0[index0] = (pa & MMU_PTE_OUTPUT_ADDR_MASK);
        }

        pte_t* kernel_table1 = phys_to_virt(kernel_table0[index0] & MMU_PTE_OUTPUT_ADDR_MASK);


        // make sure the level 1 pointer is valid
        size_t index1 = vaddr_to_l1_index(vaddr + off);

        if (kernel_table1[index1] == 0) {
            // invalid/unused entry
            paddr_t pa = alloc_func();
            kernel_table1[index1] = (pa & MMU_PTE_OUTPUT_ADDR_MASK);
        }
      
        pte_t* kernel_table2 = phys_to_virt(kernel_table1[index1] & MMU_PTE_OUTPUT_ADDR_MASK);

        // generate a standard page mapping
        size_t index2 = vaddr_to_l2_index(vaddr + off);
        kernel_table2[index2] = ((paddr + off) & MMU_PTE_OUTPUT_ADDR_MASK) | flags |
                                (_PAGE_VALID | _PAGE_WRITE | _PAGE_KERN | _PAGE_PRESENT);

        off += PAGE_SIZE;
    }

    return ZX_OK;
}


// called from start.S to configure level 1-3 page tables to map the kernel wherever it is located physically
// to KERNEL_BASE
__NO_SAFESTACK
extern "C" zx_status_t loongarch64_boot_map(pte_t* kernel_table0,
                                           const vaddr_t vaddr,
                                           const paddr_t paddr,
                                           const size_t len,
                                           const pte_t flags) {

    // the following helper routines assume that code is running in physical addressing mode (mmu off).
    // any physical addresses calculated are assumed to be the same as virtual
    auto alloc = []() -> paddr_t {
        // allocate a page out of the boot allocator, asking for a physical address
        paddr_t pa = boot_alloc_page_phys();

        // avoid using memset, since this relies on dc zva instruction, which isn't set up at
        // this point in the boot process
        // use a volatile pointer to make sure the compiler doesn't emit a memset call
        volatile pte_t* vptr = reinterpret_cast<volatile pte_t*>(pa);
        for (auto i = 0; i < MMU_KERNEL_PAGE_TABLE_ENTRIES; i++)
            vptr[i] = 0;

        return pa;
    };

    auto phys_to_virt = [](paddr_t pa) -> pte_t* {
        return reinterpret_cast<pte_t*>(pa);
    };

    return _loongarch64_boot_map(kernel_table0, vaddr, paddr, len, flags, alloc, phys_to_virt);
}

// called a bit later in the boot process once the kernel is in virtual memory to map early kernel data
extern "C" zx_status_t loongarch64_boot_map_v(const vaddr_t vaddr,
                                        const paddr_t paddr,
                                        const size_t len,
                                        const pte_t flags) {

    // assumed to be running with virtual memory enabled, so use a slightly different set of routines
    // to allocate and find the virtual mapping of memory
    auto alloc = []() -> paddr_t {
        // allocate a page out of the boot allocator, asking for a physical address
        paddr_t pa = boot_alloc_page_phys();

        // zero the memory using the physmap
        void* ptr = paddr_to_physmap(pa & (ARCH_PHYSMAP_SIZE -1));
        memset(ptr, 0, MMU_KERNEL_PAGE_TABLE_ENTRIES * sizeof(pte_t));

        return pa;
    };

    auto phys_to_virt = [](paddr_t pa) -> pte_t* {
        return reinterpret_cast<pte_t*>(paddr_to_physmap(pa));
    };

    return _loongarch64_boot_map(loongarch64_get_kernel_ptable(), vaddr, paddr, len, flags, alloc, phys_to_virt);
}


extern pte_t loongarch64_kernel_translation_table[MMU_KERNEL_PAGE_TABLE_ENTRIES_TOP] __ALIGNED(MMU_KERNEL_PAGE_TABLE_ENTRIES_TOP * 8);
extern uint64_t kernel_relocated_base;

extern "C" zx_status_t loongarch64_map_kernel_aspace() {
    // map identity phys addr space
    loongarch64_boot_map(
        loongarch64_kernel_translation_table,
        0,
        0,
        ARCH_PHYSMAP_SIZE,
        _PAGE_VALID | _PAGE_WRITE | _PAGE_DIRTY | _PAGE_NO_EXEC | _PAGE_KERN // TODO: which flag to use?
    );

    // map kernel code
    loongarch64_boot_map(
        loongarch64_kernel_translation_table,
        kernel_relocated_base,
        ((paddr_t)__code_start) & UINT32_MAX,
        _end - __code_start,
        _PAGE_VALID | _PAGE_WRITE | _PAGE_DIRTY | _PAGE_KERN | _CACHE_CC // TODO: which flag to use?
    );

    // map kernel virt addr space
    loongarch64_boot_map(
        loongarch64_kernel_translation_table,
        KERNEL_ASPACE_BASE,
        0,
        ARCH_PHYSMAP_SIZE,
        _PAGE_VALID | _PAGE_WRITE | _PAGE_DIRTY | _PAGE_NO_EXEC | _PAGE_KERN // TODO: which flag to use?
    );

    return ZX_OK;
}