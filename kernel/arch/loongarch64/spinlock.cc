#include <arch.h>
#include <arch/ops.h>
#include <arch/spinlock.h>
#include <kernel/atomic.h>

void arch_spin_lock(spin_lock_t* lock) TA_NO_THREAD_SAFETY_ANALYSIS {
    unsigned long val = arch_curr_cpu_num() + 1;
    uint64_t temp;

    __asm__ volatile(
        "1:"                         // spin loop here
        "ll.d  %[temp], %[lock], 0;" // load link from %lock
        "bnez  %[temp], 1b;"         // %lock != 0 <==> some core holding it
        "move  %[temp], %[val];"     // prepare val to write
        "sc.d  %[temp], %[lock], 0;" // store cond to %lock
        "beqz  %[temp], 1b;"         // %temp == 0 <==> SC failed, need retry
        : [temp] "=&r"(temp)
        : [lock] "r"(&lock->value), [val] "r"(val)
        : "cc", "memory");
    WRITE_PERCPU_FIELD32(num_spinlocks, READ_PERCPU_FIELD32(num_spinlocks) + 1);
}

int arch_spin_trylock(spin_lock_t* lock) TA_NO_THREAD_SAFETY_ANALYSIS {
    TODO();
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
    TODO();
}
