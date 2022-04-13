#pragma once

#ifndef __ASSEMBLER__

#include <arch/defines.h>

#include <zircon/compiler.h>
#include <stdint.h>

// exception handling
// This is the main struct used by architecture-independent code.
// It can be forward declared thus this is the "real" type and
// loongarch64_iframe_t is the alias.
struct iframe_t {
    uint64_t gpr[BASE_NUM];
    uint64_t csr[CSR_NUM];
    uint64_t fpr[FP_BASE_NUM];
};

// Lots of the code uses this name.
typedef struct iframe_t loongarch64_iframe_t;

static_assert(sizeof(iframe_t) % 16 == 0, "iframe_t should be 16B-aligned");

#endif // !__ASSEMBLER__