#include <arch.h>
#include <arch/hypervisor.h>
#include <zircon/syscalls/hypervisor.h>

zx_status_t Vcpu::Create(Guest* guest, zx_vaddr_t entry, ktl::unique_ptr<Vcpu>* out) {
    TODO();
    return ZX_OK;
}

zx_status_t Vcpu::Resume(zx_port_packet_t* packet) {
    TODO();
    return ZX_OK;
}

cpu_mask_t Vcpu::Interrupt(uint32_t vector, hypervisor::InterruptType type) {
    TODO();
    return 0;
}

void Vcpu::VirtualInterrupt(uint32_t vector) {
    TODO();
}

zx_status_t Vcpu::ReadState(uint32_t kind, void* buf, size_t len) const {
    TODO();
    return ZX_OK;
}

zx_status_t Vcpu::WriteState(uint32_t kind, const void* buf, size_t len) {
    TODO();
    return ZX_OK;
}
