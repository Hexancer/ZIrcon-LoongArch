#pragma once

#define CURRENT_PERCPU_PTR_OFFSET 16

#ifndef __ASSEMBLER__

__BEGIN_CDECLS

struct arch_thread {
    // Point to the current cpu pointer when the thread is running, used to
    // restore x18 on exception entry. Swapped on context switch.
    struct loongarch64_percpu* current_percpu_ptr;
};

__END_CDECLS

#endif // __ASSEMBLER__
