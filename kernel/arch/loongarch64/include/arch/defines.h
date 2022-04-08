#pragma once

#ifndef __ASSEMBLER__
#define _ULCAST_ (unsigned long)
#else
#define _ULCAST_
#endif

#define SHIFT_4K (12)
#define PAGE_SIZE_SHIFT (SHIFT_4K)

#define PAGE_SIZE (1L << PAGE_SIZE_SHIFT)
#define PAGE_MASK (PAGE_SIZE - 1)

#define MAX_CACHE_LINE 128

#define ARCH_DEFAULT_STACK_SIZE 8192

#define ARCH_PHYSMAP_SIZE (1UL << 32)

#define GENMASK(h, l)   (((1UL << (h + 1 - l)) -1) << (l))

#define LOONGARCH_CPUCFG2		0x2
#define  CPUCFG2_FP			GENMASK(0, 0)
#define  CPUCFG2_FPSP			GENMASK(1, 1)
#define  CPUCFG2_FPDP			GENMASK(2, 2)
#define  CPUCFG2_FPVERS			GENMASK(5, 3)
#define  CPUCFG2_LSX			GENMASK(6, 6)
#define  CPUCFG2_LASX			GENMASK(7, 7)
#define  CPUCFG2_COMPLEX		GENMASK(8, 8)
#define  CPUCFG2_CRYPTO			GENMASK(9, 9)
#define  CPUCFG2_LVZP			GENMASK(10, 10)
#define  CPUCFG2_LVZVER			GENMASK(13, 11)
#define  CPUCFG2_LLFTP			GENMASK(14, 14)
#define  CPUCFG2_LLFTPREV		GENMASK(17, 15)
#define  CPUCFG2_X86BT			GENMASK(18, 18)
#define  CPUCFG2_ARMBT			GENMASK(19, 19)
#define  CPUCFG2_MIPSBT			GENMASK(20, 20)
#define  CPUCFG2_LSPW			GENMASK(21, 21)
#define  CPUCFG2_LAM			GENMASK(22, 22)

#define LOONGARCH_CPUCFG4		0x4
#define  CPUCFG4_CCFREQ			GENMASK(31, 0)

#define LOONGARCH_CPUCFG5		0x5
#define  CPUCFG5_CCMUL			GENMASK(15, 0)
#define  CPUCFG5_CCDIV			GENMASK(31, 16)

#define LOONGARCH_CPUCFG17		0x11
#define  CPUCFG17_L1I_WAYS_M		GENMASK(15, 0)
#define  CPUCFG17_L1I_SETS_M		GENMASK(23, 16)
#define  CPUCFG17_L1I_SIZE_M		GENMASK(30, 24)
#define  CPUCFG17_L1I_WAYS		0
#define  CPUCFG17_L1I_SETS		16
#define  CPUCFG17_L1I_SIZE		24

#define LOONGARCH_CPUCFG18		0x12
#define  CPUCFG18_L1D_WAYS_M		GENMASK(15, 0)
#define  CPUCFG18_L1D_SETS_M		GENMASK(23, 16)
#define  CPUCFG18_L1D_SIZE_M		GENMASK(30, 24)
#define  CPUCFG18_L1D_WAYS		0
#define  CPUCFG18_L1D_SETS		16
#define  CPUCFG18_L1D_SIZE		24

#define PLV_KERN 			0
#define PLV_USER			3
#define PLV_MASK			0x3

#define LOONGARCH_CSR_CRMD      0
#define  CSR_CRMD_DACM_SHIFT		7
#define  CSR_CRMD_DACM_WIDTH		2
#define  CSR_CRMD_DACM			(_ULCAST_(0x3) << CSR_CRMD_DACM_SHIFT)
#define  CSR_CRMD_DACF_SHIFT		5
#define  CSR_CRMD_DACF_WIDTH		2
#define  CSR_CRMD_DACF			(_ULCAST_(0x3) << CSR_CRMD_DACF_SHIFT)
#define  CSR_CRMD_PG_SHIFT		4
#define  CSR_CRMD_PG			(_ULCAST_(0x1) << CSR_CRMD_PG_SHIFT)
#define  CSR_CRMD_DA_SHIFT		3
#define  CSR_CRMD_DA			(_ULCAST_(0x1) << CSR_CRMD_DA_SHIFT)
#define  CSR_CRMD_IE_SHIFT		2
#define  CSR_CRMD_IE			(_ULCAST_(0x1) << CSR_CRMD_IE_SHIFT)
#define  CSR_CRMD_PLV_SHIFT		0
#define  CSR_CRMD_PLV_WIDTH		2
#define  CSR_CRMD_PLV			(_ULCAST_(0x3) << CSR_CRMD_PLV_SHIFT)

