#include <arch.h>
#include <arch/arch_ops.h>
#include <arch/loongarch64.h>
#include <arch/exception.h>
#include <arch/user_copy.h>

#include <kernel/interrupt.h>
#include <kernel/thread.h>

#include <zircon/syscalls/exception.h>
#include <zircon/types.h>

/* called from assembly */
extern "C" void arch_iframe_process_pending_signals(iframe_t* iframe) {
//  DEBUG_ASSERT(iframe != nullptr);
//  thread_t* thread = get_current_thread();
//  if (unlikely(thread_is_signaled(thread))) {
//    DEBUG_ASSERT(thread->arch.suspended_general_regs == nullptr);
//
//    thread->arch.suspended_general_regs = iframe;
//    thread_process_pending_signals();
//    thread->arch.suspended_general_regs = nullptr;
//  }
  TODO();
}

void arch_fill_in_exception_context(const arch_exception_context_t* arch_context, zx_exception_report_t* report) {
//  zx_exception_context_t* zx_context = &report->context;
//
//  zx_context->arch.u.arm_64.esr = arch_context->esr;
//
//  // If there was a fatal page fault, fill in the address that caused the fault.
//  if (ZX_EXCP_FATAL_PAGE_FAULT == report->header.type) {
//    zx_context->arch.u.arm_64.far = arch_context->far;
//  } else {
//    zx_context->arch.u.arm_64.far = 0;
//  }
  TODO();
}

void arch_install_context_regs(thread_t* thread, const arch_exception_context_t* context) {
//  // TODO(ZX-563): |context->frame| will be nullptr for exceptions that
//  // don't (yet) provide the registers.
//  if (context->frame) {
//    DEBUG_ASSERT(thread->arch.suspended_general_regs == nullptr);
//    thread->arch.suspended_general_regs = context->frame;
//    thread->arch.debug_state.esr = context->esr;
//  }
  TODO();
}

void arch_remove_context_regs(thread_t* thread) {
//  thread->arch.suspended_general_regs = nullptr;
  TODO();
}
