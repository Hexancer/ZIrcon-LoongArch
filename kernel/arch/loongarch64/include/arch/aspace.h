#pragma once

#include <arch/loongarch64/mmu.h>
#include <fbl/mutex.h>
#include <vm/arch_vm_aspace.h>


class LoongarchArchVmAspace final : public ArchVmAspaceInterface {
public:
    virtual ~LoongarchArchVmAspace() {}

    virtual zx_status_t Init(vaddr_t base, size_t size, uint mmu_flags);
    virtual zx_status_t Destroy();
    virtual zx_status_t MapContiguous(vaddr_t vaddr, paddr_t paddr, size_t count,
                                      uint mmu_flags, size_t* mapped);
    virtual zx_status_t Map(vaddr_t vaddr, paddr_t* phys, size_t count,
                            uint mmu_flags, size_t* mapped);
    virtual zx_status_t Unmap(vaddr_t vaddr, size_t count, size_t* unmapped);
    virtual zx_status_t Protect(vaddr_t vaddr, size_t count, uint mmu_flags);
    virtual zx_status_t Query(vaddr_t vaddr, paddr_t* paddr, uint* mmu_flags);
    virtual vaddr_t PickSpot(vaddr_t base, uint prev_region_mmu_flags,
                             vaddr_t end, uint next_region_mmu_flags,
                             vaddr_t align, size_t size, uint mmu_flags);
    static void ContextSwitch(LoongarchArchVmAspace* from, LoongarchArchVmAspace* to);
    paddr_t arch_table_phys() const { return 0; }

private:
    inline bool IsValidVaddr(vaddr_t vaddr) {
        return (vaddr >= base_ && vaddr <= base_ + size_ - 1);
    }

    zx_status_t ProtectPageTable(vaddr_t vaddr_in, vaddr_t vaddr_rel_in, size_t size_in,
                                 pte_t attrs, uint index_shift, uint page_size_shift,
                                 volatile pte_t* page_table) TA_REQ(lock_);

    void MmuParamsFromFlags(uint mmu_flags,
                            pte_t* attrs, vaddr_t* vaddr_base,
                            uint* top_size_shift, uint* top_index_shift,
                            uint* page_size_shift);

    zx_status_t ProtectPages(vaddr_t vaddr, size_t size, pte_t attrs,
                             vaddr_t vaddr_base, uint top_size_shift,
                             uint top_index_shift, uint page_size_shift) TA_REQ(lock_);

    zx_status_t QueryLocked(vaddr_t vaddr, paddr_t* paddr, uint* mmu_flags) TA_REQ(lock_);

    void FlushTLBEntry(vaddr_t vaddr, bool terminal) TA_REQ(lock_);

    fbl::Canary<fbl::magic("VAAS")> canary_;

    DECLARE_MUTEX(LoongarchArchVmAspace) lock_;

    uint16_t asid_ = MMU_LOONGARCH64_UNUSED_ASID;

    // Pointer to the translation table.
    paddr_t tt_phys_ = 0;
    volatile pte_t* tt_virt_ = nullptr;

    // Upper bound of the number of pages allocated to back the translation
    // table.
    size_t pt_pages_ = 0;

    uint flags_ = 0;

    // Range of address space.
    vaddr_t base_ = 0;
    size_t size_ = 0;
};

using ArchVmAspace = LoongarchArchVmAspace;
