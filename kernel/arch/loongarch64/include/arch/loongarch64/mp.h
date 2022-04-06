#pragma once

#include <arch.h>
#include <arch/loongarch64.h>
#include <kernel/align.h>
#include <kernel/cpu.h>


__BEGIN_CDECLS


#define READ_PERCPU_FIELD32(field) (__builtin_trap(),0)
#define WRITE_PERCPU_FIELD32(field, value) __builtin_trap()

struct loongarch64_percpu {
    // cpu number
    uint32_t cpu_num;

    // Whether blocking is disallowed.  See arch_blocking_disallowed().
    uint32_t blocking_disallowed;

    // Number of spinlocks currently held.
    uint32_t num_spinlocks;
} __CPU_ALIGN;


// Use the x21 register to always point at the local cpu structure to allow fast access
// a per cpu structure.
// Do not directly access fields of this structure
register struct loongarch64_percpu* __loongarch64_percpu __asm("r21");

static inline void loongarch64_write_percpu_ptr(struct loongarch64_percpu* percpu) {
    __loongarch64_percpu = percpu;
}

static inline struct loongarch64_percpu* loongarch64_read_percpu_ptr(void) {
    return __loongarch64_percpu;
}

static inline cpu_num_t arch_curr_cpu_num(void) {
    return 0;
}

static inline uint arch_max_num_cpus(void) {
    return 1;
}

__END_CDECLS
