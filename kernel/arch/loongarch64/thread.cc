#include <arch.h>
#include <arch/loongarch64.h>
#include <arch/loongarch64/mp.h>
#include <debug.h>
#include <kernel/thread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

void arch_thread_initialize(thread_t* t, vaddr_t entry_point) {
    TODO();
//  // zero out the entire arch state
//  t->arch = {};
//
//  // create a default stack frame on the stack
//  vaddr_t stack_top = t->stack.top;
//
//  // make sure the top of the stack is 16 byte aligned for EABI compliance
//  stack_top = ROUNDDOWN(stack_top, 16);
//  t->stack.top = stack_top;
//
//  struct arm64_context_switch_frame* frame = (struct arm64_context_switch_frame*)(stack_top);
//  frame--;
//
//  // fill in the entry point
//  frame->lr = entry_point;
//
//  // This is really a global (boot-time) constant value.
//  // But it's stored in each thread struct to satisfy the
//  // compiler ABI (TPIDR_EL1 + ZX_TLS_STACK_GUARD_OFFSET).
//  t->arch.stack_guard = get_current_thread()->arch.stack_guard;
//
//  // set the stack pointer
//  t->arch.sp = (vaddr_t)frame;
//#if __has_feature(safe_stack)
//  t->arch.unsafe_sp =
//      ROUNDDOWN(t->stack.unsafe_base + t->stack.size, 16);
//#endif
//
//  // Initialize the debug state to a valid initial state.
//  for (size_t i = 0; i < ARM64_MAX_HW_BREAKPOINTS; i++) {
//    t->arch.debug_state.hw_bps[i].dbgbcr = 0;
//    t->arch.debug_state.hw_bps[i].dbgbvr = 0;
//  }
}

void arch_dump_thread(thread_t* t) {
    TODO();
//  if (t->state != THREAD_RUNNING) {
//    dprintf(INFO, "\tarch: ");
//    dprintf(INFO, "sp 0x%lx\n", t->arch.sp);
//  }
}

void* arch_thread_get_blocked_fp(struct thread* t) {
    TODO();
//  if (!WITH_FRAME_POINTERS)
//    return nullptr;
//
//  struct loongarch64_context_switch_frame* frame = loongarch64_get_context_switch_frame(t);
//  return (void*)frame->r29;
  return nullptr;
}
