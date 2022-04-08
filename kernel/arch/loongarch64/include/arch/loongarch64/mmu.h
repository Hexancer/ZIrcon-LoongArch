#pragma once

#ifndef __ASSEMBLER__

#include <arch/defines.h>
#include <sys/types.h>
#include <zircon/types.h>
#include <zircon/compiler.h>

#define MMU_KERNEL_SIZE_SHIFT 39
#define MMU_USER_SIZE_SHIFT   39
#define MMU_GUEST_SIZE_SHIFT  39


#define MMU_KERNEL_PAGE_SIZE_SHIFT   (PAGE_SIZE_SHIFT)
#define MMU_USER_PAGE_SIZE_SHIFT     (PAGE_SIZE_SHIFT)
#define MMU_GUEST_PAGE_SIZE_SHIFT    (PAGE_SIZE_SHIFT)
#define MMU_MAX_PAGE_SIZE_SHIFT      (PAGE_SIZE_SHIFT)


// Addr space size of specified level page table entry
#define MMU_LX_X(page_shift, level) ((4 - (level)) * ((page_shift) - 3) + 3)

// Addr space size of 1 PGD entry
#define MMU_KERNEL_TOP_SHIFT MMU_LX_X(MMU_KERNEL_PAGE_SIZE_SHIFT, 1)
#define MMU_USER_TOP_SHIFT   MMU_LX_X(MMU_KERNEL_PAGE_SIZE_SHIFT, 1)
#define MMU_GUEST_TOP_SHIFT  MMU_LX_X(MMU_KERNEL_PAGE_SIZE_SHIFT, 1)

// No. of PGD entries to fill all kernel space
#define MMU_KERNEL_PAGE_TABLE_ENTRIES_TOP (0x1 << (MMU_KERNEL_SIZE_SHIFT - MMU_KERNEL_TOP_SHIFT))
#define MMU_USER_PAGE_TABLE_ENTRIES_TOP   (0x1 << (MMU_KERNEL_SIZE_SHIFT - MMU_KERNEL_TOP_SHIFT))
#define MMU_GUEST_PAGE_TABLE_ENTRIES_TOP  (0x1 << (MMU_KERNEL_SIZE_SHIFT - MMU_KERNEL_TOP_SHIFT))
// No. of PTE entries in a page dir
#define MMU_KERNEL_PAGE_TABLE_ENTRIES (0x1 << (MMU_KERNEL_PAGE_SIZE_SHIFT - 3))

/**
 * Page Table Entry bits
 */
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
#define _PFN_SHIFT		(PAGE_SIZE_SHIFT - 12 + _PAGE_PFN_SHIFT)

#define _PAGE_USER	(PLV_USER << _PAGE_PLV_SHIFT)
#define _PAGE_KERN	(PLV_KERN << _PAGE_PLV_SHIFT)

#define _PFN_MASK (~((_ULCAST_(1) << (_PFN_SHIFT)) - 1) & \
		  ((_ULCAST_(1) << (_PAGE_PFN_END_SHIFT)) - 1))

/*
 * Cache attributes
 */

#ifndef _CACHE_SUC
#define _CACHE_SUC			(0<<_CACHE_SHIFT) /* Strong-ordered UnCached */
#endif
#ifndef _CACHE_CC
#define _CACHE_CC			(1<<_CACHE_SHIFT) /* Coherent Cached */
#endif
#ifndef _CACHE_WUC
#define _CACHE_WUC			(2<<_CACHE_SHIFT) /* Weak-ordered UnCached */
#endif

#define __READABLE	(_PAGE_VALID)
#define __WRITEABLE	(_PAGE_DIRTY | _PAGE_WRITE)

#define _PAGE_CHG_MASK	(_PAGE_SPECIAL | _PFN_MASK | _CACHE_MASK | _PAGE_PLV)
#define _HPAGE_CHG_MASK	(_PAGE_SPECIAL | _PFN_MASK | _CACHE_MASK | _PAGE_PLV | _PAGE_HUGE)

#define PAGE_NONE	__pgprot(_PAGE_PROTNONE | _PAGE_NO_READ | \
				 _PAGE_USER | _CACHE_CC)
#define PAGE_SHARED	__pgprot(_PAGE_PRESENT | _PAGE_WRITE | \
				 _PAGE_USER | _CACHE_CC)
#define PAGE_READONLY	__pgprot(_PAGE_PRESENT | _PAGE_USER | _CACHE_CC)

#define PAGE_KERNEL	__pgprot(_PAGE_PRESENT | __READABLE | __WRITEABLE | \
				 _PAGE_GLOBAL | _PAGE_KERN | _CACHE_CC)
#define PAGE_KERNEL_SUC __pgprot(_PAGE_PRESENT | __READABLE | __WRITEABLE | \
				 _PAGE_GLOBAL | _PAGE_KERN |  _CACHE_SUC)
#define PAGE_KERNEL_WUC __pgprot(_PAGE_PRESENT | __READABLE | __WRITEABLE | \
				 _PAGE_GLOBAL | _PAGE_KERN |  _CACHE_WUC)

typedef uint64_t pte_t;

#define MMU_PTE_ATTR_STRONGLY_ORD

// Zero PTE is always invalid
#define MMU_PTE_INVALID              ((pte_t)(0))
#define MMU_PTE_DESCRIPTOR_INVALID   MMU_PTE_INVALID


// Metadata range in PTE
// #define MMU_PTE_DESCRIPTOR_INVALID    (0)
#define MMU_PTE_DESCRIPTOR_MASK       (0xffUL)
#define MMU_PTE_DESCRIPTOR_TABLE      (0)      // We don't use these bits in page dir

// PA range in PTE
#define MMU_PTE_OUTPUT_ADDR_MASK      _PFN_MASK

#define MMU_PTE_PERMISSION_MASK      (_PAGE_NO_READ | _PAGE_NO_EXEC | _PAGE_RPLV | MMU_PTE_DESCRIPTOR_MASK)


__BEGIN_CDECLS

const size_t MMU_LOONGARCH64_ASID_BITS = 10;
const uint16_t MMU_LOONGARCH64_GLOBAL_ASID = (1u << MMU_LOONGARCH64_ASID_BITS) - 1;
const uint16_t MMU_LOONGARCH64_UNUSED_ASID = 0;
const uint16_t MMU_LOONGARCH64_FIRST_USER_ASID = 1;
const uint16_t MMU_LOONGARCH64_MAX_USER_ASID = MMU_LOONGARCH64_GLOBAL_ASID - 1;

pte_t* loongarch64_get_kernel_ptable();

zx_status_t loongarch64_boot_map_v(const vaddr_t vaddr,
                                   const paddr_t paddr,
                             	   const size_t len,
                             	   const pte_t flags);

__END_CDECLS

#endif // __ASSEMBLER__
