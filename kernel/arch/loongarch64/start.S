#include <asm.h>
#include <arch/loongarch64/mmu.h>
#include <arch/loongarch64.h>
#include <arch/asm_macros.h>
#include <arch/defines.h>
#include <zircon/tls.h>

#ifndef __has_feature
#define __has_feature(x) 0
#endif

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

// This code is purely position-independent and generates no relocations
// that need boot-time fixup; gen-kaslr-fixup.sh ensures this (and would
// ignore it if this code were in .text.boot, so don't put it there).
.text
FUNCTION(_start)
    // save boot parameters ($a0, $a1)
    la.pcrel $t0, zbi_paddr
    st.d     $a0, $t0, 0
    la.pcrel $t0, kernel_entry_paddr
    st.d     $a1, $t0, 0

    // config direct window and set PG
    //   this should have been done by boot-shim,
    //   but we still repeat it here
    li.d        $t0, CSR_DMW0_INIT   # Uncached, PLV0, 0x8000 xxxx xxxx xxxx
    csrwr       $t0, LOONGARCH_CSR_DMWIN0
    li.d        $t0, CSR_DMW1_INIT   # Cached,   PLV0, 0x9000 xxxx xxxx xxxx
    csrwr       $t0, LOONGARCH_CSR_DMWIN1

    la.pcrel    $t0,   virt_addr_jump     // 把标号virt_addr_jump的地方的代码地址装入寄存器t0
    li.d        $t1,   CSR_DMW1_BASE
    or          $t0,   $t0, $t1           // 通过有缓存直接映射窗口(DMW1)执行接下来的指令
    jirl        $zero, $t0, 0             // 跳转到t0，此时t0是链接是用的高位没有抹去0的虚拟地址，而前面设置的窗口保证了它能访问到bios将其装入的那段物理内存

virt_addr_jump:

    // setup stack pointer
    la.pcrel    $sp,   boot_cpu_kstack_end
    
    // setup page table
    //  enable MMU 
    bl loongarch64_boot_map
    
    // enter C routine
    b lk_main

    // should never reach here
    break 0
END_FUNCTION(_start)

// These are logically .bss (uninitialized data).  But they're set before
// clearing the .bss, so put them in .data so they don't get zeroed.
//静态数据，内核启动默认参数
.data
    .balign 64
DATA(arch_boot_plv)
    .quad -1
END_DATA(arch_boot_plv)
DATA(zbi_paddr)
    .quad -1
END_DATA(zbi_paddr)
DATA(kernel_entry_paddr)
    .quad -1
END_DATA(kernel_entry_paddr)

.bss
LOCAL_DATA(boot_cpu_kstack)
    .skip ARCH_DEFAULT_STACK_SIZE
    .balign 16
LOCAL_DATA(boot_cpu_kstack_end)
END_DATA(boot_cpu_kstack)

// This symbol is used by image.S
.global IMAGE_ELF_ENTRY
IMAGE_ELF_ENTRY = _start

// This symbol is used by gdb python to know the base of the kernel module
.global KERNEL_BASE_ADDRESS
KERNEL_BASE_ADDRESS = KERNEL_BASE
