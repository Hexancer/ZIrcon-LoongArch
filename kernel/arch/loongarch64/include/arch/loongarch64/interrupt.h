#pragma once

#ifndef __ASSEMBLER__

#include <kernel/atomic.h>

__BEGIN_CDECLS

static inline void arch_enable_ints(void) {}

static inline void arch_disable_ints(void) {}

static inline bool arch_ints_disabled(void) {
        return true;
}

__END_CDECLS

#endif // __ASSEMBLER__
