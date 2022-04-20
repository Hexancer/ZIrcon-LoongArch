#include "arch/loongarch64/exceptions.h"

#include <arch.h>
#include <arch/arch_ops.h>
#include <arch/loongarch64.h>
#include <arch/loongarch64/debug.h>
#include <arch/exception.h>
#include <arch/user_copy.h>

#include <inttypes.h>

#include <kernel/interrupt.h>
#include <kernel/thread.h>

#include <trace.h>
#include <vm/fault.h>
#include <vm/vm.h>

#include <lib/counters.h>
#include <lib/crashlog.h>

#include <zircon/syscalls/exception.h>
#include <zircon/types.h>

#define LOCAL_TRACE 0

#define CRMD  (iframe->csr[LOONGARCH_CSR_CRMD])
#define PRMD  (iframe->csr[LOONGARCH_CSR_PRMD])
#define EPC   (iframe->csr[LOONGARCH_CSR_EPC])
#define ECFG  (iframe->csr[LOONGARCH_CSR_ECFG])
#define ESTAT (iframe->csr[LOONGARCH_CSR_ESTAT])
#define BADV  (iframe->csr[LOONGARCH_CSR_BADV])
#define BADI  (iframe->csr[LOONGARCH_CSR_BADI])

static void dump_iframe(const loongarch64_iframe_t* iframe) {
    printf("iframe %p:\n", iframe);
    printf("r0   %#18" PRIx64 " r1   %#18" PRIx64 " r2   %#18" PRIx64 " r3   %#18" PRIx64 "\n", iframe->gpr[0], iframe->gpr[1], iframe->gpr[2], iframe->gpr[3]);
    printf("r4   %#18" PRIx64 " r5   %#18" PRIx64 " r6   %#18" PRIx64 " r7   %#18" PRIx64 "\n", iframe->gpr[4], iframe->gpr[5], iframe->gpr[6], iframe->gpr[7]);
    printf("r8   %#18" PRIx64 " r9   %#18" PRIx64 " r10  %#18" PRIx64 " r11  %#18" PRIx64 "\n", iframe->gpr[8], iframe->gpr[9], iframe->gpr[10], iframe->gpr[11]);
    printf("r12  %#18" PRIx64 " r13  %#18" PRIx64 " r14  %#18" PRIx64 " r15  %#18" PRIx64 "\n", iframe->gpr[12], iframe->gpr[13], iframe->gpr[14], iframe->gpr[15]);
    printf("r16  %#18" PRIx64 " r17  %#18" PRIx64 " r18  %#18" PRIx64 " r19  %#18" PRIx64 "\n", iframe->gpr[16], iframe->gpr[17], iframe->gpr[18], iframe->gpr[19]);
    printf("r20  %#18" PRIx64 " r21  %#18" PRIx64 " r22  %#18" PRIx64 " r23  %#18" PRIx64 "\n", iframe->gpr[20], iframe->gpr[21], iframe->gpr[22], iframe->gpr[23]);
    printf("r24  %#18" PRIx64 " r25  %#18" PRIx64 " r26  %#18" PRIx64 " r27  %#18" PRIx64 "\n", iframe->gpr[24], iframe->gpr[25], iframe->gpr[26], iframe->gpr[27]);
    printf("r28  %#18" PRIx64 " r29  %#18" PRIx64 " rr   %#18" PRIx64 " rsp  %#18" PRIx64 "\n", iframe->gpr[28], iframe->gpr[29], iframe->gpr[30], iframe->gpr[31]);
    printf("crmd %#18" PRIx64 " prmd %#18" PRIx64 " epc  %#18" PRIx64 " ecfg %#18" PRIx64 "\n", CRMD, PRMD, EPC, ECFG);
    printf("estt %#18" PRIx64 " badv %#18" PRIx64 " badi %#18" PRIx64 "\n", ESTAT, BADV, BADI);

    #define ERR(code, msg) case (code): printf(msg "\n"); break;

    int ecode = (ESTAT & CSR_ESTAT_EXC) >> CSR_ESTAT_EXC_SHIFT;

    switch (ecode)
    {
        ERR(0x0, "Interrput");
        ERR(0x1, "Load page invalid");
        ERR(0x2, "Store page invalid");
        ERR(0x3, "Fetch page invalid");
        ERR(0x4, "Page modified exception");
        ERR(0x5, "Page not readable");
        ERR(0x6, "Page not executable");
        ERR(0x7, "Page privilege invalid");
        ERR(0x8, "Address error");
        ERR(0x9, "Address alignment error");
        ERR(0xa, "boundary check fail");
        ERR(0xb, "Syscall");
        ERR(0xc, "Breakpoint");
        ERR(0xd, "Invalid instruction");
        ERR(0xe, "Privilege level error");
        ERR(0xf, "Floating-point disabled");
        default: DEBUG(1, "Unknown cause"); break;
    }
}

