#include "asm.h"
#include "setjmp_impl.h"
#include <zircon/tls.h>

ENTRY(longjmp)
	add.d   $t5, $a0, $zero
	add.d   $v0, $a1, $zero

	bne     $v0, $zero, 1f
	addi.d  $v0, $v0, 1

1:
	ld.d    $ra, $t5, 0
	ld.d    $sp, $t5, 8
	ld.d    $r21,$t5, 16
	ld.d    $fp, $t5, 24
	ld.d    $s0, $t5, 32
	ld.d    $s1, $t5, 40
	ld.d    $s2, $t5, 48
	ld.d    $s3, $t5, 56
	ld.d    $s4, $t5, 64
	ld.d    $s5, $t5, 72
	ld.d    $s6, $t5, 80
	ld.d    $s7, $t5, 88
	ld.d    $s8, $t5, 96
// #ifndef __loongarch64_soft_float
// 	fld.d   $fs0, $t5, 104
// 	fld.d   $fs1, $t5, 112
// 	fld.d   $fs2, $t5, 120
// 	fld.d   $fs3, $t5, 128
// 	fld.d   $fs4, $t5, 136
// 	fld.d   $fs5, $t5, 144
// 	fld.d   $fs6, $t5, 152
// 	fld.d   $fs7, $t5, 160
// #endif
	jirl    $zero, $ra, 0
END(longjmp)

ALIAS(longjmp, _longjmp)
WEAK_ALIAS(longjmp, siglongjmp)
