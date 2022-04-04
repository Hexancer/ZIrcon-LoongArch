// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SYSROOT_ZIRCON_BOOT_IMAGE_H_
#define SYSROOT_ZIRCON_BOOT_IMAGE_H_

// This file contains assembly code that cannot be clang formatted.
// clang-format off

#ifndef __ASSEMBLER__
#include <stdint.h>
#endif

// Zircon Boot Image format (ZBI).
//
// A Zircon Boot Image consists of a container header followed by boot
// items.  Each boot item has a header (zbi_header_t) and then a payload of
// zbi_header_t.length bytes, which can be any size.  The zbi_header_t.type
// field indicates how to interpret the payload.  Many types specify an
// additional type-specific header that begins a variable-sized payload.
// zbi_header_t.length does not include the zbi_header_t itself, but does
// include any type-specific headers as part of the payload.  All fields in
// all header formats are little-endian.
/*Zircon Boot Image由启动项和头容器组成，启动项紧跟着头容器。每个启动项都有一个头（zbi_header_t），
然后是一个zbi_header_t.length字节的有效负载，可以是任何大小。zbi_header_t.type字段
指出如何解释有效负载。许多类型指定了一个额外的特定于类型的头，该头开始一个可变大小的负载。
zbi_header_t.length不包括zbi_header_t本身，但包括作为有效负载一部分的任何特定类型的头。
所有头格式的所有字段都是little endian。*/
//
// Padding bytes appear after each item as needed to align the payload size
// up to a ZBI_ALIGNMENT (8-byte) boundary.  This padding is not reflected
// in the zbi_header_t.length value.
/*每个项的后部都有填充字节，根据需要将负载大小对齐到ZBI_ALIGNMENT(8字节)边界。
这个填充没有反映在zbi_header_t.length的值中。*/
//
// A "complete" ZBI can be booted by a Zircon-compatible boot loader.
// It contains one ZBI_TYPE_KERNEL_{ARCH} boot item that must come first,
// followed by any number of additional boot items, which must include
// exactly one ZBI_TYPE_STORAGE_BOOTFS item.
/*一个“完整的”ZBI可以由一个与zircon兼容的引导加载程序引导。它包含一个必须在前面的
ZBI_TYPE_KERNEL_{ARCH}启动项，后面跟着任意数量的其他启动项，这些启动项必须恰好包含一个
ZBI_TYPE_STORAGE_BOOTFS启动项。*/
//
// A partial ZBI cannot be booted, and is only used during the build process.
// It contains one or more boot items and can be combined with other ZBIs to
// make a complete ZBI.
/*部分ZBI不能引导，只能在构建过程中使用。它包含一个或多个启动项，并且可以与其他ZBI组合在
一起，从而形成一个完整的ZBI。*/
// All items begin at an 8-byte aligned offset into the image.
//所有项目都以8字节对齐的偏移量开始。
#ifdef __ASSEMBLER__
#define ZBI_ALIGNMENT           (8)
#else
#define ZBI_ALIGNMENT           (8u)
#endif

// Round n up to the next 8 byte boundary
#ifndef __ASSEMBLER__
#ifdef __cplusplus
constexpr
#endif
static inline uint32_t ZBI_ALIGN(uint32_t n) {
    return ((n + ZBI_ALIGNMENT - 1) & -ZBI_ALIGNMENT);
}
#endif

// LSW of sha256("bootdata")
#define ZBI_CONTAINER_MAGIC     (0x868cf7e6)

// LSW of sha256("bootitem")
#define ZBI_ITEM_MAGIC          (0xb5781729)

// This flag is always required.
#define ZBI_FLAG_VERSION        (0x00010000)

// ZBI items with the CRC32 flag must have a valid crc32.
// Otherwise their crc32 field must contain ZBI_ITEM_NO_CRC32
#define ZBI_FLAG_CRC32          (0x00020000)

// Value for zbi_header_t.crc32 when ZBI_FLAG_CRC32 is not set.
#define ZBI_ITEM_NO_CRC32       (0x4a87e8d6)

