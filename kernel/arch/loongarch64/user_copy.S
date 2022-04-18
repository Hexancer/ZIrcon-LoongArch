#include <asm.h>
#include <err.h>


#define dst  $a0
#define src  $a1
#define len  $a2
#define rptr $a3
#define temp $t0

// zx_status_t _loongarch64_user_copy(void *dst, const void *src, size_t len, void **fault_return)
.section .text._loongarch64_user_copy,"ax"
.balign 64 // Align to cache line.  This code fits in one cache line.
FUNCTION(_loongarch64_user_copy)

    // save resume point for page fault
    la.pcrel temp, .Lfault_from_user
    st.d     temp, rptr, 0

    // do copy (poor performance)
    beqz     len, .Luser_copy_done

.Luser_copy_loop:
    ld.b     temp, src, 0
    st.b     temp, dst, 0
    addi.d   len,  len, -1
    addi.d   src,  src, 1
    addi.d   dst,  dst, 1
    bnez     len, .Luser_copy_loop

    // copy finished without page fault
.Luser_copy_done:
    li.d     $a0, ZX_OK

.Luser_copy_return:
    st.d     $zero, rptr, 0
    jr       $ra
END_FUNCTION(_loongarch64_user_copy)

.section .text.cold._loongarch64_user_copy,"ax"
.Lfault_from_user:
    // copy triggered page fault
    .cfi_startproc
    li.d     $a0, ZX_ERR_INVALID_ARGS
    b .Luser_copy_return
    .cfi_endproc
