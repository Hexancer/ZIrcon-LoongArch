#include "syscall-entry.h"
#include "zircon-syscall-arm64.S"
#include <zircon/zx-syscall-numbers.h>

.text

.cfi_sections .eh_frame, .debug_frame

// (vmar_handle: x0, addr: x1, len: x2, handle: x3)
syscall_entry_begin zx_vmar_unmap_handle_close_thread_exit

syscall_entry_end zx_vmar_unmap_handle_close_thread_exit
