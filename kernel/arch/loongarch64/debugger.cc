#include <arch.h>
#include <arch/loongarch64.h>
//#include <arch/loongarch64/registers.h>
#include <arch/debugger.h>
#include <err.h>
#include <kernel/thread.h>
#include <kernel/thread_lock.h>
#include <string.h>
#include <sys/types.h>
#include <zircon/syscalls/debug.h>
#include <zircon/types.h>

zx_status_t arch_get_general_regs(struct thread* thread, zx_thread_state_general_regs_t* out) {
    TODO();
    return ZX_OK;
}

zx_status_t arch_set_general_regs(struct thread* thread, const zx_thread_state_general_regs_t* in) {
    Guard<spin_lock_t, IrqSave> thread_lock_guard{ThreadLock::Get()};

    // Punt if registers aren't available. E.g.,
    // ZX-563 (registers aren't available in synthetic exceptions)
    if (thread->arch.suspended_general_regs == nullptr)
        return ZX_ERR_NOT_SUPPORTED;

    loongarch64_iframe_t* out = thread->arch.suspended_general_regs;
    DEBUG_ASSERT(out);

    memcpy(&out->gpr[0], &in->r0, sizeof(out->gpr));
    // TODO: $ra should be equal to $pc, assert this
    //     out->gpr[RA_NUM] = in->pc; 
    // TODO: LA64 don't allow CSR insns in user mode
    //     out->csr[LOONGARCH_CSR_PRMD] = ...
    out->csr[LOONGARCH_CSR_EPC] = in->pc;

    return ZX_OK;
}

zx_status_t arch_get_fp_regs(struct thread* thread, zx_thread_state_fp_regs* out) {
    TODO();
    return ZX_OK;
}

zx_status_t arch_set_fp_regs(struct thread* thread, const zx_thread_state_fp_regs* in) {
    TODO();
    return ZX_OK;
}

zx_status_t arch_get_vector_regs(struct thread* thread, zx_thread_state_vector_regs* out) {
    TODO();
    return ZX_OK;
}

zx_status_t arch_set_vector_regs(struct thread* thread, const zx_thread_state_vector_regs* in) {
    TODO();
    return ZX_OK;
}

zx_status_t arch_get_debug_regs(struct thread* thread, zx_thread_state_debug_regs* out) {
    TODO();
    return ZX_OK;
}

zx_status_t arch_set_debug_regs(struct thread* thread, const zx_thread_state_debug_regs* in) {
    TODO();
    return ZX_OK;
}

zx_status_t arch_get_single_step(struct thread* thread, bool* single_step) {
    TODO();
    return ZX_OK;
}

zx_status_t arch_set_single_step(struct thread* thread, bool single_step) {
    TODO();
    return ZX_OK;
}

uint8_t arch_get_hw_breakpoint_count() {
    // TODO: return loongarch64_hw_breakpoint_count();
    return 0;
}

uint8_t arch_get_hw_watchpoint_count() {
    // TODO: return arm64_hw_watchpoint_count();
    return 0;
}
