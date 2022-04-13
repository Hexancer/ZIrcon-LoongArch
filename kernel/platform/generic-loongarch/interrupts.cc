#include <arch.h>
#include <dev/interrupt.h>
#include <zircon/types.h>

zx_status_t mask_interrupt(unsigned int vector) {
    TODO();
    return ZX_OK;
}
zx_status_t unmask_interrupt(unsigned int vector) {
    TODO();
    return ZX_OK;
}

zx_status_t register_int_handler(unsigned int vector, int_handler handler, void* arg) {
    TODO();
//  if (!is_valid_interrupt(vector, 0)) {
//    return ZX_ERR_INVALID_ARGS;
//  }
//
//  AutoSpinLock guard(&lock);
//
//  auto h = pdev_get_int_handler(vector);
//  if (handler && h->handler) {
//    return ZX_ERR_ALREADY_BOUND;
//  }
//  h->handler = handler;
//  h->arg = arg;

    return ZX_OK;
}


void platform_irq(loongarch64_iframe_t* frame) {
    TODO();
    // // get the current vector
    // uint64_t x86_vector = frame->vector;
    // DEBUG_ASSERT(x86_vector >= X86_INT_PLATFORM_BASE && x86_vector <= X86_INT_PLATFORM_MAX);

    // // deliver the interrupt
    // kInterruptManager.InvokeX86Vector(static_cast<uint8_t>(x86_vector));

    // // NOTE: On x86, we always deactivate the interrupt.
    // apic_issue_eoi();
}


void interrupt_init_percpu() {
    TODO();
}