#define LOONGARCH_CSR_PRMD      1
#define LOONGARCH_CSR_EUEN      2
#define LOONGARCH_CSR_ECFG      4
#define LOONGARCH_CSR_ESTAT     5
#define LOONGARCH_CSR_EPC       6
#define LOONGARCH_CSR_BADV      7
#define LOONGARCH_CSR_BADINST   8
#define LOONGARCH_CSR_BADI      8
#define LOONGARCH_CSR_EBASE     0xc     /* Exception entry base address */
#define LOONGARCH_CSR_CPUNUM    0x20    /* CPU core number */

#define FCSR0       $r0

#define LOONGARCH_CSR_EENTRY		0xc	/* Exception entry */

#define CSR_EUEN_LBTEN_SHIFT        3
#define CSR_EUEN_LBTEN              (_ULCAST_(0x1) << CSR_EUEN_LBTEN_SHIFT)
#define CSR_EUEN_LASXEN_SHIFT       2
#define CSR_EUEN_LASXEN             (_ULCAST_(0x1) << CSR_EUEN_LASXEN_SHIFT)
#define CSR_EUEN_LSXEN_SHIFT        1
#define CSR_EUEN_LSXEN              (_ULCAST_(0x1) << CSR_EUEN_LSXEN_SHIFT)
#define CSR_EUEN_FPEN_SHIFT         0
#define CSR_EUEN_FPEN               (_ULCAST_(0x1) << CSR_EUEN_FPEN_SHIFT)


#define LOONGARCH_CSR_PGDL		0x19	/* Page table base address when VA[47] = 0 */
#define LOONGARCH_CSR_PGDH		0x1a	/* Page table base address when VA[47] = 1 */
#define LOONGARCH_CSR_PGD		0x1b	/* Page table base */

#define LOONGARCH_CSR_PWCTL0		0x1c	/* PWCtl0 */
#define  CSR_PWCTL0_PTEW_SHIFT		30
#define  CSR_PWCTL0_PTEW_WIDTH		2
#define  CSR_PWCTL0_PTEW		(_ULCAST_(0x3) << CSR_PWCTL0_PTEW_SHIFT)
#define  CSR_PWCTL0_DIR1WIDTH_SHIFT	25
#define  CSR_PWCTL0_DIR1WIDTH_WIDTH	5
#define  CSR_PWCTL0_DIR1WIDTH		(_ULCAST_(0x1f) << CSR_PWCTL0_DIR1WIDTH_SHIFT)
#define  CSR_PWCTL0_DIR1BASE_SHIFT	20
#define  CSR_PWCTL0_DIR1BASE_WIDTH	5
#define  CSR_PWCTL0_DIR1BASE		(_ULCAST_(0x1f) << CSR_PWCTL0_DIR1BASE_SHIFT)
#define  CSR_PWCTL0_DIR0WIDTH_SHIFT	15
#define  CSR_PWCTL0_DIR0WIDTH_WIDTH	5
#define  CSR_PWCTL0_DIR0WIDTH		(_ULCAST_(0x1f) << CSR_PWCTL0_DIR0WIDTH_SHIFT)
#define  CSR_PWCTL0_DIR0BASE_SHIFT	10
#define  CSR_PWCTL0_DIR0BASE_WIDTH	5
#define  CSR_PWCTL0_DIR0BASE		(_ULCAST_(0x1f) << CSR_PWCTL0_DIR0BASE_SHIFT)
#define  CSR_PWCTL0_PTWIDTH_SHIFT	5
#define  CSR_PWCTL0_PTWIDTH_WIDTH	5
#define  CSR_PWCTL0_PTWIDTH		(_ULCAST_(0x1f) << CSR_PWCTL0_PTWIDTH_SHIFT)
#define  CSR_PWCTL0_PTBASE_SHIFT	0
#define  CSR_PWCTL0_PTBASE_WIDTH	5
#define  CSR_PWCTL0_PTBASE		(_ULCAST_(0x1f) << CSR_PWCTL0_PTBASE_SHIFT)

