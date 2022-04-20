#include "asm.h"

/*
 * void *memcpy(void *dst, const void *src, size_t n)
 *
 * a0: dst
 * a1: src
 * a2: n
 */
ENTRY(memcpy)
	move	$a3, $a0
	beqz	$a2, 2f

1:	ld.b	$t0, $a1, 0
	st.b	$t0, $a0, 0
	addi.d	$a0, $a0, 1
	addi.d	$a1, $a1, 1
	addi.d	$a2, $a2, -1
	bgt	    $a2, $zero, 1b

2:	move	$v0, $a3
	jr	    $ra
END(memcpy)

#ifdef HIDDEN
    .hidden memcpy
#endif
ALIAS(memcpy, __unsanitized_memcpy)
ASAN_WEAK_ALIAS(memcpy)