#ifndef __ASSEMBLER__
// Each header must be 8-byte aligned.  The length field specifies the
// actual payload length and does not include the size of padding.
//每个头必须是8字节对齐的。length字段指定实际有效负载长度，不包括填充的大小。
typedef struct {
    // ZBI_TYPE_* constant, see below.
    uint32_t type;

    // Size of the payload immediately following this header.  This
    // does not include the header itself nor any alignment padding
    // after the payload.
    //紧接着这个header的payload的大小。这不包括header本身，也不包括payload后的任何对齐填充。
    uint32_t length;

    // Type-specific extra data.  Each type specifies the use of this
    // field; see below.  When not explicitly specified, it should be zero.
    uint32_t extra;

    // Flags for this item.  This must always include ZBI_FLAG_VERSION.
    // It should contain ZBI_FLAG_CRC32 for any item where it's feasible
    // to compute the CRC32 at build time.  Other flags are specific to
    // each type; see below.
    //项的Flags。这必须始终包括ZBI_FLAG_version。它应该包含ZBI_FLAG_CRC32，用于
    //在构建时计算CRC32是可行的任何项目。其他标志针对每种类型；见下文。
    uint32_t flags;

    // For future expansion.  Set to 0.
    uint32_t reserved0;
    uint32_t reserved1;

    // Must be ZBI_ITEM_MAGIC.
    uint32_t magic;

    // Must be the CRC32 of payload if ZBI_FLAG_CRC32 is set,
    // otherwise must be ZBI_ITEM_NO_CRC32.
    uint32_t crc32;
} zbi_header_t;
#endif

// Be sure to add new types to ZBI_ALL_TYPES.
#define ZBI_ALL_TYPES(macro) \
    macro(ZBI_TYPE_CONTAINER, "CONTAINER", ".bin") \
    macro(ZBI_TYPE_KERNEL_X64, "KERNEL_X64", ".bin") \
    macro(ZBI_TYPE_KERNEL_ARM64, "KERNEL_ARM64", ".bin") \
    macro(ZBI_TYPE_KERNEL_LOONGARCH64, "KERNEL_LOONGARCH64", ".bin") \
    macro(ZBI_TYPE_DISCARD, "DISCARD", ".bin") \
    macro(ZBI_TYPE_STORAGE_RAMDISK, "RAMDISK", ".bin") \
    macro(ZBI_TYPE_STORAGE_BOOTFS, "BOOTFS", ".bin") \
    macro(ZBI_TYPE_STORAGE_BOOTFS_FACTORY, "BOOTFS_FACTORY", ".bin") \
    macro(ZBI_TYPE_CMDLINE, "CMDLINE", ".txt") \
    macro(ZBI_TYPE_CRASHLOG, "CRASHLOG", ".bin") \
    macro(ZBI_TYPE_NVRAM, "NVRAM", ".bin") \
    macro(ZBI_TYPE_PLATFORM_ID, "PLATFORM_ID", ".bin") \
    macro(ZBI_TYPE_CPU_CONFIG, "CPU_CONFIG", ".bin") /* Deprecated */ \
    macro(ZBI_TYPE_CPU_TOPOLOGY, "CPU_TOPOLOGY", ".bin") \
    macro(ZBI_TYPE_MEM_CONFIG, "MEM_CONFIG", ".bin") \
    macro(ZBI_TYPE_KERNEL_DRIVER, "KERNEL_DRIVER", ".bin") \
    macro(ZBI_TYPE_ACPI_RSDP, "ACPI_RSDP", ".bin") \
    macro(ZBI_TYPE_SMBIOS, "SMBIOS", ".bin") \
    macro(ZBI_TYPE_EFI_MEMORY_MAP, "EFI_MEMORY_MAP", ".bin") \
    macro(ZBI_TYPE_EFI_SYSTEM_TABLE, "EFI_SYSTEM_TABLE", ".bin") \
    macro(ZBI_TYPE_E820_TABLE, "E820_TABLE", ".bin") \
    macro(ZBI_TYPE_DEBUG_UART, "DEBUG_UART", ".bin") \
    macro(ZBI_TYPE_FRAMEBUFFER, "FRAMEBUFFER", ".bin") \
    macro(ZBI_TYPE_DRV_MAC_ADDRESS, "DRV_MAC_ADDRESS", ".bin") \
    macro(ZBI_TYPE_DRV_PARTITION_MAP, "DRV_PARTITION_MAP", ".bin") \
    macro(ZBI_TYPE_DRV_BOARD_PRIVATE, "DRV_BOARD_PRIVATE", ".bin") \
    macro(ZBI_TYPE_BOOT_CONFIG, "BOOT_CONFIG", ".bin") \
    macro(ZBI_TYPE_BOOT_VERSION, "BOOT_VERSION", ".bin")
