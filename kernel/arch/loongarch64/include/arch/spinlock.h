#pragma once

#include <arch/loongarch64/mp.h>
#include <lib/zircon-internal/thread_annotations.h>

__BEGIN_CDECLS

#define SPIN_LOCK_INITIAL_VALUE \
    (spin_lock_t) { 0 }

typedef struct spin_lock {
    unsigned long value;
} spin_lock_t;

typedef unsigned int spin_lock_saved_state_t;
typedef unsigned int spin_lock_save_flags_t;

void arch_spin_lock(spin_lock_t* lock);
int arch_spin_trylock(spin_lock_t* lock);
void arch_spin_unlock(spin_lock_t* lock);

static inline uint arch_spin_lock_holder_cpu(spin_lock_t* lock) {
    return 0;
}

static inline bool arch_spin_lock_held(spin_lock_t* lock) {
    return 0;
}

enum {
    ARCH_DEFAULT_SPIN_LOCK_FLAG_INTERRUPTS
};


static inline void
arch_interrupt_save(spin_lock_saved_state_t* statep, spin_lock_save_flags_t flags) {}

static inline void
arch_interrupt_restore(spin_lock_saved_state_t old_state, spin_lock_save_flags_t flags) {}

__END_CDECLS
