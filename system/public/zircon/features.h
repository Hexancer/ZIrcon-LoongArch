// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SYSROOT_ZIRCON_FEATURES_H_
#define SYSROOT_ZIRCON_FEATURES_H_

// clang-format off

// types of features that can be retrieved via |zx_system_get_features|
#define ZX_FEATURE_KIND_CPU                   ((uint32_t)0)
#define ZX_FEATURE_KIND_HW_BREAKPOINT_COUNT   ((uint32_t)1)
#define ZX_FEATURE_KIND_HW_WATCHPOINT_COUNT   ((uint32_t)2)

// arch-independent CPU features
#define ZX_HAS_CPU_FEATURES            ((uint32_t)(1u << 0))

#if defined(__x86_64__)

// x86-64 CPU features
// None; use cpuid instead

#elif defined(__aarch64__)

// arm64 CPU features
#define ZX_ARM64_FEATURE_ISA_FP        ((uint32_t)(1u << 1))
#define ZX_ARM64_FEATURE_ISA_ASIMD     ((uint32_t)(1u << 2))
#define ZX_ARM64_FEATURE_ISA_AES       ((uint32_t)(1u << 3))
#define ZX_ARM64_FEATURE_ISA_PMULL     ((uint32_t)(1u << 4))
#define ZX_ARM64_FEATURE_ISA_SHA1      ((uint32_t)(1u << 5))
#define ZX_ARM64_FEATURE_ISA_SHA2      ((uint32_t)(1u << 6))
#define ZX_ARM64_FEATURE_ISA_CRC32     ((uint32_t)(1u << 7))
#define ZX_ARM64_FEATURE_ISA_ATOMICS   ((uint32_t)(1u << 8))
#define ZX_ARM64_FEATURE_ISA_RDM       ((uint32_t)(1u << 9))
#define ZX_ARM64_FEATURE_ISA_SHA3      ((uint32_t)(1u << 10))
#define ZX_ARM64_FEATURE_ISA_SM3       ((uint32_t)(1u << 11))
#define ZX_ARM64_FEATURE_ISA_SM4       ((uint32_t)(1u << 12))
#define ZX_ARM64_FEATURE_ISA_DP        ((uint32_t)(1u << 13))
#define ZX_ARM64_FEATURE_ISA_DPB       ((uint32_t)(1u << 14))

#elif defined(__loongarch64)

/*
 * CPU Option encodings
 */
#define ZX_LOONGARCH64_FEATURE_BIT_CPUCFG            0    /* CPU has CPUCFG */
#define ZX_LOONGARCH64_FEATURE_BIT_LAM               1    /* CPU has Atomic instructions */
#define ZX_LOONGARCH64_FEATURE_BIT_UAL               2    /* CPU has Unaligned Access support */
#define ZX_LOONGARCH64_FEATURE_BIT_FPU               3    /* CPU has FPU */
#define ZX_LOONGARCH64_FEATURE_BIT_LSX               4    /* CPU has 128bit SIMD instructions */
#define ZX_LOONGARCH64_FEATURE_BIT_LASX              5    /* CPU has 256bit SIMD instructions */
#define ZX_LOONGARCH64_FEATURE_BIT_COMPLEX           6    /* CPU has Complex instructions */
#define ZX_LOONGARCH64_FEATURE_BIT_CRYPTO            7    /* CPU has Crypto instructions */
#define ZX_LOONGARCH64_FEATURE_BIT_LVZ               8    /* CPU has Virtualization extension */
#define ZX_LOONGARCH64_FEATURE_BIT_LBT_X86           9    /* CPU has X86 Binary Translation */
#define ZX_LOONGARCH64_FEATURE_BIT_LBT_ARM           10    /* CPU has ARM Binary Translation */
#define ZX_LOONGARCH64_FEATURE_BIT_LBT_MIPS          11    /* CPU has MIPS Binary Translation */
#define ZX_LOONGARCH64_FEATURE_BIT_TLB               12    /* CPU has TLB */
#define ZX_LOONGARCH64_FEATURE_BIT_CSR               13    /* CPU has CSR feature */
#define ZX_LOONGARCH64_FEATURE_BIT_WATCH             14    /* CPU has watchpoint registers */
#define ZX_LOONGARCH64_FEATURE_BIT_VINT              15    /* CPU has vectored interrupts */
#define ZX_LOONGARCH64_FEATURE_BIT_CSRIPI            16    /* CPU has CSR-IPI */
#define ZX_LOONGARCH64_FEATURE_BIT_EXTIOI            17    /* CPU has EXT-IOI */
#define ZX_LOONGARCH64_FEATURE_BIT_PREFETCH          18    /* CPU has prefetch instructions */
#define ZX_LOONGARCH64_FEATURE_BIT_PMP               19    /* CPU has perfermance counter */
#define ZX_LOONGARCH64_FEATURE_BIT_SCALEFREQ         20    /* CPU support scale cpufreq */
#define ZX_LOONGARCH64_FEATURE_BIT_GUESTID           21    /* CPU has GuestID feature */
#define ZX_LOONGARCH64_FEATURE_BIT_HYPERVISOR        22    /* CPU has hypervisor (run in VM) */

