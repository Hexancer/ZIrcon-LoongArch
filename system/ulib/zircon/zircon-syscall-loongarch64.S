// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

.macro zircon_syscall num, name, caller
    // use T8 to store syscall number
    li.d $t8, \num
    syscall 0
// This symbol at the return address identifies this as an approved call site.
    .hidden CODE_SYSRET_\name\()_VIA_\caller
CODE_SYSRET_\name\()_VIA_\caller\():
.endm

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