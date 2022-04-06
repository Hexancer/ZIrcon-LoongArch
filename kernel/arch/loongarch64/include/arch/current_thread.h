#pragma once

#include <arch.h>
__BEGIN_CDECLS

/* use the cpu local thread context pointer to store current_thread */
static inline struct thread* get_current_thread(void) {
    // TODO: do we need stack_guard like arm64
    void* tp;
    __asm__ __volatile__("move %0, $tp" : "=r"(tp));
    return (struct thread*)tp;
}

static inline void set_current_thread(struct thread* t) {
    __asm__ __volatile__("move $tp, %0" : : "r"(t));
}

__END_CDECLS