#define LOONGARCH_CSR_PWCTL1		0x1d	/* PWCtl1 */
#define  CSR_PWCTL1_DIR3WIDTH_SHIFT	18
#define  CSR_PWCTL1_DIR3WIDTH_WIDTH	5
#define  CSR_PWCTL1_DIR3WIDTH		(_ULCAST_(0x1f) << CSR_PWCTL1_DIR3WIDTH_SHIFT)
#define  CSR_PWCTL1_DIR3BASE_SHIFT	12
#define  CSR_PWCTL1_DIR3BASE_WIDTH	5
#define  CSR_PWCTL1_DIR3BASE		(_ULCAST_(0x1f) << CSR_PWCTL0_DIR3BASE_SHIFT)
#define  CSR_PWCTL1_DIR2WIDTH_SHIFT	6
#define  CSR_PWCTL1_DIR2WIDTH_WIDTH	5
#define  CSR_PWCTL1_DIR2WIDTH		(_ULCAST_(0x1f) << CSR_PWCTL1_DIR2WIDTH_SHIFT)
#define  CSR_PWCTL1_DIR2BASE_SHIFT	0
#define  CSR_PWCTL1_DIR2BASE_WIDTH	5
#define  CSR_PWCTL1_DIR2BASE		(_ULCAST_(0x1f) << CSR_PWCTL0_DIR2BASE_SHIFT)

#define LOONGARCH_CSR_STLBPGSIZE	0x1e
#define  CSR_STLBPGSIZE_PS_WIDTH	6
#define  CSR_STLBPGSIZE_PS		(_ULCAST_(0x3f))

#define CSR_DMW0_PLV0		0x0000000000000001  // _CONST64_(1 << 0)
#define CSR_DMW0_VSEG		0x0000000000008000  // _CONST64_(0x8000)
#define CSR_DMW0_BASE		0x8000000000000000  // (CSR_DMW0_VSEG << DMW_PABITS)
#define CSR_DMW0_INIT		0x8000000000000001  // (CSR_DMW0_BASE | CSR_DMW0_PLV0)

#define CSR_DMW1_PLV0		0x0000000000000001  // _CONST64_(1 << 0)
#define CSR_DMW1_MAT		0x0000000000000010  // _CONST64_(1 << 4)
#define CSR_DMW1_VSEG		0x0000000000009000  // _CONST64_(0x9000)
#define CSR_DMW1_BASE		0x9000000000000000  // (CSR_DMW1_VSEG << DMW_PABITS)
#define CSR_DMW1_INIT		0x9000000000000011  // (CSR_DMW1_BASE | CSR_DMW1_MAT | CSR_DMW1_PLV0)



#define LOONGARCH_CSR_KS0          0x30
#define LOONGARCH_CSR_KS1          0x31

/* TLB Refill registers */
#define LOONGARCH_CSR_TLBRENTRY		0x88	/* TLB refill exception entry */
#define LOONGARCH_CSR_TLBRBADV		0x89	/* TLB refill badvaddr */
#define LOONGARCH_CSR_TLBREPC		0x8a	/* TLB refill EPC */
#define LOONGARCH_CSR_TLBRSAVE		0x8b	/* KScratch for TLB refill exception */
#define LOONGARCH_CSR_TLBRELO0		0x8c	/* TLB refill entrylo0 */
#define LOONGARCH_CSR_TLBRELO1		0x8d	/* TLB refill entrylo1 */
#define LOONGARCH_CSR_TLBREHI		0x8e	/* TLB refill entryhi */
#define LOONGARCH_CSR_TLBRPRMD		0x8f	/* TLB refill mode info */

#define LOONGARCH_CSR_DMWIN0		0x180	/* 64 direct map win0: MEM & IF */
#define LOONGARCH_CSR_DMWIN1		0x181	/* 64 direct map win1: MEM & IF */
#define LOONGARCH_CSR_DMWIN2		0x182	/* 64 direct map win2: MEM */
#define LOONGARCH_CSR_DMWIN3		0x183	/* 64 direct map win3: MEM */

