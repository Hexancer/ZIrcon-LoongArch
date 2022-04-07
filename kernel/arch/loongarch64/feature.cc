#include "arch/loongarch64/feature.h"

#include <arch/loongarch64.h>
#include <bits.h>
#include <fbl/algorithm.h>
#include <inttypes.h>

// saved feature bitmap
uint32_t loongarch64_features;

static loongarch64_cache_info_t cache_info[SMP_MAX_CPUS];

// cache size parameters cpus, default to a reasonable minimum
uint32_t loongarch64_icache_size = 32;
uint32_t loongarch64_dcache_size = 32;

// call on every cpu to save features
void loongarch64_feature_init() {
    // set up some global constants based on the boot cpu
    cpu_num_t cpu = arch_curr_cpu_num();
    if (cpu == 0) {
        // read the dcache and icache line size
        unsigned int config;

        config = read_cpucfg(LOONGARCH_CPUCFG17);
        loongarch64_icache_size = 1 << ((config & CPUCFG17_L1I_SIZE_M) >> CPUCFG17_L1I_SIZE);

        config = read_cpucfg(LOONGARCH_CPUCFG18);
        loongarch64_dcache_size = 1 << ((config & CPUCFG18_L1D_SIZE_M) >> CPUCFG18_L1D_SIZE);

        config = read_cpucfg(LOONGARCH_CPUCFG2);
        if (config & CPUCFG2_FP) {
            loongarch64_features |= ZX_LOONGARCH64_FEATURE_ISA_FPU;
        }

        // TODO: add more features here
    }

    // read the cache info for each cpu
    loongarch64_get_cache_info(&(cache_info[cpu]));

    // TODO: Why ARM implementation need to support 16 bit ASID
}


void loongarch64_get_cache_info(loongarch64_cache_info_t* info) {
    // TODO
}

void loongarch64_dump_cache_info(uint32_t cpu) {
    // TODO
}