#pragma once

#ifndef __ASSEMBLER__

#include <arch/defines.h>
#include <sys/types.h>

#define MMU_KERNEL_PAGE_SIZE_SHIFT      (PAGE_SIZE_SHIFT)

#define MMU_LX_X(page_shift, level) ((4 - (level)) * ((page_shift) - 3) + 3)

#define MMU_GUEST_SIZE_SHIFT    48

// TODO: PTE size
typedef uint64_t pte_t;

#endif // __ASSEMBLER__
