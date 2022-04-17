#include <asm.h>
#include <arch/defines.h>
#include <arch/asm_macros.h>
#include <arch/arch_thread.h>
#include <arch/loongarch64.h>
// #include <arch/loongarch64/exceptions.h>
#include <zircon/zx-syscall-numbers.h>

#define RSIZE   8       /* 64 bit mode register size */


.section .text.boot.vectab,"ax",@progbits
.align 12

FUNCTION(entry_exception_handler)
    // T0 swapped with KS0, SP swapped with KS1
    
    // Make sure we are always using kernel stack to create frames,
    // which means in USER mode we need to read KSP from KS2
    csrrd   T0, LOONGARCH_CSR_PRMD
    andi    T0, T0, PLV_MASK
    bnez    T0, exception_save_user 

exception_save_kernel:
    // Read back SP
    csrrd   SP, LOONGARCH_CSR_KS1
    b       exception_save_done

exception_save_user:
    // Read KSP from KS2
    csrrd   SP, LOONGARCH_CSR_KS2
    
exception_save_done:
    addi.d  T0, ZERO, -0x10
    and     SP, SP, T0
    addi.d  SP, SP,  -((CSR_NUM + BASE_NUM + FP_BASE_NUM)  * RSIZE)

    st.d   ZERO, SP, ZERO_NUM * RSIZE
    st.d   RA, SP, RA_NUM * RSIZE
    st.d   GP, SP, GP_NUM * RSIZE
    st.d   A0, SP, A0_NUM * RSIZE
    st.d   A1, SP, A1_NUM * RSIZE
    st.d   A2, SP, A2_NUM * RSIZE
    st.d   A3, SP, A3_NUM * RSIZE
    st.d   A4, SP, A4_NUM * RSIZE
    st.d   A5, SP, A5_NUM * RSIZE
    st.d   A6, SP, A6_NUM * RSIZE
    st.d   A7, SP, A7_NUM * RSIZE
    st.d   T1, SP, T1_NUM * RSIZE
    st.d   T2, SP, T2_NUM * RSIZE
    st.d   T3, SP, T3_NUM * RSIZE
    st.d   T4, SP, T4_NUM * RSIZE
    st.d   T5, SP, T5_NUM * RSIZE
    st.d   T6, SP, T6_NUM * RSIZE
    st.d   T7, SP, T7_NUM * RSIZE
    st.d   T8, SP, T8_NUM * RSIZE
    st.d   TP, SP, TP_NUM * RSIZE
    st.d   FP, SP, FP_NUM * RSIZE
    st.d   S0, SP, S0_NUM * RSIZE
    st.d   S1, SP, S1_NUM * RSIZE
    st.d   S2, SP, S2_NUM * RSIZE
    st.d   S3, SP, S3_NUM * RSIZE
    st.d   S4, SP, S4_NUM * RSIZE
    st.d   S5, SP, S5_NUM * RSIZE
    st.d   S6, SP, S6_NUM * RSIZE
    st.d   S7, SP, S7_NUM * RSIZE
    st.d   S8, SP, S8_NUM * RSIZE

    /*
     * save T0/SP from scratch registers on stack
     */
    csrrd  T0, LOONGARCH_CSR_KS0
    st.d   T0, SP, T0_NUM * RSIZE
    csrrd  T0, LOONGARCH_CSR_KS1
    st.d   T0, SP, SP_NUM * RSIZE

    csrrd   T0, LOONGARCH_CSR_CRMD
    st.d    T0, SP, (LOONGARCH_CSR_CRMD + BASE_NUM)  * RSIZE
    csrrd   T0, LOONGARCH_CSR_PRMD
    st.d    T0, SP, (LOONGARCH_CSR_PRMD + BASE_NUM)  * RSIZE
    csrrd   T0, LOONGARCH_CSR_ECFG
    st.d    T0, SP, (LOONGARCH_CSR_ECFG + BASE_NUM) * RSIZE
    csrrd   T0, LOONGARCH_CSR_ESTAT
    st.d    T0, SP, (LOONGARCH_CSR_ESTAT + BASE_NUM)  * RSIZE
    csrrd   T0, LOONGARCH_CSR_EPC
    st.d    T0, SP, (LOONGARCH_CSR_EPC+ BASE_NUM)    * RSIZE
    csrrd   T0, LOONGARCH_CSR_BADV
    st.d    T0, SP, (LOONGARCH_CSR_BADV + BASE_NUM)  * RSIZE
    csrrd   T0, LOONGARCH_CSR_BADI
    st.d    T0, SP, (LOONGARCH_CSR_BADI + BASE_NUM)  * RSIZE
    csrrd   T0, LOONGARCH_CSR_EUEN
    st.d    T0, SP, (LOONGARCH_CSR_EUEN + BASE_NUM)  * RSIZE

    /* Save FPU context */
    ori     T1, ZERO, CSR_EUEN_FPEN
    and     T2, T0, T1
    beqz    T2, save_fpu_done

    fst.d   $f0,  SP, (FP0_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f1,  SP, (FP1_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f2,  SP, (FP2_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f3,  SP, (FP3_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f4,  SP, (FP4_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f5,  SP, (FP5_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f6,  SP, (FP6_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f7,  SP, (FP7_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f8,  SP, (FP8_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f9,  SP, (FP9_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f10, SP, (FP10_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f11, SP, (FP11_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f12, SP, (FP12_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f13, SP, (FP13_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f14, SP, (FP14_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f15, SP, (FP15_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f16, SP, (FP16_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f17, SP, (FP17_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f18, SP, (FP18_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f19, SP, (FP19_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f20, SP, (FP20_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f21, SP, (FP21_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f22, SP, (FP22_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f23, SP, (FP23_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f24, SP, (FP24_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f25, SP, (FP25_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f26, SP, (FP26_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f27, SP, (FP27_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f28, SP, (FP28_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f29, SP, (FP29_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f30, SP, (FP30_NUM + FP_BASE_INDEX) * RSIZE
    fst.d   $f31, SP, (FP31_NUM + FP_BASE_INDEX) * RSIZE

    movfcsr2gr      T3, FCSR0
    st.d            T3, SP, (FCSR_NUM + FP_BASE_INDEX) * RSIZE
    movcf2gr        T3, $fcc0
    or              T2, T3, ZERO
    movcf2gr        T3, $fcc1
    bstrins.d       T2, T3, 0xf, 0x8
    movcf2gr        T3, $fcc2
    bstrins.d       T2, T3, 0x17, 0x10
    movcf2gr        T3, $fcc3
    bstrins.d       T2, T3, 0x1f, 0x18
    movcf2gr        T3, $fcc4
    bstrins.d       T2, T3, 0x27, 0x20
    movcf2gr        T3, $fcc5
    bstrins.d       T2, T3, 0x2f, 0x28
    movcf2gr        T3, $fcc6
    bstrins.d       T2, T3, 0x37, 0x30
    movcf2gr        T3, $fcc7
    bstrins.d       T2, T3, 0x3f, 0x38
    st.d            T2, SP, (FCC_NUM + FP_BASE_INDEX)  * RSIZE

save_fpu_done:
save_context_done:
    // Check if this is an syscall
    //   1. syscall should only come from USER mode
    csrrd   T0, LOONGARCH_CSR_PRMD
    andi    T0, T0, PLV_MASK
    beqz    T0, handle_general_exception

    //   2. ECODE should be SYS
    csrrd   T0, LOONGARCH_CSR_ESTAT
    srli.d  T0, T0, CSR_ESTAT_EXC_SHIFT
    li.d    T1, CSR_ESTAT_EXC_SYS
    bne     T0, T1, handle_general_exception

handle_syscall:
    // read EPC to T7, as syscall convention
    csrrd   T7, LOONGARCH_CSR_EPC
    addi.d  T7, T7, 4
    b       loongarch64_handle_syscall
    // we jump to handle_exception_done after syscall handling

handle_general_exception:    
    move    A0,   SP
    bl      loongarch64_handle_exception

handle_exception_done:
    /*disable interrupt*/
    li.d    T0,   (1 << 2)
    csrxchg ZERO, T0, LOONGARCH_CSR_CRMD

    ld.d    T0, SP, (LOONGARCH_CSR_PRMD + BASE_NUM) * RSIZE
    csrwr   T0, LOONGARCH_CSR_PRMD
    ld.d    T0, SP, (LOONGARCH_CSR_ECFG + BASE_NUM) * RSIZE
    csrwr   T0, LOONGARCH_CSR_ECFG
    ld.d    T0, SP, (LOONGARCH_CSR_EPC + BASE_NUM) * RSIZE
    csrwr   T0, LOONGARCH_CSR_EPC

    ld.d    T0, SP, (LOONGARCH_CSR_EUEN + BASE_NUM) * RSIZE
    ori     T1, ZERO, CSR_EUEN_FPEN
    and     T2, T0, T1
    beqz    T2, restore_fpu_done

    /*
     * check previous FP state
     * restore FP contect if FP enabled
     */
    fld.d   $f0,  SP, (FP0_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f1,  SP, (FP1_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f2,  SP, (FP2_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f3,  SP, (FP3_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f4,  SP, (FP4_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f5,  SP, (FP5_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f6,  SP, (FP6_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f7,  SP, (FP7_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f8,  SP, (FP8_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f9,  SP, (FP9_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f10, SP, (FP10_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f11, SP, (FP11_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f12, SP, (FP12_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f13, SP, (FP13_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f14, SP, (FP14_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f15, SP, (FP15_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f16, SP, (FP16_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f17, SP, (FP17_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f18, SP, (FP18_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f19, SP, (FP19_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f20, SP, (FP20_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f21, SP, (FP21_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f22, SP, (FP22_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f23, SP, (FP23_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f24, SP, (FP24_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f25, SP, (FP25_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f26, SP, (FP26_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f27, SP, (FP27_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f28, SP, (FP28_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f29, SP, (FP29_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f30, SP, (FP30_NUM + FP_BASE_INDEX) * RSIZE
    fld.d   $f31, SP, (FP31_NUM + FP_BASE_INDEX) * RSIZE

    ld.d    T0, SP, (FCSR_NUM + FP_BASE_INDEX) * RSIZE
    movgr2fcsr      FCSR0, T0
    ld.d    T0, SP, (FCC_NUM + FP_BASE_INDEX) * RSIZE
    bstrpick.d      T1, T0, 7, 0
    movgr2cf        $fcc0, T1
    bstrpick.d      T1, T0, 15, 8
    movgr2cf        $fcc1, T1
    bstrpick.d      T1, T0, 23, 16
    movgr2cf        $fcc2, T1
    bstrpick.d      T1, T0, 31, 24
    movgr2cf        $fcc3, T1
    bstrpick.d      T1, T0, 39, 32
    movgr2cf        $fcc4, T1
    bstrpick.d      T1, T0, 47, 40
    movgr2cf        $fcc5, T1
    bstrpick.d      T1, T0, 55, 48
    movgr2cf        $fcc6, T1
    bstrpick.d      T1, T0, 63, 56
    movgr2cf        $fcc7, T1

restore_fpu_done:
    ld.d    RA, SP, RA_NUM * RSIZE
    ld.d    GP, SP, GP_NUM * RSIZE
    ld.d    A0, SP, A0_NUM * RSIZE
    ld.d    A1, SP, A1_NUM * RSIZE
    ld.d    A2, SP, A2_NUM * RSIZE
    ld.d    A3, SP, A3_NUM * RSIZE
    ld.d    A4, SP, A4_NUM * RSIZE
    ld.d    A5, SP, A5_NUM * RSIZE
    ld.d    A6, SP, A6_NUM * RSIZE
    ld.d    A7, SP, A7_NUM * RSIZE
    // T0 restoration deferred since we need to use it
    ld.d    T1, SP, T1_NUM * RSIZE
    ld.d    T2, SP, T2_NUM * RSIZE
    ld.d    T3, SP, T3_NUM * RSIZE
    ld.d    T4, SP, T4_NUM * RSIZE
    ld.d    T5, SP, T5_NUM * RSIZE
    ld.d    T6, SP, T6_NUM * RSIZE
    ld.d    T7, SP, T7_NUM * RSIZE
    ld.d    T8, SP, T8_NUM * RSIZE
    ld.d    TP, SP, TP_NUM * RSIZE
    ld.d    FP, SP, FP_NUM * RSIZE
    ld.d    S0, SP, S0_NUM * RSIZE
    ld.d    S1, SP, S1_NUM * RSIZE
    ld.d    S2, SP, S2_NUM * RSIZE
    ld.d    S3, SP, S3_NUM * RSIZE
    ld.d    S4, SP, S4_NUM * RSIZE
    ld.d    S5, SP, S5_NUM * RSIZE
    ld.d    S6, SP, S6_NUM * RSIZE
    ld.d    S7, SP, S7_NUM * RSIZE
    ld.d    S8, SP, S8_NUM * RSIZE

    // Restore stack pointer
    ld.d    T0, SP, (LOONGARCH_CSR_PRMD + BASE_NUM) * RSIZE
    andi    T0, T0, PLV_MASK
    bnez    T0, exception_restore_user 

exception_restore_kernel:
    // Nothing to do, load from iframe is enough
    b       exception_restore_done

exception_restore_user:
    // save KSP in KS2
    // deallocate iframe
    addi.d  T0, SP, ((CSR_NUM + BASE_NUM + FP_BASE_NUM)  * RSIZE)
    csrwr   T0, LOONGARCH_CSR_KS2
    
exception_restore_done:
    ld.d    T0, SP, T0_NUM * RSIZE
    ld.d    SP, SP, SP_NUM * RSIZE

    ertn
END_FUNCTION(entry_exception_handler)

/*
   Exception trampoline copied down to RAM after initialization.
 */
FUNCTION(entry_exception)
    csrwr   T0, LOONGARCH_CSR_KS0
    csrwr   SP, LOONGARCH_CSR_KS1
    la      T0, entry_exception_handler
    jirl    ZERO, T0, 0
END_FUNCTION(entry_exception)


FUNCTION(entry_tlb_refill)
	csrwr	$t0, LOONGARCH_CSR_TLBRSAVE
	csrrd	$t0, LOONGARCH_CSR_PGD
	lddir	$t0, $t0, 3
	lddir	$t0, $t0, 1
	ldpte	$t0, 0
	ldpte	$t0, 1
	tlbfill
	csrrd	$t0, LOONGARCH_CSR_TLBRSAVE
	ertn
END_FUNCTION(entry_tlb_refill)

//
// Syscall args are in a0-a7 already.
// pc is in T7 and needs to go in the next available register,
// or the stack if the regs are full.
//
.macro pre_args, nargs
.if \nargs == 8
    addi.d  SP, SP, -16
    st.d    T7, SP, 0
.else
    move $a\nargs, T7
.endif
.endm

.macro post_args, nargs
.if \nargs == 8
    ld.d    T7, SP, 0
    addi.d  SP, SP, 16
.endif
    b       handle_syscall_done
.endm

//
// Expected state prior to arm64_syscall_dispatcher branch...
//
// a0-a7 - contains syscall arguments
// t7    - contains EPC
// t8    - contains syscall_num
// sp    - points to base of frame (frame->r[0])
//
// Expected state prior to unknown_syscall and wrapper_syscall...
//
// a0-a7 - contains syscall arguments
// t7    - contains EPC
//
FUNCTION(loongarch64_handle_syscall)
    // Restore per cpu pointer
    csrrd   TP, LOONGARCH_CSR_KS3

    // Verify syscall number and call the unknown handler if bad.
    li.d    T0, ZX_SYS_COUNT
    bltu    T0, T8, handle_unknown_syscall

    // Jump to the right syscall wrapper.
    // call_wrapper_table is 4096 byte aligned so just use adrp
    la.pcrel T0, call_wrapper_table
    slli.d   T1, T8, 3
    ldx.d    T0, T0, T1
    jirl     ZERO, T0, 0
    // at end of wrapper call, we always jump to handle_syscall_done

handle_unknown_syscall:
    move    A0, T8
    pre_args 1
    bl   unknown_syscall
    post_args 1

handle_syscall_done:
    // Upon return from syscall, x0 = status, x1 = thread signalled
    // Move the status to frame->r[0] for return to userspace.
    st.d    A0, SP, 0
    // Check for pending signals. If none, just return.
    beqz    A1, handle_exception_done
    move    A0, SP
    bl      arch_iframe_process_pending_signals
    b       handle_exception_done
END_FUNCTION(loongarch64_handle_syscall)


// Adds a label for making the syscall and adds it to the jump table.
.macro syscall_dispatch nargs, syscall
    .pushsection .text.syscall-dispatch,"ax",%progbits
    LOCAL_FUNCTION(.Lcall_\syscall\())
        pre_args \nargs
        bl wrapper_\syscall
        post_args \nargs
    END_FUNCTION(.Lcall_\syscall\())
    .popsection
    .pushsection .rodata.syscall-table,"a",%progbits
        .quad .Lcall_\syscall
    .popsection
.endm


// Adds the label for the jump table.
.macro start_syscall_dispatch
    .pushsection .rodata.syscall-table,"a",%progbits
    // align on 4096 byte boundary to save an instruction on table lookup
    .balign 4096
    call_wrapper_table:
    .popsection
.endm

#include <zircon/syscall-kernel-branches.S>