KCOUNTER(exceptions_brkpt, "exceptions.breakpoint")
KCOUNTER(exceptions_hw_brkpt, "exceptions.hw_breakpoint")
KCOUNTER(exceptions_hw_wp, "exceptions.hw_watchpoint")
KCOUNTER(exceptions_fpu, "exceptions.fpu")
KCOUNTER(exceptions_page, "exceptions.page_fault")
KCOUNTER(exceptions_irq, "exceptions.irq")
KCOUNTER(exceptions_unhandled, "exceptions.unhandled")
KCOUNTER(exceptions_user, "exceptions.user")
KCOUNTER(exceptions_unknown, "exceptions.unknown")

static zx_status_t try_dispatch_user_data_fault_exception(
    zx_excp_type_t type, loongarch64_iframe_t* iframe) {
    arch_exception_context_t context = {};
    DEBUG_ASSERT(iframe != nullptr);
    context.frame = iframe;

    arch_enable_ints();
    zx_status_t status = dispatch_user_exception(type, &context);
    arch_disable_ints();
    return status;
}

static zx_status_t try_dispatch_user_exception(
    zx_excp_type_t type, loongarch64_iframe_t* iframe) {
    return try_dispatch_user_data_fault_exception(type, iframe);
}

__NO_RETURN static void exception_die(loongarch64_iframe_t* iframe) {
    platform_panic_start();

    /* fatal exception, die here */
    dump_iframe(iframe);
    crashlog.iframe = iframe;

    platform_halt(HALT_ACTION_HALT, HALT_REASON_SW_PANIC);
}


static inline void loongarch64_restore_percpu_pointer() {
    loongarch64_write_percpu_ptr(get_current_thread()->arch.current_percpu_ptr);
}

extern "C" void loongarch64_handle_exception(loongarch64_iframe_t *iframe) {
    int ecode = (ESTAT & CSR_ESTAT_EXC) >> CSR_ESTAT_EXC_SHIFT;

    bool is_user = (PRMD & PLV_MASK) == PLV_USER;
    if (is_user) {
        loongarch64_restore_percpu_pointer();
    }

    if (ecode == CSR_ESTAT_EXC_INT) {
        loongarch64_handle_irq(iframe);
    } else if (CSR_ESTAT_EXC_PIL <= ecode && ecode <= CSR_ESTAT_EXC_PPI) {
        loongarch64_handle_pf(iframe);
    } else {
        DEBUG(1, "Hello from exception handler");
        dump_iframe(iframe);
        while (1) {}
    }
}

extern "C" void loongarch64_handle_pf(iframe_t* iframe) {
    int ecode = (ESTAT & CSR_ESTAT_EXC) >> CSR_ESTAT_EXC_SHIFT;

    uint64_t badv = BADV;
    bool is_user = (PRMD & PLV_MASK) == PLV_USER;

    uint pf_flags = 0;
    if (ecode == CSR_ESTAT_EXC_PIS || ecode == CSR_ESTAT_EXC_PME) {
        pf_flags |= VMM_PF_FLAG_WRITE;
    }
    if (is_user) {
        pf_flags |= VMM_PF_FLAG_USER;
    }
    if (ecode == CSR_ESTAT_EXC_PIF || ecode == CSR_ESTAT_EXC_PNX) {
        pf_flags |= VMM_PF_FLAG_INSTRUCTION;
    }
    if (ecode == CSR_ESTAT_EXC_PIL || ecode == CSR_ESTAT_EXC_PIS || ecode == CSR_ESTAT_EXC_PIF) {
        pf_flags |= VMM_PF_FLAG_NOT_PRESENT;
    }

    arch_enable_ints();
    kcounter_add(exceptions_page, 1);
    CPU_STATS_INC(page_faults);
    zx_status_t err = vmm_page_fault_handler(badv, pf_flags);
    __asm__ __volatile__("invtlb 0x00, $r0, $r0" ::: "memory");
    arch_disable_ints();
    if (err >= 0)
        return;

    // Check if the current thread was expecting a data fault and
    // we should return to its handler.
    thread_t* thr = get_current_thread();
    if (thr->arch.data_fault_resume != NULL && is_user_address(BADV)) {
        EPC = (uintptr_t)thr->arch.data_fault_resume;
        return;
    }

    // If this is from user space, let the user exception handler
    // get a shot at it.
    if (is_user) {
        kcounter_add(exceptions_user, 1);
        if (try_dispatch_user_data_fault_exception(ZX_EXCP_FATAL_PAGE_FAULT, iframe) == ZX_OK)
            return;
    }

    printf("page fault: PC at %#" PRIx64 ", is_user %d, BADV %" PRIx64 "\n", EPC, is_user, badv);
    exception_die(iframe);
}

