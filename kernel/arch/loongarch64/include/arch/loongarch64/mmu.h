#pragma once

#ifndef __ASSEMBLER__

#include <arch/defines.h>
#include <sys/types.h>

#define MMU_KERNEL_SIZE_SHIFT 39
#define MMU_USER_SIZE_SHIFT   39
#define MMU_GUEST_SIZE_SHIFT  39


#define MMU_KERNEL_PAGE_SIZE_SHIFT   (PAGE_SIZE_SHIFT)

// Addr space size of specified level page table entry
#define MMU_LX_X(page_shift, level) ((4 - (level)) * ((page_shift) - 3) + 3)

// Addr space size of 1 PGD entry
#define MMU_KERNEL_TOP_SHIFT MMU_LX_X(MMU_KERNEL_PAGE_SIZE_SHIFT, 1)

// No. of PGD entries to fill all kernel space
#define MMU_KERNEL_PAGE_TABLE_ENTRIES_TOP (0x1 << (MMU_KERNEL_SIZE_SHIFT - MMU_KERNEL_TOP_SHIFT))
// No. of PTE entries in a page dir
#define MMU_KERNEL_PAGE_TABLE_ENTRIES (0x1 << (MMU_KERNEL_PAGE_SIZE_SHIFT - 3))
// PA range in PTE
#define MMU_PTE_OUTPUT_ADDR_MASK      (0xffffffffUL << 12)

/**
 * Page Table Entry bits
 */
#define _ULCAST_ (unsigned long)

#define	_PAGE_VALID_SHIFT	0
#define	_PAGE_DIRTY_SHIFT	1
#define	_PAGE_PLV_SHIFT		2  /* 2~3, two bits */
#define	_CACHE_SHIFT		4  /* 4~5, two bits */
#define	_PAGE_GLOBAL_SHIFT	6
#define	_PAGE_HUGE_SHIFT	6  /* HUGE is a PMD bit */
#define	_PAGE_PRESENT_SHIFT	7
#define	_PAGE_WRITE_SHIFT	8
#define	_PAGE_PROTNONE_SHIFT	9
#define	_PAGE_SPECIAL_SHIFT	10
#define	_PAGE_HGLOBAL_SHIFT	12 /* HGlobal is a PMD bit */
#define	_PAGE_PFN_SHIFT		12
#define	_PAGE_PFN_END_SHIFT	48
#define	_PAGE_NO_READ_SHIFT	61
#define	_PAGE_NO_EXEC_SHIFT	62
#define	_PAGE_RPLV_SHIFT	63

/* Used only by software */
#define _PAGE_PRESENT		(_ULCAST_(1) << _PAGE_PRESENT_SHIFT)
#define _PAGE_WRITE		(_ULCAST_(1) << _PAGE_WRITE_SHIFT)
#define _PAGE_PROTNONE		(_ULCAST_(1) << _PAGE_PROTNONE_SHIFT)
#define _PAGE_SPECIAL		(_ULCAST_(1) << _PAGE_SPECIAL_SHIFT)

/* Used by TLB hardware (placed in EntryLo*) */
#define _PAGE_VALID		(_ULCAST_(1) << _PAGE_VALID_SHIFT)
#define _PAGE_DIRTY		(_ULCAST_(1) << _PAGE_DIRTY_SHIFT)
#define _PAGE_PLV		(_ULCAST_(3) << _PAGE_PLV_SHIFT)
#define _PAGE_GLOBAL		(_ULCAST_(1) << _PAGE_GLOBAL_SHIFT)
#define _PAGE_HUGE		(_ULCAST_(1) << _PAGE_HUGE_SHIFT)
#define _PAGE_HGLOBAL		(_ULCAST_(1) << _PAGE_HGLOBAL_SHIFT)
#define _PAGE_NO_READ		(_ULCAST_(1) << _PAGE_NO_READ_SHIFT)
#define _PAGE_NO_EXEC		(_ULCAST_(1) << _PAGE_NO_EXEC_SHIFT)
#define _PAGE_RPLV		(_ULCAST_(1) << _PAGE_RPLV_SHIFT)
#define _CACHE_MASK		(_ULCAST_(3) << _CACHE_SHIFT)
#define _PFN_SHIFT		(PAGE_SHIFT - 12 + _PAGE_PFN_SHIFT)

#define _PAGE_USER	(PLV_USER << _PAGE_PLV_SHIFT)
#define _PAGE_KERN	(PLV_KERN << _PAGE_PLV_SHIFT)

#define _PFN_MASK (~((_ULCAST_(1) << (_PFN_SHIFT)) - 1) & \
		  ((_ULCAST_(1) << (_PAGE_PFN_END_SHIFT)) - 1))

typedef uint64_t pte_t;

#endif // __ASSEMBLER__