/*ZBI{
    container header->length,extra,flags ...
    boot item1->{ zbi_header_t {zbi_header_t.length,zbi_header_t.type ...} + payload + padding bytes }
    boot item2-> ^^^^^
    boot item3-> ^^^^^
    boot itemN
                     
    }                 
*/
// Each ZBI starts with a container header.
//     length:          Total size of the image after this header.
//                      This includes all item headers, payloads, and padding.
//                      It does not include the container header itself.
//                      Must be a multiple of ZBI_ALIGNMENT.
//     extra:           Must be ZBI_CONTAINER_MAGIC.
//     flags:           Must be ZBI_FLAG_VERSION and no other flags.
#define ZBI_TYPE_CONTAINER      (0x544f4f42) // BOOT

// Define a container header in assembly code.  The symbol name is defined
// as a local label; use .global symbol to make it global.  The length
// argument can use assembly label arithmetic like any immediate operand.
#ifdef __ASSEMBLER__ //为什么定义__ASSEMBLER__??
#define ZBI_CONTAINER_HEADER(symbol, length)    \
    .balign ZBI_ALIGNMENT;                      \
    symbol:                                     \
        .int ZBI_TYPE_CONTAINER;                \
        .int (length);                          \
        .int ZBI_CONTAINER_MAGIC;               \
        .int ZBI_FLAG_VERSION;                  \
        .int 0;                                 \
        .int 0;                                 \
        .int ZBI_ITEM_MAGIC;                    \
        .int ZBI_ITEM_NO_CRC32;                 \
    .size symbol, . - symbol;                   \
    .type symbol, %object
#else
#define ZBI_CONTAINER_HEADER(length) {          \
    ZBI_TYPE_CONTAINER,                         \
    (length),                                   \
    ZBI_CONTAINER_MAGIC,                        \
    ZBI_FLAG_VERSION,                           \
    0,                                          \
    0,                                          \
    ZBI_ITEM_MAGIC,                             \
    ZBI_ITEM_NO_CRC32,                          \
}
#endif


// The kernel image.  In a complete ZBI this item must always be first,
// immediately after the ZBI_TYPE_CONTAINER header.  The contiguous memory
// image of the kernel is formed from the ZBI_TYPE_CONTAINER header, the
// ZBI_TYPE_KERNEL_{ARCH} header, and the payload.

