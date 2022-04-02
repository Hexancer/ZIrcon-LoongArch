#pragma once

#ifndef __ASSEMBLER__

#include <arch.h>
#include <arch/loongarch64.h>
#include <arch/loongarch64/interrupt.h>
#include <arch/loongarch64/mp.h>
#include <zircon/compiler.h>

__BEGIN_CDECLS

static inline void arch_spinloop_pause(void) {
    TODO();
}

#define mb()
#define smp_mb()

static inline uint64_t arch_cycle_count(void) {
    TODO();
    return 0;
}

static inline uint32_t arch_cpu_features(void) {
    TODO();
    return 0;
}

static inline uint32_t arch_dcache_line_size(void) {
    TODO();
    return 0;
}

static inline uint32_t arch_icache_line_size(void) {
    TODO();
    return 0;
}

static inline void arch_trace_process_create(uint64_t pid, paddr_t tt_phys) {
    TODO();
}

static inline bool arch_cas_16_acquire(volatile unsigned __int128* dst,
                                       volatile unsigned __int128* expected,
                                       unsigned __int128 desired) {
    TODO();
    return true;
}

__END_CDECLS
#endif