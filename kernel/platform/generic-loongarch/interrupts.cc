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
