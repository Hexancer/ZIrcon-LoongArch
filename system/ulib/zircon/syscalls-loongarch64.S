/* define and implement the zircon syscall wrappers for loongarch64 */

#include "syscall-entry.h"
#include "zircon-syscall-loongarch64.S"

.text

.cfi_sections .eh_frame, .debug_frame

.macro m_syscall name, num, nargs, public
syscall_entry_begin \name
    zircon_syscall \num, \name, \name
    ertn
syscall_entry_end \name \public
.endm

#include <zircon/syscalls-loongarch64.S>

#include "syscalls-stubs.S"
