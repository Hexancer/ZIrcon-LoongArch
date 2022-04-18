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
    uint64_t csr[CSR_NUM - 2];
    uint64_t tp;  // KS3
    uint64_t __pad;
    uint64_t fpr[FP_BASE_NUM];
};

// Lots of the code uses this name.
typedef struct iframe_t loongarch64_iframe_t;
typedef struct iframe_t iframe_short_t;

static_assert(sizeof(iframe_t) % 16 == 0, "iframe_t should be 16B-aligned");

#endif // !__ASSEMBLER__

#define LOONGARCH64_IFRAME_OFFSET_R     0
#define LOONGARCH64_IFRAME_OFFSET_SP    (SP_NUM * 8)
#define LOONGARCH64_IFRAME_OFFSET_PRMD  ((BASE_NUM + LOONGARCH_CSR_PRMD) * 8)
#define LOONGARCH64_IFRAME_OFFSET_EPC   ((BASE_NUM + LOONGARCH_CSR_EPC) * 8)

#ifndef __ASSEMBLER__

static_assert(__offsetof(iframe_t, gpr[0]) == LOONGARCH64_IFRAME_OFFSET_R, "");
static_assert(__offsetof(iframe_t, gpr[SP_NUM]) == LOONGARCH64_IFRAME_OFFSET_SP, "");
static_assert(__offsetof(iframe_t, csr[LOONGARCH_CSR_EPC]) == LOONGARCH64_IFRAME_OFFSET_EPC, "");
static_assert(__offsetof(iframe_t, csr[LOONGARCH_CSR_PRMD]) == LOONGARCH64_IFRAME_OFFSET_PRMD, "");

#endif // !__ASSEMBLER__