/* called from assembly */
extern "C" uint32_t loongarch64_handle_irq(iframe_t* iframe) {
    bool is_user = (PRMD & PLV_MASK) == PLV_USER;
    bool is_kernel = (PRMD & PLV_MASK) == PLV_KERN;

    if (is_user) {
        // if we came from a lower level, restore the per cpu pointer
        loongarch64_restore_percpu_pointer();
    }

    int_handler_saved_state_t state;
    int_handler_start(&state);

    kcounter_add(exceptions_irq, 1);
    platform_irq(iframe);

    bool do_preempt = int_handler_finish(&state);

    /* if we came from user space, check to see if we have any signals to handle */
    if (unlikely(is_user) && thread_is_signaled(get_current_thread())) {
        arch_iframe_process_pending_signals(iframe);
    }

    /* preempt the thread if the interrupt has signaled it */
    if (do_preempt)
        thread_preempt();

    /* if we're returning to kernel space, make sure we restore the correct x21 */
    if (is_kernel) {
        iframe->gpr[21] = (uint64_t)loongarch64_read_percpu_ptr();
    }

    return 0;
}

/* called from assembly */
extern "C" void loongarch64_finish_user_irq(uint32_t exit_flags, loongarch64_iframe_t* iframe) {
    // we came from a lower level, so restore the per cpu pointer
    loongarch64_restore_percpu_pointer();

    /* in the case of receiving a kill signal, this function may not return,
     * but the scheduler would have been invoked so it's fine.
     */
    if (unlikely(exit_flags & LOONGARCH64_IRQ_EXIT_THREAD_SIGNALED)) {
        DEBUG_ASSERT(iframe != nullptr);
        arch_iframe_process_pending_signals(iframe);
    }

    /* preempt the thread if the interrupt has signaled it */
    if (exit_flags & LOONGARCH64_IRQ_EXIT_RESCHEDULE)
        thread_preempt();
}

/* called from assembly */
extern "C" void arch_iframe_process_pending_signals(iframe_t* iframe) {
    DEBUG_ASSERT(iframe != nullptr);
    thread_t *thread = get_current_thread();
    if (unlikely(thread_is_signaled(thread))) {
        DEBUG_ASSERT(thread->arch.suspended_general_regs == nullptr);

        thread->arch.suspended_general_regs = iframe;
        thread_process_pending_signals();
        thread->arch.suspended_general_regs = nullptr;
    }
}

void arch_dump_exception_context(const arch_exception_context_t* context) {
    // uint32_t ec = BITS_SHIFT(context->esr, 31, 26);
    // uint32_t iss = BITS(context->esr, 24, 0);

    // switch (ec) {
    // case 0b100000: /* instruction abort from lower level */
    // case 0b100001: /* instruction abort from same level */
    //     printf("instruction abort: PC at %#" PRIx64
    //            ", address %#" PRIx64 " IFSC %#x %s\n",
    //            context->frame->elr, context->far,
    //            BITS(context->esr, 5, 0),
    //            BIT(ec, 0) ? "" : "user ");

    //     break;
    // case 0b100100: /* data abort from lower level */
    // case 0b100101: /* data abort from same level */
    //     printf("data abort: PC at %#" PRIx64
    //            ", address %#" PRIx64 " %s%s\n",
    //            context->frame->elr, context->far,
    //            BIT(ec, 0) ? "" : "user ",
    //            BIT(iss, 6) ? "write" : "read");
    // }

    dump_iframe(context->frame);

    // try to dump the user stack
    if (is_user_address(context->frame->gpr[SP_NUM])) {
        uint8_t buf[256];
        if (arch_copy_from_user(buf, (void*)context->frame->gpr[SP_NUM], sizeof(buf)) == ZX_OK) {
            printf("bottom of user stack at 0x%lx:\n", (vaddr_t)context->frame->gpr[SP_NUM]);
            hexdump_ex(buf, sizeof(buf), context->frame->gpr[SP_NUM]);
        }
    }
}

void arch_fill_in_exception_context(const arch_exception_context_t* arch_context, zx_exception_report_t* report) {
    zx_exception_context_t *zx_context = &report->context;

    zx_context->arch.u.loongarch64 = {};

    // // If there was a fatal page fault, fill in the address that caused the fault.
    // if (ZX_EXCP_FATAL_PAGE_FAULT == report->header.type) {
    //     zx_context->arch.u.arm_64.far = arch_context->far;
    // } else {
    //     zx_context->arch.u.arm_64.far = 0;
    // }
}

zx_status_t arch_dispatch_user_policy_exception(void) {
    arch_exception_context_t context = {};
    return dispatch_user_exception(ZX_EXCP_POLICY_ERROR, &context);
}

void arch_install_context_regs(thread_t *thread, const arch_exception_context_t *context) {
    // TODO(ZX-563): |context->frame| will be nullptr for exceptions that
    // don't (yet) provide the registers.
    if (context->frame)
    {
        DEBUG_ASSERT(thread->arch.suspended_general_regs == nullptr);
        thread->arch.suspended_general_regs = context->frame;
    }
}

void arch_remove_context_regs(thread_t *thread) {
    thread->arch.suspended_general_regs = nullptr;
}