/*
内核映像。在完整的ZBI中，该项目必须始终位于第一位，紧跟在ZBI_TYPE_CONTAINER头之后。
内核的连续内存映像由ZBI_TYPE_CONTAINER头、ZBI_TYPE_kernel_{ARCH}头和有效负载payload组成。
*/
/*
ZBI{
    container header->length,extra,flags ...
    kernel image(KERNEL_X64)->{ zbi_header_t {zbi_header_t.length,zbi_header_t.type ...} + payload + padding bytes }
    boot item2-> ^^^^^
    boot item3-> ^^^^^
    boot itemN
                     
    }                 
*/
//
// The boot loader loads the whole image starting with the container header
// through to the end of the kernel item's payload into contiguous physical
// memory.  It then constructs a partial ZBI elsewhere in memory, which has
// a ZBI_TYPE_CONTAINER header of its own followed by all the other items
// that were in the booted ZBI plus other items synthesized by the boot
// loader to describe the machine.  This partial ZBI must be placed at an
// address (where the container header is found) that is aligned to the
// machine's page size.  The precise protocol for transferring control to
// the kernel's entry point varies by machine.
/*
引导加载程序bootloader将整个映像image从容器头一直加载到内核项有效负载的末尾，加载到连续的物理内存中。
然后，它在内存中的其他地方构造一个部分ZBI，它有一个自己的ZBI_TYPE_容器头，
后面是booted ZBI中的所有其他项，再加上启动加载程序合成的用于描述机器的其他项。
此部分ZBI必须放置在与机器页面大小对齐的地址（找到容器标题的位置）。
将控制转移到内核入口点的精确协议因机器而异
*/
//
// On all machines, the kernel requires some amount of scratch memory to be
// available immediately after the kernel image at boot.  It needs this
// space for early setup work before it has a chance to read any memory-map
// information from the boot loader.  The `reserve_memory_size` field tells
// the boot loader how much space after the kernel's load image it must
// leave available for the kernel's use.  The boot loader must place its
// constructed ZBI or other reserved areas at least this many bytes after
// the kernel image.
/*
在所有机器上，启动时内核需要在内核映像处于启动后立即提供一定量的临时内存。
在有机会从引导加载程序读取任何内存映射信息之前，它需要这个空间来进行早期设置工作。
“reserve_memory_size”字段告诉引导加载程序在内核的加载映像之后必须留出多少空间供内核使用。
引导加载程序必须将其构造的ZBI或其他保留区域至少放在内核映像之后这么多字节
*/
//
// x86-64
//
//     The kernel assumes it was loaded at a fixed physical address of
//     0x100000 (1MB).  zbi_kernel_t.entry is the absolute physical address
//     of the PC location where the kernel will start.
//     TODO(SEC-31): Perhaps this will change??
//     The processor is in 64-bit mode with direct virtual to physical
//     mapping covering the physical memory where the kernel and
//     bootloader-constructed ZBI were loaded, which must be below 4GB.
//     The %rsi register (or %esi, since the high 32 bits must be zero)
//     holds the physical address of the bootloader-constructed ZBI.
//     All other registers are unspecified.

/*
内核假定它是在0x100000（1MB）的固定物理地址加载的。zbi_kernel_t.entry是内核将启动的PC位置的绝对物理地址。
处理器处于64位模式，直接虚拟到物理映射覆盖加载内核和引导加载程序构造的ZBI的物理内存，
该内存必须低于4GB。%rsi寄存器（或%esi，因为高32位必须为零）保存引导加载程序ZBI的物理地址。
所有其他寄存器均未指明。*/
//
//  ARM64
//
//     zbi_kernel_t.entry is an offset from the beginning of the image
//     (i.e., the ZBI_TYPE_CONTAINER header before the ZBI_TYPE_KERNEL_ARM64
//     header) to the PC location in the image where the kernel will
//     start.  The processor is in physical address mode at EL1 or
//     above.  The kernel image and the bootloader-constructed ZBI each
//     can be loaded anywhere in physical memory.  The x0 register
//     holds the physical address of the bootloader-constructed ZBI.
//     All other registers are unspecified.
/*
zbi_kernel_t.entry是从映像的开头（即zbi_TYPE_kernel_ARM64头之前的zbi_TYPE_容器头）
到映像中内核将启动的PC位置的偏移量。处理器处于EL1或以上的物理地址模式。
内核映像和ZBI构建的引导加载程序都可以加载到物理内存中的任何位置。x0寄存器保存引导加载程序ZBI的物理地址。
所有其他寄存器均未指明。
*/
//
#define ZBI_TYPE_KERNEL_PREFIX      (0x004e524b) // KRN\0
#define ZBI_TYPE_KERNEL_MASK        (0x00FFFFFF)
#define ZBI_TYPE_KERNEL_X64         (0x4c4e524b) // KRNL
#define ZBI_TYPE_KERNEL_ARM64       (0x384e524b) // KRN8
#define ZBI_TYPE_KERNEL_LOONGARCH64 (0x474e524b) // KRNG
#define ZBI_IS_KERNEL_BOOTITEM(x)   (((x) & ZBI_TYPE_KERNEL_MASK) ==  \
                                     ZBI_TYPE_KERNEL_PREFIX)

