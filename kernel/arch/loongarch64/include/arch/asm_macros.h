#pragma once

// Note: due to a bug in AS implementation on LoongArch,
//    ".cfi_rel_offset" directive can't recognize "$r"
//    prefix before the register number, so here we pass
//    bare register number instead of mnemonics
.macro push_reg reg
addi.d $sp,    $sp, -8
st.d   $r\reg, $sp, 0
.cfi_adjust_cfa_offset 8
.cfi_rel_offset \reg, 0
.endm

.macro pop_reg reg
ld.d   $r\reg, $sp, 0
addi.d $sp,    $sp, 8
.cfi_adjust_cfa_offset -8
.cfi_same_value \reg
.endm


.macro sub_from_sp value
addi.d sp, sp, -\value
.cfi_adjust_cfa_offset \value
.endm

.macro add_to_sp value
addi.d sp, sp, \value
.cfi_adjust_cfa_offset -\value
.endm
