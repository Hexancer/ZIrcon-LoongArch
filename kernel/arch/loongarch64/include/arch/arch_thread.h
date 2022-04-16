#pragma once

#define CURRENT_PERCPU_PTR_OFFSET 16

#ifndef __ASSEMBLER__

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

__END_CDECLS

#endif // __ASSEMBLER__