#ifndef __ASSEMBLER__
typedef struct {
    // Entry-point address.  The interpretation of this differs by machine.
    uint64_t entry;
    // Minimum amount (in bytes) of scratch memory that the kernel requires
    // immediately after its load image.
    uint64_t reserve_memory_size;
} zbi_kernel_t;

// The whole contiguous image loaded into memory by the boot loader.
typedef struct {
    zbi_header_t hdr_file;
    zbi_header_t hdr_kernel;
    zbi_kernel_t data_kernel;
    uint8_t contents[/*hdr_kernel.length - sizeof(zbi_kernel_t)*/];
    // data_kernel.reserve_memory_size bytes in memory are free after contents.
} zircon_kernel_t;
#endif


// A discarded item that should just be ignored.  This is used for an
// item that was already processed and should be ignored by whatever
// stage is now looking at the ZBI.  An earlier stage already "consumed"
// this information, but avoided copying data around to remove it from
// the ZBI item stream.
/*
一个应该被忽略的被丢弃的项目。这用于已经处理的项目，无论哪个阶段正在查看ZBI，
都应该忽略该项目。早期阶段已经“消耗”了这些信息，但避免了复制数据以将其从ZBI项目流中删除。
*/
#define ZBI_TYPE_DISCARD        (0x50494b53) // SKIP


// ZBI_TYPE_STORAGE_* types represent an image that might otherwise
// appear on some block storage device, i.e. a RAM disk of some sort.
// All zbi_header_t fields have the same meanings for all these types.
// The interpretation of the payload (after possible decompression) is
// indicated by the specific zbi_header_t.type value.
//
// **Note:** The ZBI_TYPE_STORAGE_* types are not a long-term stable ABI.
//  - Items of these types are always packed for a specific version of the
//    kernel and userland boot services, often in the same build that compiles
//    the kernel.
//  - These item types are **not** expected to be synthesized or
//    examined by boot loaders.
//  - New versions of the `zbi` tool will usually retain the ability to
//    read old formats and non-default switches to write old formats, for
//    diagnostic use.
//
// The zbi_header_t.extra field always gives the exact size of the
// original, uncompressed payload.  That equals zbi_header_t.length when
// the payload is not compressed.  If ZBI_FLAG_STORAGE_COMPRESSED is set in
// zbi_header_t.flags, then the payload is compressed.
//
// **Note:** Magic-number and header bytes at the start of the compressed
// payload indicate the compression algorithm and parameters.  The set of
// compression formats is not a long-term stable ABI.
//  - Zircon [userboot](../../../../docs/userboot.md) and core services
//    do the decompression.  A given kernel build's `userboot` will usually
//    only support one particular compression format.
//  - The `zbi` tool will usually retain the ability to compress and
//    decompress for old formats, and can be used to convert between formats.
#define ZBI_FLAG_STORAGE_COMPRESSED     (0x00000001)

// A virtual disk image.  This is meant to be treated as if it were a
// storage device.  The payload (after decompression) is the contents of
// the storage device, in whatever format that might be.
/*
虚拟磁盘映像。这意味着将其视为存储设备。有效载荷（解压后）是存储设备的内容，可以是任何格式。
*/
#define ZBI_TYPE_STORAGE_RAMDISK        (0x4b534452) // RDSK

// The /boot filesystem in BOOTFS format, specified in <zircon/boot/bootfs.h>.
// A complete ZBI must have exactly one ZBI_TYPE_STORAGE_BOOTFS item.
// Zircon [userboot](../../../../docs/userboot.md) handles the contents
// of this filesystem.
#define ZBI_TYPE_STORAGE_BOOTFS         (0x42534642) // BFSB

// Device-specific factory data, stored in BOOTFS format, specified below.
#define ZBI_TYPE_STORAGE_BOOTFS_FACTORY (0x46534642) // BFSF

// The remaining types are used to communicate information from the boot
// loader to the kernel.  Usually these are synthesized in memory by the
// boot loader, but they can also be included in a ZBI along with the
// kernel and BOOTFS.  Some boot loaders may set the zbi_header_t flags
// and crc32 fields to zero, though setting them to ZBI_FLAG_VERSION and
// ZBI_ITEM_NO_CRC32 is specified.  The kernel doesn't check.


