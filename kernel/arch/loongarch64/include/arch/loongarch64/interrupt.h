#pragma once

#ifndef __ASSEMBLER__

#include <arch.h>
#include <arch/loongarch64/csr.h>
#include <kernel/atomic.h>

__BEGIN_CDECLS

static inline void arch_enable_ints(void) {
	csr_xchgl(CSR_CRMD_IE, CSR_CRMD_IE, LOONGARCH_CSR_CRMD);
}

static inline void arch_disable_ints(void) {
    csr_xchgl(0, CSR_CRMD_IE, LOONGARCH_CSR_CRMD);
}

static inline bool arch_ints_disabled(void) {
	return !(csr_readl(LOONGARCH_CSR_CRMD) & CSR_CRMD_IE);
}

__END_CDECLS

#endif // __ASSEMBLER__
