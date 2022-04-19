#pragma once

#include <arch.h>
#include <arch/loongarch64/debug.h>

__BEGIN_CDECLS

register uint8_t* __loongarch64_tp __asm("tp");

/* use the cpu local thread context pointer to store current_thread */
static inline struct thread* get_current_thread(void) {
    uint8_t* tp = __loongarch64_tp - offsetof(struct thread, arch.thread_pointer_location);
    return (struct thread*)tp;
}

static inline void set_current_thread(struct thread* t) {
    __loongarch64_tp = (uint8_t*)&t->arch.thread_pointer_location;
}


static inline void loongarch64_save_thread_pointer(void) {
    csr_writeq((uint64_t)__loongarch64_tp, LOONGARCH_CSR_KS4);
}

static inline void loongarch64_restore_thread_pointer(void) {
    __loongarch64_tp = (uint8_t*) csr_readq(LOONGARCH_CSR_KS4);
}

__END_CDECLS