// A kernel command line fragment, a NUL-terminated UTF-8 string.
// Multiple ZBI_TYPE_CMDLINE items can appear.  They are treated as if
// concatenated with ' ' between each item, in the order they appear:
// first items in the complete ZBI containing the kernel; then items in
// the ZBI synthesized by the boot loader.  The kernel interprets the
// [whole command line](../../../../docs/kernel_cmdline.md).
/*内核命令行片段，以NUL结尾的UTF-8字符串。可以显示多个ZBI_类型_CMDLINE项目。
它们被视为在每个项目之间以“”连接，顺序如下：包含内核的完整ZBI中的第一个项目；
然后由引导加载程序合成ZBI中的项。内核解释
//[整个命令行]（../../../../docs/kernel_cmdline.md）。
*/
#define ZBI_TYPE_CMDLINE                (0x4c444d43) // CMDL

// The crash log from the previous boot, a UTF-8 string.
#define ZBI_TYPE_CRASHLOG               (0x4d4f4f42) // BOOM

// Physical memory region that will persist across warm boots.
// zbi_nvram_t gives the physical base address and length in bytes.
#define ZBI_TYPE_NVRAM                  (0x4c4c564e) // NVLL
// This reflects a typo we need to support for a while.
#define ZBI_TYPE_NVRAM_DEPRECATED       (0x4c4c5643) // CVLL
#ifndef __ASSEMBLER__
typedef struct {
    uint64_t base;
    uint64_t length;
} zbi_nvram_t;
#endif

#define ZBI_BOARD_NAME_LEN 32

// Platform ID Information.
#define ZBI_TYPE_PLATFORM_ID            (0x44494C50) // PLID
#ifndef __ASSEMBLER__
typedef struct {
    uint32_t vid;
    uint32_t pid;
    char board_name[ZBI_BOARD_NAME_LEN];
} zbi_platform_id_t;
#endif

// CPU configuration, a zbi_cpu_config_t header followed by one or more
// zbi_cpu_cluster_t entries.  zbi_header_t.length must equal
// zbi_cpu_config_t.cluster_count * sizeof(zbi_cpu_cluster_t).
#define ZBI_TYPE_CPU_CONFIG             (0x43555043) // CPUC
#ifndef __ASSEMBLER__
typedef struct {
    // Number of CPU cores in the cluster.
    uint32_t cpu_count;

    // Reserved for future use.  Set to 0.
    uint32_t type;
    uint32_t flags;
    uint32_t reserved;
} zbi_cpu_cluster_t;

typedef struct {
    // Number of zbi_cpu_cluster_t entries following this header.
    uint32_t cluster_count;

    // Reserved for future use.  Set to 0.
    uint32_t reserved[3];

    // cluster_count entries follow.
    zbi_cpu_cluster_t clusters[];
} zbi_cpu_config_t;
#endif

#define ZBI_TYPE_CPU_TOPOLOGY           (0x544F504F) // TOPO

#ifndef __ASSEMBLER__

#define ZBI_MAX_SMT 4

// These are Used in the flags field of zbi_topology_processor_t.

// This is the processor that boots the system and the last to be shutdown.
#define ZBI_TOPOLOGY_PROCESSOR_PRIMARY 0b1

// This is the processor that handles all interrupts, some architectures will
// not have one.
#define ZBI_TOPOLOGY_PROCESSOR_INTERRUPT 0b10

#define ZBI_TOPOLOGY_NO_PARENT 0xFFFF

typedef enum {
    ZBI_TOPOLOGY_ARCH_UNDEFINED = 0, // Intended primarily for testing.
    ZBI_TOPOLOGY_ARCH_X86 = 1,
    ZBI_TOPOLOGY_ARCH_ARM = 2,
    ZBI_TOPOLOGY_ARCH_LOONGARCH = 3
} zbi_topology_architecture_t;

typedef struct {
    // Cluster ids for each level, one being closest to the cpu.
    // These map to aff1, aff2, and aff3 values in the ARM registers.
    uint8_t cluster_1_id;
    uint8_t cluster_2_id;
    uint8_t cluster_3_id;

    // Id of the cpu inside of the bottom-most cluster, aff0 value.
    uint8_t cpu_id;

    // The GIC interface number for this processor.
    // In GIC v3+ this is not necessary as the processors are addressed by their
    // affinity routing (all cluster ids followed by cpu_id).
    uint8_t gic_id;
}  zbi_topology_arm_info_t;

