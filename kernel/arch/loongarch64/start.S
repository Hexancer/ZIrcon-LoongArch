#include <asm.h>
#include <arch/loongarch64/mmu.h>
#include <arch/loongarch64.h>
#include <arch/asm_macros.h>
#include <arch/defines.h>
#include <zircon/tls.h>

#ifndef __has_feature
#define __has_feature(x) 0
#endif

#define STACK_SIZE  4096

// This code is purely position-independent and generates no relocations
// that need boot-time fixup; gen-kaslr-fixup.sh ensures this (and would
// ignore it if this code were in .text.boot, so don't put it there).
.text
FUNCTION(_start)
    // disable interrupt
    li.d        $t0, CSR_CRMD_IE
    csrxchg     $r0, $t0, LOONGARCH_CSR_CRMD 

    // clear bss segment
    la.pcrel    $t0, __bss_start            // zero out [__bss_start, _end)
    la.pcrel    $t1, _end
    beq         $t0, $t1,  clear_bss_done   //.bss 段大小为 0 则跳转 clear_bss_done
clear_bss_loop:
    st.d        $r0, $t0, 0
    addi.d      $t0, $t0, 8
    bne         $t0, $t1,  clear_bss_loop
clear_bss_done:

    // save boot parameters ($a0, $a1)
    la.pcrel $t0, zbi_paddr
    st.d     $a0, $t0, 0
    la.pcrel $t0, kernel_entry_paddr
    st.d     $a1, $t0, 0

    /* save the physical address the kernel is loaded at */
    //保存内核开始地址到 kernel_base_phys 全局变量
    la.pcrel $t0, __code_start
    la.pcrel $t1, kernel_base_phys
    st.d     $t0, $t1, 0

    // setup stack pointer
    la.pcrel $sp, boot_cpu_kstack_end

    /* make sure the boot allocator is given a chance to figure out where
     * we are loaded in physical memory. */
    bl       boot_alloc_init

    // config direct window and set PG
    //   this should have been done by boot-shim,
    //   but we still repeat it here
    li.d        $t0, CSR_DMW0_INIT   # Uncached, PLV0, 0x8000 xxxx xxxx xxxx
    csrwr       $t0, LOONGARCH_CSR_DMWIN0
    li.d        $t0, CSR_DMW1_INIT   # Cached,   PLV0, 0x9000 xxxx xxxx xxxx
    csrwr       $t0, LOONGARCH_CSR_DMWIN1

    la.pcrel    $t0,   dmw_jmup           // 把标号dmw_jump的地方的代码地址装入寄存器t0
    li.d        $t1,   CSR_DMW1_BASE
    or          $t0,   $t0, $t1           // 通过有缓存直接映射窗口(DMW1)执行接下来的指令
    jirl        $zero, $t0, 0             // 跳转到t0，此时t0是链接是用的高位没有抹去0的虚拟地址，而前面设置的窗口保证了它能访问到bios将其装入的那段物理内存
dmw_jmup:

    // setup stack pointer again with virt addr
    la.pcrel $sp, boot_cpu_kstack_end

    // TODO: do aslr fixup in the future

    // setup page table, map kernel space to $kernel_base
    bl loongarch64_map_kernel_aspace

    // setup PWCTL
    li.d     $t0,  0x4d52c     // (9 << 15) | (21 << 10) | (9 << 5) | 12
    csrwr    $t0,  LOONGARCH_CSR_PWCTL0
    li.d     $t0,  0x25e       // (9 << 6)  | 30
	csrwr    $t0,  LOONGARCH_CSR_PWCTL1

    // setup PGD
    la.pcrel $t0, loongarch64_kernel_translation_table // Get PA of PGD
    li.d     $t1, 0xffffffffff
    and      $t0, $t0, $t1
    csrwr    $t0, LOONGARCH_CSR_PGDH // write PGDH
    csrwr    $t0, LOONGARCH_CSR_PGDL // write PGDL

    // TODO: setup exception handler routine, especially TLB refill helper
    la.pcrel $t0, entry_tlb_refill
    csrwr    $t0, LOONGARCH_CSR_TLBRENTRY

    la.pcrel $t0, entry_exception
    csrwr    $t0, LOONGARCH_CSR_EENTRY


    // invalidate TLB entries
    invtlb   0, $r0, $r0
    // memory barrier
    dbar     0

    // 已经设置好页表, 现在跳转到基于分页机制的虚拟地址上
    //   跳转PC = (标号PC - 代码段起始位置) + 重定位地址
    la.pcrel    $t0,   paging_jump           // t0 = 标号PC (paging_jump)
    la.pcrel    $t1,   __code_start          // t1 = 代码段起始位置
    sub.d       $t0,   $t0, $t1              // t0 = (标号PC - 代码段起始位置)
    la.pcrel    $t1,   kernel_relocated_base // t1 = 重定位地址的指针
    ld.d        $t1,   $t1, 0                // t1 = 重定位地址
    add.d       $t0,   $t0, $t1              // t0 = (标号PC - 代码段起始位置) + 重定位地址
    jirl        $zero, $t0, 0                // 跳转到t0

paging_jump:
    // now we have setup page table and jump to paging virtual address

    // set the per cpu pointer for cpu 0
    la.pcrel    $r21,  loongarch64_percpu_array
    
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
