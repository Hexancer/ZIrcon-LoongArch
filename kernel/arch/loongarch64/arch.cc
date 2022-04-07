#include <arch.h>
#include <arch/loongarch64/feature.h>
#include <kernel/thread.h>
#include <platform.h>
#include <zircon/types.h>

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
  TODO();
}

__NO_RETURN int arch_idle_thread_routine(void*) {
    for (;;)
        __asm__ volatile("idle 0");
}