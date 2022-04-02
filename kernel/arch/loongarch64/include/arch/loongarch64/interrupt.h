#pragma once

#ifndef __ASSEMBLER__

#include <arch.h>
#include <kernel/atomic.h>

__BEGIN_CDECLS

static inline void arch_enable_ints(void) {
    TODO();
}

static inline void arch_disable_ints(void) {
    TODO();
}

static inline bool arch_ints_disabled(void) {
    TODO();
    return true;
}

__END_CDECLS

#endif // __ASSEMBLER__
