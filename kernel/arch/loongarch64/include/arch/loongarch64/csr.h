#pragma once

#include <arch/loongarch64/intrinsics.h>

static inline uint32_t csr_readl(uint32_t reg)
{
	return __csrrd(reg);
}

static inline uint64_t csr_readq(uint32_t reg)
{
	return __dcsrrd(reg);
}

static inline void csr_writel(uint32_t val, uint32_t reg)
{
	__csrwr(val, reg);
}

static inline void csr_writeq(uint64_t val, uint32_t reg)
{
	__dcsrwr(val, reg);
}

static inline uint32_t csr_xchgl(uint32_t val, uint32_t mask, uint32_t reg)
{
	return __csrxchg(val, mask, reg);
}

static inline uint64_t csr_xchgq(uint64_t val, uint64_t mask, uint32_t reg)
{
	return __dcsrxchg(val, mask, reg);
}

static inline uint32_t read_cpucfg(uint32_t reg)
{
	return __cpucfg(reg);
}

static inline uint64_t drdtime(void)
{
	int rID = 0;
	uint64_t val = 0;

	__asm__ __volatile__(
		"rdtime.d %0, %1 \n\t"
		: "=r"(val), "=r"(rID)
		:
		);
	return val;
}