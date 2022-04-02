#pragma once

__BEGIN_CDECLS

/* use the cpu local thread context pointer to store current_thread */
static inline struct thread* get_current_thread(void) {
    void* tp;
    __asm__ __volatile__("move $tp, %0"
                         : "=r"(tp));
    return (struct thread*)tp;
}

__END_CDECLS