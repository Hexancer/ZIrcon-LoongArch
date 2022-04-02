#pragma once

#include <arch/loongarch64.h>
#include <kernel/cpu.h>


__BEGIN_CDECLS


#define READ_PERCPU_FIELD32(field) 0
#define WRITE_PERCPU_FIELD32(field, value)

static inline cpu_num_t arch_curr_cpu_num(void) {
    return 0;
}

static inline uint arch_max_num_cpus(void) {
    return 0;
}

__END_CDECLS