#define BIT_ULL(nr)        (1ULL << (nr))

#define ZX_LOONGARCH64_FEATURE_ISA_CPUCFG          BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_CPUCFG)
#define ZX_LOONGARCH64_FEATURE_ISA_LAM             BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_LAM)
#define ZX_LOONGARCH64_FEATURE_ISA_UAL             BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_UAL)
#define ZX_LOONGARCH64_FEATURE_ISA_FPU             BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_FPU)
#define ZX_LOONGARCH64_FEATURE_ISA_LSX             BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_LSX)
#define ZX_LOONGARCH64_FEATURE_ISA_LASX            BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_LASX)
#define ZX_LOONGARCH64_FEATURE_ISA_COMPLEX         BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_COMPLEX)
#define ZX_LOONGARCH64_FEATURE_ISA_CRYPTO          BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_CRYPTO)
#define ZX_LOONGARCH64_FEATURE_ISA_LVZ             BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_LVZ)
#define ZX_LOONGARCH64_FEATURE_ISA_LBT_X86         BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_LBT_X86)
#define ZX_LOONGARCH64_FEATURE_ISA_LBT_ARM         BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_LBT_ARM)
#define ZX_LOONGARCH64_FEATURE_ISA_LBT_MIPS        BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_LBT_MIPS)
#define ZX_LOONGARCH64_FEATURE_ISA_TLB             BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_TLB)
#define ZX_LOONGARCH64_FEATURE_ISA_CSR             BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_CSR)
#define ZX_LOONGARCH64_FEATURE_ISA_WATCH           BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_WATCH)
#define ZX_LOONGARCH64_FEATURE_ISA_VINT            BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_VINT)
#define ZX_LOONGARCH64_FEATURE_ISA_CSRIPI          BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_CSRIPI)
#define ZX_LOONGARCH64_FEATURE_ISA_EXTIOI          BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_EXTIOI)
#define ZX_LOONGARCH64_FEATURE_ISA_PREFETCH        BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_PREFETCH)
#define ZX_LOONGARCH64_FEATURE_ISA_PMP             BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_PMP)
#define ZX_LOONGARCH64_FEATURE_ISA_SCALEFREQ       BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_SCALEFREQ)
#define ZX_LOONGARCH64_FEATURE_ISA_GUESTID         BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_GUESTID)
#define ZX_LOONGARCH64_FEATURE_ISA_HYPERVISOR      BIT_ULL(ZX_LOONGARCH64_FEATURE_BIT_HYPERVISOR)

#define ZX_LOONGARCH64_FEATURE_ISA_FPE ((uint32_t)(1u << 1))

#else

#error what architecture?

#endif

#endif // SYSROOT_ZIRCON_FEATURES_H_
