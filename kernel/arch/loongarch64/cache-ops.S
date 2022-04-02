// Copyright 2016 The Fuchsia Authors
// Copyright (c) 2014, Google Inc. All rights reserved
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT


#include <asm.h>
#include <arch/ops.h>
#include <arch/defines.h>

.text

    /* void arch_flush_cache_range(addr_t start, size_t len); */
FUNCTION(arch_clean_cache_range)
    break 0
END_FUNCTION(arch_clean_cache_range)

    /* void arch_flush_invalidate_cache_range(addr_t start, size_t len); */
FUNCTION(arch_clean_invalidate_cache_range)
    break 0
END_FUNCTION(arch_clean_invalidate_cache_range)

    /* void arch_invalidate_cache_range(addr_t start, size_t len); */
FUNCTION(arch_invalidate_cache_range)
    break 0
END_FUNCTION(arch_invalidate_cache_range)

    /* void arch_sync_cache_range(addr_t start, size_t len); */
FUNCTION(arch_sync_cache_range)
    break 0
END_FUNCTION(arch_sync_cache_range)

/* void arch_invalidate_cache_all()
 *      should only be used early in boot, prior to enabling mmu/cache
 */
FUNCTION(arch_invalidate_cache_all)
    break 0
END_FUNCTION(arch_invalidate_cache_all)