typedef struct {
    // TODO: anything to add here? 按qemu的来可以先模仿ARM很多可以复用
} zbi_topology_loongarch_info_t;

typedef struct {
    // Indexes here correspond to the logical_ids index for the thread.
    uint32_t apic_ids[ZBI_MAX_SMT];
    uint32_t apic_id_count;
}  zbi_topology_x86_info_t;

typedef struct {
    uint16_t logical_ids[ZBI_MAX_SMT];
    uint8_t logical_id_count;

    uint16_t flags;

    // Should be one of zbi_topology_arm_info_t.
    // If UNDEFINED then nothing will be set in arch_info.
    uint8_t architecture;
    union {
        zbi_topology_arm_info_t arm;
        zbi_topology_x86_info_t x86;
        zbi_topology_loongarch_info_t loongarch;
    } architecture_info;

} zbi_topology_processor_t;

typedef struct {
    // Relative performance level of this processor in the system, with 0
    // representing the lowest performance.
    // For example on a two cluster ARM big.LITTLE system 0 would be the little
    // cores and 1 would represent the big cores.
    uint8_t performance_class;
} zbi_topology_cluster_t;

typedef struct {
  // Starting and ending memory addresses of this numa region.
  uint64_t start_address;
  uint64_t end_address;
} zbi_topology_numa_region_t;

typedef enum {
    ZBI_TOPOLOGY_ENTITY_UNDEFINED = 0, // Unused default.
    ZBI_TOPOLOGY_ENTITY_PROCESSOR = 1,
    ZBI_TOPOLOGY_ENTITY_CLUSTER = 2,
    ZBI_TOPOLOGY_ENTITY_CACHE = 3,
    ZBI_TOPOLOGY_ENTITY_DIE = 4,
    ZBI_TOPOLOGY_ENTITY_SOCKET = 5,
    ZBI_TOPOLOGY_ENTITY_POWER_PLANE = 6,
    ZBI_TOPOLOGY_ENTITY_NUMA_REGION = 7,
} zbi_topology_entity_type_t;

typedef struct {
    // Should be one of zbi_topology_entity_type_t.
    uint8_t entity_type;
    uint16_t parent_index;
    union {
        zbi_topology_processor_t processor;
        zbi_topology_cluster_t cluster;
        zbi_topology_numa_region_t numa_region;
    } entity;
} zbi_topology_node_t;

#endif

// Memory configuration, one or more zbi_mem_range_t entries.
// zbi_header_t.length is sizeof(zbi_mem_range_t) times the number of entries.
#define ZBI_TYPE_MEM_CONFIG             (0x434D454D) // MEMC
#ifndef __ASSEMBLER__
typedef struct {
    uint64_t    paddr;
    uint64_t    length;
    uint32_t    type;
    uint32_t    reserved;
} zbi_mem_range_t;
#endif
#define ZBI_MEM_RANGE_RAM               (1)
#define ZBI_MEM_RANGE_PERIPHERAL        (2)
#define ZBI_MEM_RANGE_RESERVED          (3)

// Kernel driver configuration.  The zbi_header_t.extra field gives a
// KDRV_* type that determines the payload format.
// See [driver-config.h](<zircon/boot/driver-config.h>) for details.
#define ZBI_TYPE_KERNEL_DRIVER          (0x5652444B) // KDRV

// ACPI Root Table Pointer, a uint64_t physical address.
#define ZBI_TYPE_ACPI_RSDP              (0x50445352) // RSDP

// SMBIOS entry point, a uint64_t physical address.
#define ZBI_TYPE_SMBIOS                 (0x49424d53) // SMBI

// EFI memory map, a uint64_t entry size followed by a sequence of
// EFI memory descriptors aligned on that entry size.
#define ZBI_TYPE_EFI_MEMORY_MAP         (0x4d494645) // EFIM

