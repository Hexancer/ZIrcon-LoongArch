#pragma once

#ifndef __ASSEMBLER__

#include <arch/loongarch64/iframe.h>

#include <zircon/types.h>

struct arch_exception_context {
    struct iframe_t* frame;
};

#endif // __ASSEMBLER__