#include <asm.h>
#include <arch/asm_macros.h>

/* void loongarch64_context_switch(vaddr_t *old_sp, vaddr_t new_sp); */
FUNCTION(loongarch64_context_switch)
    // /* save old frame */
    // /* This layout should match struct context_switch_frame */
    push_reg 31
    push_reg 30
    push_reg 29
    push_reg 28
    push_reg 27
    push_reg 26
    push_reg 25
    push_reg 24
    push_reg 23
    push_reg 22
    push_reg 2   // $tp
    push_reg 1   // $ra

    /* save old sp */
    move  $t8, $sp
    st.d  $t8, $a0, 0

    /* load new sp */
    move  $sp, $a1

    /* restore new frame */
    pop_reg 1    // $ra
    pop_reg 2    // $tp
    pop_reg 22
    pop_reg 23
    pop_reg 24
    pop_reg 25
    pop_reg 26
    pop_reg 27
    pop_reg 28
    pop_reg 29
    pop_reg 30
    pop_reg 31

    jr $r1
END_FUNCTION(loongarch64_context_switch)