// EFI system table, a uint64_t physical address.
#define ZBI_TYPE_EFI_SYSTEM_TABLE       (0x53494645) // EFIS

// E820 memory table, an array of e820entry_t.
#define ZBI_TYPE_E820_TABLE             (0x30323845) // E820

/* EFI Variable for Crash Log */
#define ZIRCON_VENDOR_GUID \
    {0x82305eb2, 0xd39e, 0x4575, {0xa0, 0xc8, 0x6c, 0x20, 0x72, 0xd0, 0x84, 0x4c}}
#define ZIRCON_CRASHLOG_EFIVAR \
    { 'c', 'r', 'a', 's', 'h', 'l', 'o', 'g', 0 };
#define ZIRCON_CRASHLOG_EFIATTR \
    (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)

// Debug serial port, a zbi_uart_t entry.
#define ZBI_TYPE_DEBUG_UART             (0x54524155) // UART
#ifndef __ASSEMBLER__
typedef struct {
    uint64_t base;
    uint32_t type;
    uint32_t irq;
} zbi_uart_t;
#endif
#define ZBI_UART_NONE                   (0)
#define ZBI_UART_PC_PORT                (1)
#define ZBI_UART_PC_MMIO                (2)

// Framebuffer parameters, a zbi_swfb_t entry.
#define ZBI_TYPE_FRAMEBUFFER            (0x42465753) // SWFB

// A copy of the boot configuration stored as a kvstore
// within the sysconfig partition.
#define ZBI_TYPE_BOOT_CONFIG        (0x47464342) // BCFG

// A copy of the boot version stored within the sysconfig
// partition
#define ZBI_TYPE_BOOT_VERSION       (0x53525642) // BVRS

#ifndef __ASSEMBLER__
typedef struct {
    // Physical memory address.
    uint64_t base;

    // Pixel layout and format.
    // See [../pixelformat.h](<zircon/pixelformat.h>).
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t format;
} zbi_swfb_t;
#endif


// ZBI_TYPE_DRV_* types (LSB is 'm') contain driver metadata.
#define ZBI_TYPE_DRV_METADATA(type)     (((type) & 0xFF) == 0x6D) // 'm'

// MAC address for Ethernet, Wifi, Bluetooth, etc.  zbi_header_t.extra
// is a board-specific index to specify which device the MAC address
// applies to.  zbi_header_t.length gives the size in bytes, which
// varies depending on the type of address appropriate for the device.
#define ZBI_TYPE_DRV_MAC_ADDRESS        (0x43414D6D) // mMAC

// A partition map for a storage device, a zbi_partition_map_t header
// followed by one or more zbi_partition_t entries.  zbi_header_t.extra
// is a board-specific index to specify which device this applies to.
#define ZBI_TYPE_DRV_PARTITION_MAP      (0x5452506D) // mPRT
#define ZBI_PARTITION_NAME_LEN          (32)
#define ZBI_PARTITION_GUID_LEN          (16)

// Private information for the board driver.
#define ZBI_TYPE_DRV_BOARD_PRIVATE      (0x524F426D) // mBOR

#ifndef __ASSEMBLER__
typedef struct {
    // GUID specifying the format and use of data stored in the partition.
    uint8_t type_guid[ZBI_PARTITION_GUID_LEN];

    // GUID unique to this partition.
    uint8_t uniq_guid[ZBI_PARTITION_GUID_LEN];

    // First and last block occupied by this partition.
    uint64_t first_block;
    uint64_t last_block;

    // Reserved for future use.  Set to 0.
    uint64_t flags;

    char name[ZBI_PARTITION_NAME_LEN];
} zbi_partition_t;

typedef struct {
    // Total blocks used on the device.
    uint64_t block_count;
    // Size of each block in bytes.
    uint64_t block_size;

    // Number of partitions in the map.
    uint32_t partition_count;

    // Reserved for future use.
    uint32_t reserved;

    // Device GUID.
    uint8_t guid[ZBI_PARTITION_GUID_LEN];

    // partition_count partition entries follow.
    zbi_partition_t partitions[];
} zbi_partition_map_t;
#endif

#endif  // SYSROOT_ZIRCON_BOOT_IMAGE_H_
