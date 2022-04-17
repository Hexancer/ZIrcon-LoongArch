#pragma once

#define CURRENT_PERCPU_PTR_OFFSET 16

#ifndef __ASSEMBLER__

#include <assert.h>
#include <stddef.h>
#include <sys/types.h>
#include <zircon/compiler.h>
#include <zircon/tls.h>

__BEGIN_CDECLS

struct arch_thread {
    uintptr_t stack_guard;
    vaddr_t unsafe_sp;
    union {
        char thread_pointer_location;
        vaddr_t sp;
    };

    // Debugger access to userspace general regs while suspended or stopped
    // in an exception.
    // The regs are saved on the stack and then a pointer is stored here.
    // Nullptr if not suspended or not stopped in an exception.
    // TODO(ZX-563): Also nullptr for synthetic exceptions that don't provide
    // them yet.
    struct iframe_t* suspended_general_regs;

    // Point to the current cpu pointer when the thread is running, used to
    // restore x18 on exception entry. Swapped on context switch.
    struct loongarch64_percpu* current_percpu_ptr;
};

#define thread_pointer_offsetof(field)          \
    ((int)offsetof(struct arch_thread, field) - \
     (int)offsetof(struct arch_thread, thread_pointer_location))

static_assert(
    thread_pointer_offsetof(stack_guard) == ZX_TLS_STACK_GUARD_OFFSET,
    "stack_guard field in wrong place");
static_assert(
    thread_pointer_offsetof(unsafe_sp) == ZX_TLS_UNSAFE_SP_OFFSET,
    "unsafe_sp field in wrong place");
static_assert(
    thread_pointer_offsetof(current_percpu_ptr) == CURRENT_PERCPU_PTR_OFFSET,
    "per cpu ptr offset in wrong place");

__END_CDECLS

#endif // __ASSEMBLER__
