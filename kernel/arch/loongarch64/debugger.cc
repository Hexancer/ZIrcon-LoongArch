#include <arch.h>
#include <arch/loongarch64.h>
//#include <arch/loongarch64/registers.h>
//#include <arch/debugger.h>
#include <err.h>
#include <kernel/thread.h>
#include <kernel/thread_lock.h>
#include <string.h>
#include <sys/types.h>
#include <zircon/syscalls/debug.h>
#include <zircon/types.h>

uint8_t arch_get_hw_breakpoint_count() {
    // TODO: return loongarch64_hw_breakpoint_count();
    return 0;
}

uint8_t arch_get_hw_watchpoint_count() {
    // TODO: return arm64_hw_watchpoint_count();
    return 0;
}
