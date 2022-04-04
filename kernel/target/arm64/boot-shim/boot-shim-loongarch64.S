// Copyright 2018 The Fuchsia Authors
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#include <asm.h>
#include <arch/asm_macros.h>
#include <zircon/boot/image.h>

#define CSR_DMW0_PLV0		0x0000000000000001  // _CONST64_(1 << 0)
#define CSR_DMW0_VSEG		0x0000000000008000  // _CONST64_(0x8000)
#define CSR_DMW0_BASE		0x8000000000000000  // (CSR_DMW0_VSEG << DMW_PABITS)
#define CSR_DMW0_INIT		0x8000000000000001  // (CSR_DMW0_BASE | CSR_DMW0_PLV0)

#define CSR_DMW1_PLV0		0x0000000000000001  // _CONST64_(1 << 0)
#define CSR_DMW1_MAT		0x0000000000000010  // _CONST64_(1 << 4)
#define CSR_DMW1_VSEG		0x0000000000009000  // _CONST64_(0x9000)
#define CSR_DMW1_BASE		0x9000000000000000  // (CSR_DMW1_VSEG << DMW_PABITS)
#define CSR_DMW1_INIT		0x9000000000000011  // (CSR_DMW1_BASE | CSR_DMW1_MAT | CSR_DMW1_PLV0)

#define LOONGARCH_CSR_DMWIN0		0x180	/* 64 direct map win0: MEM & IF */
#define LOONGARCH_CSR_DMWIN1		0x181	/* 64 direct map win1: MEM & IF */
#define LOONGARCH_CSR_DMWIN2		0x182	/* 64 direct map win2: MEM */
#define LOONGARCH_CSR_DMWIN3		0x183	/* 64 direct map win3: MEM */

#define LOONGARCH_CSR_CRMD		0x0	/* Current mode info */

#define STACK_SIZE  4096

.section .text.boot0,"ax"//ax是啥？
FUNCTION(_start)
    // typically points to device tree at entry

    /* Config direct window and set PG */
    li.d        $t0, CSR_DMW0_INIT   # UC, PLV0, 0x8000 xxxx xxxx xxxx
    csrwr       $t0, LOONGARCH_CSR_DMWIN0
    li.d        $t0, CSR_DMW1_INIT   # CA, PLV0, 0x9000 xxxx xxxx xxxx
    csrwr       $t0, LOONGARCH_CSR_DMWIN1

    /* We might not get launched at the address the kernel is linked to,
     * so we jump there. We must setup direct access window before this.
     */
    la.abs      $t0,   virt_addr_jump //把标号virt_addr_jump的地方的代码地址装入寄存器t0
    jirl        $zero, $t0, 0         //跳转到t0，此时t0是链接是用的高位没有抹去0的虚拟地址，而前面设置的窗口保证了它能访问到bios将其装入的那段物理内存

virt_addr_jump:

    // Disable caches and MMU
cache_disable:
cache_disable_done:
    // setup stack
    // x0: pointer to device tree
    la.abs  $sp, stack_end
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
