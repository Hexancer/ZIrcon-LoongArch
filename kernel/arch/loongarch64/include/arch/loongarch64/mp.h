#pragma once

#include <arch.h>
#include <arch/loongarch64.h>
#include <kernel/align.h>
#include <kernel/cpu.h>

__BEGIN_CDECLS

struct loongarch64_percpu {
    // cpu number
    uint32_t cpu_num;

    // Whether blocking is disallowed.  See arch_blocking_disallowed().
    uint32_t blocking_disallowed;

    // Number of spinlocks currently held.
    uint32_t num_spinlocks;
} __CPU_ALIGN;

void arch_init_cpu_map(uint cluster_count, const uint* cluster_cpus);
void arch_register_mpid(uint cpu_id, uint64_t mpid);
void loongarch64_init_percpu_early(void);

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

static inline uint32_t loongarch64_read_percpu_u32(size_t offset) {
    uint32_t val;

    // mark as volatile to force a read of the field to make sure
    // the compiler always emits a read when asked and does not cache
    // a copy between
    __asm__ volatile("ld.w  %[val], $r21, %[offset]"
                     : [val] "=r"(val)
                     : [offset] "Ir"(offset));
    return val;
}

static inline void loongarch64_write_percpu_u32(size_t offset, uint32_t val) {
    __asm__("st.w  %[val], $r21, %[offset]"
            ::[val] "r"(val), [offset] "Ir"(offset)
            : "memory");
}

static inline cpu_num_t arch_curr_cpu_num(void) {
    return 0;
}

static inline uint arch_max_num_cpus(void) {
    return 1;
}

#define READ_PERCPU_FIELD32(field) \
    loongarch64_read_percpu_u32(offsetof(struct loongarch64_percpu, field))

#define WRITE_PERCPU_FIELD32(field, value) \
    loongarch64_write_percpu_u32(offsetof(struct loongarch64_percpu, field), (value))


__END_CDECLS
