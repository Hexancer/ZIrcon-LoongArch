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

    // Point to the current cpu pointer when the thread is running, used to
    // restore x18 on exception entry. Swapped on context switch.
    struct loongarch64_percpu* current_percpu_ptr;
};

__END_CDECLS

#endif // __ASSEMBLER__
