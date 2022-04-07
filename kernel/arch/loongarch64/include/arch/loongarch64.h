#pragma once

#ifndef __ASSEMBLER__

#include <arch/loongarch64/iframe.h>

#include <zircon/types.h>

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

struct arch_exception_context {
    struct iframe_t* frame;
};

#endif // __ASSEMBLER__