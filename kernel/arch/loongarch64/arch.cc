#include <arch.h>
#include <arch/loongarch64/feature.h>
#include <arch/mp.h>
#include <debug.h>
#include <kernel/cmdline.h>
#include <kernel/thread.h>
#include <lk/main.h>
#include <platform.h>
#include <trace.h>
#include <zircon/types.h>

#define LOCAL_TRACE 0

// Used to hold up the boot sequence on secondary CPUs until signaled by the primary.
static int secondaries_released = 0;

static volatile int secondaries_to_init = 0;

void arch_setup_uspace_iframe(iframe_t* iframe,
                              uintptr_t pc, uintptr_t sp,
                              uintptr_t arg1, uintptr_t arg2) {
//  // Set up a default spsr to get into 64bit user space:
//  //  - Zeroed NZCV.
//  //  - No SS, no IL, no D.
//  //  - All interrupts enabled.
//  //  - Mode 0: EL0t.
//  //
//  // TODO: (hollande,travisg) Need to determine why some platforms throw an
//  //         SError exception when first switching to uspace.
//  uint32_t spsr = 1 << 8;  // Mask SError exceptions (currently unhandled).
//
//  iframe->r[0] = arg1;
//  iframe->r[1] = arg2;
//  iframe->usp = sp;
//  iframe->elr = pc;
//  iframe->spsr = spsr;
//
//  iframe->mdscr = MSDCR_EL1_INITIAL_VALUE;
  TODO();
}

void arch_enter_uspace(iframe_t* iframe) {
//  thread_t* ct = get_current_thread();
//
//  LTRACEF("arm_uspace_entry(%#" PRIxPTR ", %#" PRIxPTR ", %#" PRIxPTR
//  ", %#" PRIxPTR ", %#" PRIxPTR ", 0, %#" PRIxPTR ")\n",
//      iframe->r[0], iframe->r[1], iframe->spsr, ct->stack.top,
//      iframe->usp, iframe->elr);
//
//  arch_disable_ints();
//
//  loongarch64_uspace_entry(iframe, ct->stack.top);
  TODO();
  __UNREACHABLE;
}

static void loongarch64_cpu_early_init() {
    // Make sure the per cpu pointer is set up.
    loongarch64_init_percpu_early();


    // Set the vector base.
    // TODO: switch to vectored exceptions

    // Save all of the features of the cpu.
    loongarch64_feature_init();

}

void arch_early_init() {
    loongarch64_cpu_early_init();

    platform_init_mmu_mappings();
}

void arch_init() TA_NO_THREAD_SAFETY_ANALYSIS {
    arch_mp_init_percpu();

    loongarch64_feature_debug(true);

    uint32_t max_cpus = arch_max_num_cpus();
    uint32_t cmdline_max_cpus = cmdline_get_uint32("kernel.smp.maxcpus", max_cpus);
    if (cmdline_max_cpus > max_cpus || cmdline_max_cpus <= 0) {
        printf("invalid kernel.smp.maxcpus value, defaulting to %u\n", max_cpus);
        cmdline_max_cpus = max_cpus;
    }

    secondaries_to_init = cmdline_max_cpus - 1;

    lk_init_secondary_cpus(secondaries_to_init);

    LTRACEF("releasing %d secondary cpus\n", secondaries_to_init);
    atomic_store(&secondaries_released, 1);

    // Flush the signaling variable since the secondary cpus may have not yet enabled their caches.
    arch_clean_cache_range((addr_t)&secondaries_released, sizeof(secondaries_released));
}

__NO_RETURN int arch_idle_thread_routine(void*) {
    for (;;)
        __asm__ volatile("idle 0");
}

struct _arch_cas_16 {};

bool arch_cas_16_acquire(volatile unsigned __int128* dst,
                         volatile unsigned __int128* expected,
                         unsigned __int128 desired) {

    static DECLARE_MUTEX(_arch_cas_16) lock_;

    // TODO: We assume that all 16B CAS op will use this routine,
    //       so add a mutex here should suffice
    Guard<Mutex> _cas{&lock_};

    unsigned __int128 temp = *dst;
    if (temp != *expected) {
        return false;
    } else {
        *dst = desired;
        return true;
    }
}