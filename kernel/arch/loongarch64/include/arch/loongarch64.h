#pragma once

#ifndef __ASSEMBLER__

#include <arch/loongarch64/iframe.h>
#include <sys/types.h>

#include <zircon/types.h>

__BEGIN_CDECLS

typedef struct {
    // uint8_t ctype;
    // bool write_through;
    // bool write_back;
    // bool read_alloc;
    // bool write_alloc;
    // uint32_t num_sets;
    // uint32_t associativity;
    // uint32_t line_size;
} loongarch64_cache_desc_t;

typedef struct {
    // uint8_t inner_boundary;
    // uint8_t lou_u;
    // uint8_t loc;
    // uint8_t lou_is;
    // loongarch64_cache_desc_t level_data_type[7];
    // loongarch64_cache_desc_t level_inst_type[7];
} loongarch64_cache_info_t;

void platform_irq(loongarch64_iframe_t* frame);

struct arch_exception_context {
    struct iframe_t* frame;
};

// Register state layout used by loongarch64_context_switch().
struct loongarch64_context_switch_frame {
    // TODO: What else regs do we need to save?
    uint64_t ra;   // link register
    uint64_t r22;  // callee-saved regs
    uint64_t r23;
    uint64_t r24;
    uint64_t r25;
    uint64_t r26;
    uint64_t r27;
    uint64_t r28;
    uint64_t r29;
    uint64_t r30;
    uint64_t r31;
};

void loongarch64_context_switch(vaddr_t* old_sp, vaddr_t new_sp);

__END_CDECLS

#endif // __ASSEMBLER__