/* Exception types decoded by machdep exception decoder */
#define EXC_INT         0       /* HW interrupt */
#define EXC_TLBL        1       /* TLB miss on a load */
#define EXC_TLBS        2       /* TLB miss on a store */
#define EXC_TLBI        3       /* TLB miss on a ifetch */
#define EXC_TLBM        4       /* TLB modified fault */
#define EXC_TLBRI       5       /* TLB Read-Inhibit exception */
#define EXC_TLBXI       6       /* TLB Execution-Inhibit exception */
#define EXC_TLBPE       7       /* TLB Privilege Error */
#define EXC_ADE         8       /* Address Error */
#define EXC_ALE         9       /* Unalign Access */
#define EXC_OOB         10      /* Out of bounds */
#define EXC_SYS         11      /* System call */
#define EXC_BP          12      /* Breakpoint */
#define EXC_INE         13      /* Inst. Not Exist */
#define EXC_IPE         14      /* Inst. Privileged Error */
#define EXC_FPDIS       15      /* FPU Disabled */
#define EXC_LSXDIS      16      /* LSX Disabled */
#define EXC_LASXDIS     17      /* LASX Disabled */
#define EXC_FPE         18      /* Floating Point Exception */
#define EXC_WATCH       19      /* Watch address reference */
#define EXC_BAD         255     /* Undecodeable */

#define COPY_SIGCODE    // copy sigcode above user stack in exec
#define ZERO  $r0 /* wired zero */
#define RA    $r1 /* return address */
#define GP    $r2 /* global pointer - caller saved for PIC */
#define SP    $r3 /* stack pointer */
#define V0    $r4 /* return value - caller saved */
#define V1    $r5
#define A0    $r4 /* argument registers */
#define A1    $r5
#define A2    $r6
#define A3    $r7
#define A4    $r8 /* arg reg 64 bit; caller saved in 32 bit */
#define A5    $r9
#define A6    $r10
#define A7    $r11
#define T0    $r12 /* caller saved */
#define T1    $r13
#define T2    $r14
#define T3    $r15
#define T4    $r16 /* callee saved */
#define T5    $r17
#define T6    $r18
#define T7    $r19
#define T8    $r20 /* caller saved */
#define TP    $r21 /* TLS */
#define FP    $r22 /* frame pointer */
#define S0    $r23 /* callee saved */
#define S1    $r24
#define S2    $r25
#define S3    $r26
#define S4    $r27
#define S5    $r28
#define S6    $r29
#define S7    $r30
#define S8    $r31 /* callee saved */

#define RA_NUM     1
#define GP_NUM     2
#define SP_NUM     3
#define A0_NUM     4
#define A1_NUM     5
#define A2_NUM     6
#define A3_NUM     7
#define A4_NUM     8
#define A5_NUM     9
#define A6_NUM     10
#define A7_NUM     11
#define T0_NUM     12
#define T1_NUM     13
#define T2_NUM     14
#define T3_NUM     15
#define T4_NUM     16
#define T5_NUM     17
#define T6_NUM     18
#define T7_NUM     19
#define T8_NUM     20
#define TP_NUM     21
#define FP_NUM     22
#define S0_NUM     23
#define S1_NUM     24
#define S2_NUM     25
#define S3_NUM     26
#define S4_NUM     27
#define S5_NUM     28
#define S6_NUM     29
#define S7_NUM     30
#define S8_NUM     31

#define FP0_NUM    0
#define FP1_NUM    1
#define FP2_NUM    2
#define FP3_NUM    3
#define FP4_NUM    4
#define FP5_NUM    5
#define FP6_NUM    6
#define FP7_NUM    7
#define FP8_NUM    8
#define FP9_NUM    9
#define FP10_NUM   10
#define FP11_NUM   11
#define FP12_NUM   12
#define FP13_NUM   13
#define FP14_NUM   14
#define FP15_NUM   15
#define FP16_NUM   16
#define FP17_NUM   17
#define FP18_NUM   18
#define FP19_NUM   19
#define FP20_NUM   20
#define FP21_NUM   21
#define FP22_NUM   22
#define FP23_NUM   23
#define FP24_NUM   24
#define FP25_NUM   25
#define FP26_NUM   26
#define FP27_NUM   27
#define FP28_NUM   28
#define FP29_NUM   29
#define FP30_NUM   30
#define FP31_NUM   31
#define FCSR_NUM   32
#define FCC_NUM    33

#define FP_BASE_NUM             34
#define BASE_NUM                32
#define CSR_NUM                 10
#define FP_BASE_INDEX           (CSR_NUM + BASE_NUM)