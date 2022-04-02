#include <arch.h>
#include <kernel/thread.h>
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
//  arm64_uspace_entry(iframe, ct->stack.top);
  TODO();
  __UNREACHABLE;
}
