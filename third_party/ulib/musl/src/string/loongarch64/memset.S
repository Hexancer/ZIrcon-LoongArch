#include "asm.h"

/*
 * void *memset(void *s, int c, size_t n)
 *
 * a0: s
 * a1: c
 * a2: n
 */
ENTRY(memset)
	move	$a3, $a0
	beqz	$a2, 2f

1:	st.b	$a1, $a0, 0
	addi.d	$a0, $a0, 1
	addi.d	$a2, $a2, -1
	bgt	    $a2, $zero, 1b

2:	move	$v0, $a3
	jr	    $ra
END(memset)

#ifdef HIDDEN
    .hidden memset
#else
ALIAS(memset, __unsanitized_memset)
ASAN_WEAK_ALIAS(memset)

// This name is called from inside libc to avoid going through the PLT.
.hidden __libc_memset
ALIAS(memset, __libc_memset)
#endif
