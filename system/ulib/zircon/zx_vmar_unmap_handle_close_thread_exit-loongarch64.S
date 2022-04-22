#include "syscall-entry.h"
#include "zircon-syscall-loongarch64.S"
#include <zircon/zx-syscall-numbers.h>

.text

.cfi_sections .eh_frame, .debug_frame

// (vmar_handle: a0, addr: a1, len: a2, handle: a3)
syscall_entry_begin zx_vmar_unmap_handle_close_thread_exit

    // Save the handle argument in a callee-saves register (x19).
    // Callee-save that register so we can unwind in the error case.
    // We only need to save x19, but we have to keep SP aligned to 16
    // bytes, so we always push and pop registers in pairs.
    push_reg 23 // $s0
    push_reg 24 // $s1
    move $s0, $a3

    zircon_syscall ZX_SYS_vmar_unmap, zx_vmar_unmap, zx_vmar_unmap_handle_close_thread_exit
    bnez $a0, .Lvmar_unmap_fail

    // Now the stack is gone and we can never return!

    move $a0, $s0
    zircon_syscall ZX_SYS_handle_close, zx_handle_close, zx_vmar_unmap_handle_close_thread_exit
    bnez $a0, .Lhandle_close_fail

    zircon_syscall ZX_SYS_thread_exit, zx_thread_exit, zx_vmar_unmap_handle_close_thread_exit

    // It should be impossible to get here.
.Lthread_exit_returned:
    break 0

.Lvmar_unmap_fail:
    pop_reg 24 // $s1
    pop_reg 23 // $s0
    jr  $ra

.Lhandle_close_fail:
    break 0

syscall_entry_end zx_vmar_unmap_handle_close_thread_exit
