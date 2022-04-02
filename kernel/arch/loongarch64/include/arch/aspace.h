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
    virtual paddr_t arch_table_phys() const;
};

using ArchVmAspace = LoongarchArchVmAspace;
