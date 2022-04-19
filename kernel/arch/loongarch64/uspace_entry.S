#include <asm.h>
#include <arch/loongarch64/iframe.h>
#include <arch/defines.h>

#define CPU_LOG_INT        (1 << 4)
#define CPU_LOG_EXEC       (1 << 5)
#define CPU_LOG_PCALL      (1 << 6)
#define CPU_LOG_TB_CPU     (1 << 8)
#define CPU_LOG_RESET      (1 << 9)
#define LOG_UNIMP          (1 << 10)
#define LOG_GUEST_ERROR    (1 << 11)
#define CPU_LOG_MMU        (1 << 12)

// void loongarch64_uspace_entry(iframe_t* iframe, vaddr_t kstack) __NO_RETURN;
FUNCTION(loongarch64_uspace_entry)
    // save kstack
    csrwr  A1, LOONGARCH_CSR_KS2

    ld.d   A2, A0, LOONGARCH64_IFRAME_OFFSET_EPC
    csrwr  A2, LOONGARCH_CSR_EPC
    ld.d   A2, A0, LOONGARCH64_IFRAME_OFFSET_PRMD
    csrwr  A2, LOONGARCH_CSR_PRMD

    // ld.d   ZERO, A0, LOONGARCH64_IFRAME_OFFSET_R + (ZERO_NUM * 8)
    ld.d   RA,  A0, LOONGARCH64_IFRAME_OFFSET_R + (RA_NUM * 8)
    ld.d   TP,  A0, LOONGARCH64_IFRAME_OFFSET_R + (TP_NUM * 8)
    ld.d   SP,  A0, LOONGARCH64_IFRAME_OFFSET_R + (SP_NUM * 8)
    // Setting A0 deferred to last, we're still using it.
    ld.d   A1,  A0, LOONGARCH64_IFRAME_OFFSET_R + (A1_NUM * 8)
    ld.d   A2,  A0, LOONGARCH64_IFRAME_OFFSET_R + (A2_NUM * 8)
    ld.d   A3,  A0, LOONGARCH64_IFRAME_OFFSET_R + (A3_NUM * 8)
    ld.d   A4,  A0, LOONGARCH64_IFRAME_OFFSET_R + (A4_NUM * 8)
    ld.d   A5,  A0, LOONGARCH64_IFRAME_OFFSET_R + (A5_NUM * 8)
    ld.d   A6,  A0, LOONGARCH64_IFRAME_OFFSET_R + (A6_NUM * 8)
    ld.d   A7,  A0, LOONGARCH64_IFRAME_OFFSET_R + (A7_NUM * 8)
    ld.d   T0,  A0, LOONGARCH64_IFRAME_OFFSET_R + (T0_NUM * 8)
    ld.d   T1,  A0, LOONGARCH64_IFRAME_OFFSET_R + (T1_NUM * 8)
    ld.d   T2,  A0, LOONGARCH64_IFRAME_OFFSET_R + (T2_NUM * 8)
    ld.d   T3,  A0, LOONGARCH64_IFRAME_OFFSET_R + (T3_NUM * 8)
    ld.d   T4,  A0, LOONGARCH64_IFRAME_OFFSET_R + (T4_NUM * 8)
    ld.d   T5,  A0, LOONGARCH64_IFRAME_OFFSET_R + (T5_NUM * 8)
    ld.d   T6,  A0, LOONGARCH64_IFRAME_OFFSET_R + (T6_NUM * 8)
    ld.d   T7,  A0, LOONGARCH64_IFRAME_OFFSET_R + (T7_NUM * 8)
    ld.d   T8,  A0, LOONGARCH64_IFRAME_OFFSET_R + (T8_NUM * 8)
    ld.d   X0, A0, LOONGARCH64_IFRAME_OFFSET_R + (X0_NUM * 8)
    ld.d   FP,  A0, LOONGARCH64_IFRAME_OFFSET_R + (FP_NUM * 8)
    ld.d   S0,  A0, LOONGARCH64_IFRAME_OFFSET_R + (S0_NUM * 8)
    ld.d   S1,  A0, LOONGARCH64_IFRAME_OFFSET_R + (S1_NUM * 8)
    ld.d   S2,  A0, LOONGARCH64_IFRAME_OFFSET_R + (S2_NUM * 8)
    ld.d   S3,  A0, LOONGARCH64_IFRAME_OFFSET_R + (S3_NUM * 8)
    ld.d   S4,  A0, LOONGARCH64_IFRAME_OFFSET_R + (S4_NUM * 8)
    ld.d   S5,  A0, LOONGARCH64_IFRAME_OFFSET_R + (S5_NUM * 8)
    ld.d   S6,  A0, LOONGARCH64_IFRAME_OFFSET_R + (S6_NUM * 8)
    ld.d   S7,  A0, LOONGARCH64_IFRAME_OFFSET_R + (S7_NUM * 8)
    ld.d   S8,  A0, LOONGARCH64_IFRAME_OFFSET_R + (S8_NUM * 8)

    ld.d   A0,  A0, LOONGARCH64_IFRAME_OFFSET_R + (A0_NUM * 8)

    // Lazy loading of the FPU means we don't need to zero the simd registers
    ertn

END_FUNCTION(loongarch64_uspace_entry)
