#include <arch/ops.h>
#include <arch/spinlock.h>
#include <kernel/atomic.h>

void arch_spin_lock(spin_lock_t* lock) TA_NO_THREAD_SAFETY_ANALYSIS {

}

int arch_spin_trylock(spin_lock_t* lock) TA_NO_THREAD_SAFETY_ANALYSIS {
//  unsigned long val = arch_curr_cpu_num() + 1;
//  uint64_t out;
//
//  __asm__ volatile(
//    "ldaxr   %[out], [%[lock]];"
//    "cbnz    %[out], 1f;"
//    "stxr    %w[out], %[val], [%[lock]];"
//    "1:"
//    : [out] "=&r"(out)
//    : [lock] "r"(&lock->value), [val] "r"(val)
//    : "cc", "memory");
//
//  if (out == 0) {
//    WRITE_PERCPU_FIELD32(num_spinlocks, READ_PERCPU_FIELD32(num_spinlocks) + 1);
//  }
//  return (int)out;
  return 0;
}

void arch_spin_unlock(spin_lock_t* lock) TA_NO_THREAD_SAFETY_ANALYSIS {

}
