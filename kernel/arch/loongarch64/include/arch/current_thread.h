#pragma once

#include <arch.h>
#include <arch/defines.h>
#include <arch/loongarch64/csr.h>

__BEGIN_CDECLS

/* use the cpu local thread context pointer to store current_thread */
static inline struct thread* get_current_thread(void) {
    char* tp = (char *) csr_readq(LOONGARCH_CSR_KS3);
    tp -= offsetof(struct thread, arch.thread_pointer_location);
    return (struct thread*)tp;
}

static inline void set_current_thread(struct thread* t) {
    csr_writeq(((uint64_t)&t->arch.thread_pointer_location), LOONGARCH_CSR_KS3);
}

__END_CDECLS