#pragma once

#define SHIFT_4K (12)
#define PAGE_SIZE_SHIFT (SHIFT_4K)
#define PAGE_SIZE (1L << PAGE_SIZE_SHIFT)


#define MAX_CACHE_LINE 128

#define ARCH_DEFAULT_STACK_SIZE 8192

#define ARCH_PHYSMAP_SIZE (1UL << 39)

#define PLV_KERN 			0
#define PLV_USER			3
#define PLV_MASK			0x3

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

#define LOONGARCH_CSR_DMWIN0		0x180	/* 64 direct map win0: MEM & IF */
#define LOONGARCH_CSR_DMWIN1		0x181	/* 64 direct map win1: MEM & IF */
#define LOONGARCH_CSR_DMWIN2		0x182	/* 64 direct map win2: MEM */
#define LOONGARCH_CSR_DMWIN3		0x183	/* 64 direct map win3: MEM */

#define LOONGARCH_CSR_CRMD		0x0	/* Current mode info */
#define LOONGARCH_CSR_EENTRY		0xc	/* Exception entry */

/* TLB Refill registers */
#define LOONGARCH_CSR_TLBRENTRY		0x88	/* TLB refill exception entry */
#define LOONGARCH_CSR_TLBRBADV		0x89	/* TLB refill badvaddr */
#define LOONGARCH_CSR_TLBREPC		0x8a	/* TLB refill EPC */
#define LOONGARCH_CSR_TLBRSAVE		0x8b	/* KScratch for TLB refill exception */
#define LOONGARCH_CSR_TLBRELO0		0x8c	/* TLB refill entrylo0 */
#define LOONGARCH_CSR_TLBRELO1		0x8d	/* TLB refill entrylo1 */
#define LOONGARCH_CSR_TLBREHI		0x8e	/* TLB refill entryhi */
#define LOONGARCH_CSR_TLBRPRMD		0x8f	/* TLB refill mode info */
