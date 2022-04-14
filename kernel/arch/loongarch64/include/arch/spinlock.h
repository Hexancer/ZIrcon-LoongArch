#pragma once

#include <arch.h>
#include <arch/loongarch64/interrupt.h>
#include <arch/loongarch64/mp.h>
#include <lib/zircon-internal/thread_annotations.h>

__BEGIN_CDECLS

#define SPIN_LOCK_INITIAL_VALUE \
    (spin_lock_t) { 0 }

typedef struct TA_CAP("mutex") spin_lock {
    unsigned long value;
} spin_lock_t;

typedef unsigned int spin_lock_saved_state_t;
typedef unsigned int spin_lock_save_flags_t;

void arch_spin_lock(spin_lock_t* lock) TA_ACQ(lock);
int arch_spin_trylock(spin_lock_t* lock) TA_TRY_ACQ(false, lock);
void arch_spin_unlock(spin_lock_t* lock) TA_REL(lock);

static inline uint arch_spin_lock_holder_cpu(spin_lock_t* lock) {
    return (uint)__atomic_load_n(&lock->value, __ATOMIC_RELAXED) - 2;
}

static inline bool arch_spin_lock_held(spin_lock_t* lock) {
    return arch_spin_lock_holder_cpu(lock) == arch_curr_cpu_num();
}

enum {
    SPIN_LOCK_FLAG_IRQ = 0x40000000,
    ARCH_DEFAULT_SPIN_LOCK_FLAG_INTERRUPTS = SPIN_LOCK_FLAG_IRQ
};

enum {
    /* private */
    SPIN_LOCK_STATE_RESTORE_IRQ = 1,
};

static inline void
arch_interrupt_save(spin_lock_saved_state_t* statep, spin_lock_save_flags_t flags) {
    spin_lock_saved_state_t state = 0;
    if ((flags & SPIN_LOCK_FLAG_IRQ) && !arch_ints_disabled()) {
        state |= SPIN_LOCK_STATE_RESTORE_IRQ;
        arch_disable_ints();
    }
    *statep = state;
}

static inline void
arch_interrupt_restore(spin_lock_saved_state_t old_state, spin_lock_save_flags_t flags) {
    if ((flags & SPIN_LOCK_FLAG_IRQ) && (old_state & SPIN_LOCK_STATE_RESTORE_IRQ))
        arch_enable_ints();
}

__END_CDECLS
