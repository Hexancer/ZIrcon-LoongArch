#pragma once

#include <arch/loongarch64.h>
#include <stdint.h>
#include <zircon/compiler.h>
#include <zircon/features.h>

__BEGIN_CDECLS

extern uint32_t loongarch64_features;

static inline bool loongarch64_feature_test(uint32_t feature) {

    return loongarch64_features & feature;
}

extern uint32_t loongarch64_icache_size;
extern uint32_t loongarch64_dcache_size;

// call on every cpu to initialize the feature set
void loongarch64_feature_init(void);

// dump the feature set
void loongarch64_feature_debug(bool full);

void loongarch64_get_cache_info(loongarch64_cache_info_t* info);
void loongarch64_dump_cache_info(uint32_t cpu);

__END_CDECLS
