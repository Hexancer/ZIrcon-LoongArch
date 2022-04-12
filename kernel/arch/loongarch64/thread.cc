#include <arch.h>
#include <arch/loongarch64.h>
#include <arch/loongarch64/mp.h>
#include <debug.h>
#include <kernel/thread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

#define LOCAL_TRACE 0

void arch_thread_initialize(thread_t* t, vaddr_t entry_point) {
    // zero out the entire arch state
    t->arch = {};

    // create a default stack frame on the stack
    vaddr_t stack_top = t->stack.top;

    // make sure the top of the stack is 16 byte aligned for EABI compliance
    stack_top = ROUNDDOWN(stack_top, 16);
    t->stack.top = stack_top;

    struct loongarch64_context_switch_frame* frame = (struct loongarch64_context_switch_frame*)(stack_top);
    frame--;

    // fill in the entry point
    frame->ra = entry_point;

    // This is really a global (boot-time) constant value.
    // But it's stored in each thread struct to satisfy the
    // compiler ABI (TPIDR_EL1 + ZX_TLS_STACK_GUARD_OFFSET).
    t->arch.stack_guard = get_current_thread()->arch.stack_guard;

    // set the stack pointer
    t->arch.sp = (vaddr_t)frame;
    #if __has_feature(safe_stack)
    t->arch.unsafe_sp =
        ROUNDDOWN(t->stack.unsafe_base + t->stack.size, 16);
    #endif

    // // Initialize the debug state to a valid initial state.
    // for (size_t i = 0; i < ARM64_MAX_HW_BREAKPOINTS; i++) {
    // t->arch.debug_state.hw_bps[i].dbgbcr = 0;
    // t->arch.debug_state.hw_bps[i].dbgbvr = 0;
    // }
}

__NO_SAFESTACK void arch_thread_construct_first(thread_t* t) {
    // make sure the thread saves a copy of the current cpu pointer
    t->arch.current_percpu_ptr = loongarch64_read_percpu_ptr();

    // Force the thread pointer immediately to the real struct.  This way
    // our callers don't have to avoid safe-stack code or risk losing track
    // of the unsafe_sp value.  The caller's unsafe_sp value is visible at
    // TPIDR_EL1 + ZX_TLS_UNSAFE_SP_OFFSET as expected, though TPIDR_EL1
    // happens to have changed.  (We're assuming that the compiler doesn't
    // decide to cache the TPIDR_EL1 value across this function call, which
    // would be pointless since it's just one instruction to fetch it afresh.)
    set_current_thread(t);
}

__NO_SAFESTACK void arch_context_switch(thread_t* oldthread,
                                        thread_t* newthread) {
    LTRACEF("old %p (%s), new %p (%s)\n", oldthread, oldthread->name, newthread, newthread->name);
    smp_mb(); /* broadcast tlb operations in case the thread moves to another cpu */

    /* set the current cpu pointer in the new thread's structure so it can be
     * restored on exception entry.
     */
    newthread->arch.current_percpu_ptr = loongarch64_read_percpu_ptr();

    // arm64_fpu_context_switch(oldthread, newthread);
    // arm64_debug_state_context_switch(oldthread, newthread);
    loongarch64_context_switch(&oldthread->arch.sp, newthread->arch.sp);
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
