#pragma once

#ifndef __ASSEMBLER__

#include <arch.h>
#include <arch/loongarch64.h>
#include <arch/loongarch64/feature.h>
#include <arch/loongarch64/interrupt.h>
#include <arch/loongarch64/mp.h>
#include <zircon/compiler.h>

__BEGIN_CDECLS

static inline void arch_spinloop_pause(void) {
    __asm__ volatile("nop" ::: "memory");
}

#define __sync()	__asm__ __volatile__("dbar 0" : : : "memory")

#define fast_wmb()	__sync()
#define fast_rmb()	__sync()
#define fast_mb()	__sync()
#define fast_iob()	__sync()
#define wbflush()	__sync()

#define wmb()		fast_wmb()
#define rmb()		fast_rmb()
#define mb()		fast_mb()
#define iob()		fast_iob()

#define __smp_mb()	__asm__ __volatile__("dbar 0" : : : "memory")
#define __smp_rmb()	__asm__ __volatile__("dbar 0" : : : "memory")
#define __smp_wmb()	__asm__ __volatile__("dbar 0" : : : "memory")
#define smp_mb()    __smp_mb()


static inline uint64_t arch_cycle_count(void) {
    TODO();
    return 0;
}

static inline uint32_t arch_cpu_features(void) {
    return loongarch64_features;
}

static inline uint32_t arch_dcache_line_size(void) {
    return loongarch64_dcache_size;
}

static inline uint32_t arch_icache_line_size(void) {
    return loongarch64_icache_size;
}

// Log architecture-specific data for process creation.
// This can only be called after the process has been created and before
// it is running. Alas we can't use zx_koid_t here as the arch layer is at a
// lower level than zircon.
static inline void arch_trace_process_create(uint64_t pid, paddr_t tt_phys) {
    // nothing to do
}

bool arch_cas_16_acquire(volatile unsigned __int128* dst,
                         volatile unsigned __int128* expected,
                         unsigned __int128 desired);

__END_CDECLS
#endif