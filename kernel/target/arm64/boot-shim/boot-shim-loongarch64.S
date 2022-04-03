// Copyright 2018 The Fuchsia Authors
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#include <asm.h>
#include <arch/asm_macros.h>
#include <zircon/boot/image.h>

#define SCTLR_I     (1 << 12)   // Instruction cache enable
#define SCTLR_C     (1 << 2)    // Cache enable
#define SCTLR_M     (1 << 0)    // MMU enable

#define STACK_SIZE  4096

.section .text.boot0,"ax"
FUNCTION(_start)
    // x0 typically points to device tree at entry

    // Disable caches and MM
cache_disable:
cache_disable_done:
    // setup stack
    // x0: pointer to device tree
    bl      boot_shim

    // x0: bootdata_t* to pass to kernel
    // x1: kernel entry point
    jirl    $r0, $ra, 0
END_FUNCTION(_start)

.bss
.balign 16
LOCAL_DATA(stack)
    .skip STACK_SIZE
LOCAL_DATA(stack_end)
END_DATA(stack)
