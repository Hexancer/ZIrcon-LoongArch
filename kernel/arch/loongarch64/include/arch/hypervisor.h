#pragma once

#include <arch.h>
#include <zircon/types.h>
#include <hypervisor/guest_physical_address_space.h>
#include <hypervisor/id_allocator.h>
#include <hypervisor/interrupt_tracker.h>
#include <hypervisor/trap_map.h>


class Guest {
public:
    static zx_status_t Create(ktl::unique_ptr<Guest>* out);
//     ~Guest();
//     DISALLOW_COPY_ASSIGN_AND_MOVE(Guest);

    zx_status_t SetTrap(uint32_t kind, zx_vaddr_t addr, size_t len,
                        fbl::RefPtr<PortDispatcher> port, uint64_t key);

    hypervisor::GuestPhysicalAddressSpace* AddressSpace() const { TODO(); return gpas_.get(); }
//     hypervisor::TrapMap* Traps() { return &traps_; }
//     uint8_t Vmid() const { return vmid_; }

//     zx_status_t AllocVpid(uint8_t* vpid);
//     zx_status_t FreeVpid(uint8_t vpid);

private:
    ktl::unique_ptr<hypervisor::GuestPhysicalAddressSpace> gpas_;
//     hypervisor::TrapMap traps_;
//     const uint8_t vmid_;

//     DECLARE_MUTEX(Guest) vcpu_mutex_;
//     // TODO(alexlegg): Find a good place for this constant to live (max vcpus).
//     hypervisor::IdAllocator<uint8_t, 8> TA_GUARDED(vcpu_mutex_) vpid_allocator_;

//     explicit Guest(uint8_t vmid);
};


class Vcpu {
public:
    static zx_status_t Create(Guest* guest, zx_vaddr_t entry, ktl::unique_ptr<Vcpu>* out);
//     ~Vcpu();
//     DISALLOW_COPY_ASSIGN_AND_MOVE(Vcpu);

    zx_status_t Resume(zx_port_packet_t* packet);
    cpu_mask_t Interrupt(uint32_t vector, hypervisor::InterruptType type);
    void VirtualInterrupt(uint32_t vector);
    zx_status_t ReadState(uint32_t kind, void* buf, size_t len) const;
    zx_status_t WriteState(uint32_t kind, const void* buf, size_t len);

// private:
//     Guest* guest_;
//     const uint8_t vpid_;
//     const thread_t* thread_;
//     ktl::atomic<bool> running_;
//     // We allocate El2State in its own page as it is passed between EL1 and EL2,
//     // which have different address space mappings. This ensures that El2State
//     // will not cross a page boundary and be incorrectly accessed in EL2.
//     hypervisor::PagePtr<El2State> el2_state_;
//     GichState gich_state_;
//     uint64_t hcr_;

//     Vcpu(Guest* guest, uint8_t vpid, const thread_t* thread);
};
