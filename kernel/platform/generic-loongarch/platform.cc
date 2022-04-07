#include <debug.h>

#include <arch.h>
#include <arch/loongarch64/debug.h>
#include <kernel/cmdline.h>
#include <object/resource_dispatcher.h>

#include <dev/display.h>
#include <dev/hw_rng.h>
#include <dev/uart.h>
#include <vm/physmap.h>

#include <platform.h>
#include <platform/timer.h>

#include <arch/loongarch64/periphmap.h>

#include <vm/bootreserve.h>

#include <lib/debuglog.h>
#include <lib/memory_limit.h>
#include <lib/system-topology.h>

#include <libzbi/zbi-cpp.h>
#include <pdev/pdev.h>
#include <zircon/boot/image.h>
#include <zircon/types.h>

// Defined in start.S.
extern paddr_t kernel_entry_paddr;
extern paddr_t zbi_paddr;

static void* ramdisk_base;
static size_t ramdisk_size;

static zbi_header_t* zbi_root = nullptr;

static zbi_nvram_t lastlog_nvram;

static bool halt_on_panic = false;
static bool uart_disabled = false;

// all of the configured memory arenas from the zbi
// at the moment, only support 1 arena
static pmm_arena_info_t mem_arena = {
    /* .name */ "sdram",
    /* .flags */ 0,
    /* .priority */ 0,
    /* .base */ 0, // filled in by zbi
    /* .size */ 0, // filled in by zbi
};

// boot items to save for mexec
// TODO(voydanoff): more generic way of doing this that can be shared with PC platform
static uint8_t mexec_zbi[4096];
static size_t mexec_zbi_length = 0;

static constexpr bool kProcessZbiEarly = true;

static inline bool is_zbi_container(void* addr) {
    DEBUG_ASSERT(addr);

    zbi_header_t* item = (zbi_header_t*)addr;
    return item->type == ZBI_TYPE_CONTAINER;
}

static void save_mexec_zbi(zbi_header_t* item) {
    size_t length = ZBI_ALIGN(
        static_cast<uint32_t>(sizeof(zbi_header_t) + item->length));
    ASSERT(sizeof(mexec_zbi) - mexec_zbi_length >= length);

    memcpy(&mexec_zbi[mexec_zbi_length], item, length);
    mexec_zbi_length += length;
}

static void process_mem_range(const zbi_mem_range_t* mem_range) {
    switch (mem_range->type) {
    case ZBI_MEM_RANGE_RAM:
        if (mem_arena.size == 0) {
            mem_arena.base = mem_range->paddr;
            mem_arena.size = mem_range->length;
            dprintf(INFO, "mem_arena.base %#" PRIx64 " size %#" PRIx64 "\n", mem_arena.base,
                    mem_arena.size);
        } else {
            if (mem_range->paddr) {
                mem_arena.base = mem_range->paddr;
                dprintf(INFO, "overriding mem arena 0 base from FDT: %#zx\n", mem_arena.base);
            }
            // if mem_area.base is already set, then just update the size
            mem_arena.size = mem_range->length;
            dprintf(INFO, "overriding mem arena 0 size from FDT: %#zx\n", mem_arena.size);
        }
        break;
    case ZBI_MEM_RANGE_PERIPHERAL: {
        auto status = add_periph_range(mem_range->paddr, mem_range->length);
        ASSERT(status == ZX_OK);
        break;
    }
    case ZBI_MEM_RANGE_RESERVED:
        dprintf(INFO, "boot reserve mem range: phys base %#" PRIx64 " length %#" PRIx64 "\n",
                mem_range->paddr, mem_range->length);
        boot_reserve_add_range(mem_range->paddr, mem_range->length);
        break;
    default:
        panic("bad mem_range->type in process_mem_range\n");
        break;
    }
}

// Called during platform_init_early, the heap is not yet present.
static zbi_result_t process_zbi_item_early(zbi_header_t* item,
                                           void* payload, void*) {
    if (ZBI_TYPE_DRV_METADATA(item->type)) {
        save_mexec_zbi(item);
        return ZBI_RESULT_OK;
    }

    switch (item->type) {
    case ZBI_TYPE_KERNEL_DRIVER:
    case ZBI_TYPE_PLATFORM_ID:
        // we don't process these here, but we need to save them for mexec
        save_mexec_zbi(item);
        break;
    case ZBI_TYPE_CMDLINE: {
        if (item->length < 1) {
            break;
        }
        char* contents = reinterpret_cast<char*>(payload);
        contents[item->length - 1] = '\0';
        cmdline_append(contents);
        break;
    }
    case ZBI_TYPE_MEM_CONFIG: {
        zbi_mem_range_t* mem_range = reinterpret_cast<zbi_mem_range_t*>(payload);
        uint32_t count = item->length / (uint32_t)sizeof(zbi_mem_range_t);
        for (uint32_t i = 0; i < count; i++) {
            process_mem_range(mem_range++);
        }
        save_mexec_zbi(item);
        break;
    }

    case ZBI_TYPE_NVRAM: {
        zbi_nvram_t* nvram = reinterpret_cast<zbi_nvram_t*>(payload);
        memcpy(&lastlog_nvram, nvram, sizeof(lastlog_nvram));
        dprintf(INFO, "boot reserve nvram range: phys base %#" PRIx64 " length %#" PRIx64 "\n",
                nvram->base, nvram->length);
        boot_reserve_add_range(nvram->base, nvram->length);
        save_mexec_zbi(item);
        break;
    }
    }

    return ZBI_RESULT_OK;
}

static constexpr zbi_topology_node_t fallback_topology = {
    .entity_type = ZBI_TOPOLOGY_ENTITY_PROCESSOR,
    .parent_index = ZBI_TOPOLOGY_NO_PARENT,
    .entity = {
        .processor = {
            .logical_ids = {0},
            .logical_id_count = 1,
            .flags = 0,
            .architecture = ZBI_TOPOLOGY_ARCH_LOONGARCH,
            .architecture_info = {
                .loongarch = zbi_topology_loongarch_info_t()
            }
        }
    }
};

static void init_topology(zbi_topology_node_t* nodes, size_t node_count) {
    auto result = system_topology::Graph::InitializeSystemTopology(nodes, node_count);
    if (result != ZX_OK) {
        printf("Failed to initialize system topology! error: %d\n", result);

        // Try to fallback to a topology of just this processor.
        result = system_topology::Graph::InitializeSystemTopology(&fallback_topology, 1);
        ASSERT(result == ZX_OK);
    }

    arch_set_num_cpus(static_cast<uint>(
        system_topology::GetSystemTopology().processor_count()));
}

// Called after heap is up, but before multithreading.
static zbi_result_t process_zbi_item_late(zbi_header_t* item,
                                          void* payload, void*) {
    switch (item->type) {
    case ZBI_TYPE_CPU_CONFIG: {
        zbi_cpu_config_t* cpu_config = reinterpret_cast<zbi_cpu_config_t*>(payload);

        // Convert old zbi_cpu_config into zbi_topology structure.

        // Allocate some memory to work in.
        size_t node_count = 0;
        for (size_t cluster = 0; cluster < cpu_config->cluster_count; cluster++) {
            // Each cluster will get a node.
            node_count++;
            node_count += cpu_config->clusters[cluster].cpu_count;
        }

        fbl::AllocChecker checker;
        auto flat_topology = ktl::unique_ptr<zbi_topology_node_t[]> {
            new (&checker) zbi_topology_node_t[node_count]};
        if (!checker.check()) {
            return ZBI_RESULT_ERROR;
        }

        // Initialize to 0.
        memset(flat_topology.get(), 0, sizeof(zbi_topology_node_t) * node_count);

        // Create topology structure.
        size_t flat_index = 0;
        uint16_t logical_id = 0;
        for (size_t cluster = 0; cluster < cpu_config->cluster_count; cluster++) {
            const auto cluster_index = flat_index;
            auto& node = flat_topology.get()[flat_index++];
            node.entity_type = ZBI_TOPOLOGY_ENTITY_CLUSTER;
            node.parent_index = ZBI_TOPOLOGY_NO_PARENT;

            // We don't have this data so it is a guess that little cores are
            // first.
            node.entity.cluster.performance_class = static_cast<uint8_t>(cluster);

            for (size_t i = 0; i < cpu_config->clusters[cluster].cpu_count; i++) {
                auto& node = flat_topology.get()[flat_index++];
                node.entity_type = ZBI_TOPOLOGY_ENTITY_PROCESSOR;
                node.parent_index = static_cast<uint16_t>(cluster_index);
                node.entity.processor.logical_id_count = 1;
                node.entity.processor.logical_ids[0] = logical_id;
                node.entity.processor.architecture = ZBI_TOPOLOGY_ARCH_LOONGARCH;
            }
        }
        DEBUG_ASSERT(flat_index == node_count);

        // Initialize topology subsystem.
        init_topology(flat_topology.get(), node_count);
        save_mexec_zbi(item);
        break;
    }
    case ZBI_TYPE_CPU_TOPOLOGY: {
        const int node_count = item->length / item->extra;

        zbi_topology_node_t* nodes =
            reinterpret_cast<zbi_topology_node_t*>(payload);

        init_topology(nodes, node_count);
        save_mexec_zbi(item);
        break;
    }
    }
    return ZBI_RESULT_OK;
}

static void process_zbi(zbi_header_t* root, bool early) {
    DEBUG_ASSERT(root);
    zbi_result_t result;

    uint8_t* zbi_base = reinterpret_cast<uint8_t*>(root);
    zbi::Zbi image(zbi_base);

    // Make sure the image looks valid.
    result = image.Check(nullptr);
    if (result != ZBI_RESULT_OK) {
        // TODO(gkalsi): Print something informative here?
        return;
    }

    image.ForEach(early ? process_zbi_item_early : process_zbi_item_late, nullptr);
}

void platform_early_init(void) {
    // if the zbi_paddr variable is -1, it was not set
    // in start.S, so we are in a bad place.
    if (zbi_paddr == -1UL) {
        panic("no zbi_paddr!\n");
    }

    void* zbi_vaddr = paddr_to_physmap(zbi_paddr & (ARCH_PHYSMAP_SIZE -1));

    // initialize the boot memory reservation system
    boot_reserve_init();

    if (zbi_vaddr && is_zbi_container(zbi_vaddr)) {
        zbi_header_t* header = (zbi_header_t*)zbi_vaddr;

        ramdisk_base = header;
        ramdisk_size = ROUNDUP(header->length + sizeof(*header), PAGE_SIZE);
    } else {
        panic("no bootdata!\n");
    }

    if (!ramdisk_base || !ramdisk_size) {
        panic("no ramdisk!\n");
    }

    zbi_root = reinterpret_cast<zbi_header_t*>(ramdisk_base);

    // walk the zbi structure and process all the items
    process_zbi(zbi_root, kProcessZbiEarly);

    // is the cmdline option to bypass dlog set ?
    dlog_bypass_init();

    // bring up kernel drivers after we have mapped our peripheral ranges
    pdev_init(zbi_root);

    // Serial port should be active now
    platform_dputs_thread("hello\n", 6);

    // Read cmdline after processing zbi, which may contain cmdline data.
    halt_on_panic = cmdline_get_bool("kernel.halt-on-panic", false);

    // Check if serial should be enabled
    const char* serial_mode = cmdline_get("kernel.serial");
    uart_disabled = (serial_mode != NULL && !strcmp(serial_mode, "none"));

    // add the ramdisk to the boot reserve memory list
    paddr_t ramdisk_start_phys = physmap_to_paddr(ramdisk_base);
    paddr_t ramdisk_end_phys = ramdisk_start_phys + ramdisk_size;
    dprintf(INFO, "reserving ramdisk phys range [%#" PRIx64 ", %#" PRIx64 "]\n",
            ramdisk_start_phys, ramdisk_end_phys - 1);
    boot_reserve_add_range(ramdisk_start_phys, ramdisk_size);

    // check if a memory limit was passed in via kernel.memory-limit-mb and
    // find memory ranges to use if one is found.
    zx_status_t status = memory_limit_init();
    if (status == ZX_OK) {
        // Figure out and add arenas based on the memory limit and our range of DRAM
        memory_limit_add_range(mem_arena.base, mem_arena.size, mem_arena);
        status = memory_limit_add_arenas(mem_arena);
    }

    // If no memory limit was found, or adding arenas from the range failed, then add
    // the existing global arena.
    if (status != ZX_OK) {
        // Init returns not supported if no limit exists
        if (status != ZX_ERR_NOT_SUPPORTED) {
            dprintf(INFO, "memory limit lib returned an error (%d), falling back to defaults\n",
                    status);
        }
        pmm_add_arena(&mem_arena);
    }

    // tell the boot allocator to mark ranges we've reserved as off limits
    boot_reserve_wire();
}

void platform_dputs_thread(const char* str, size_t len) {
    if (uart_disabled) {
        return;
    }
    uart_puts(str, len, true, true);
}

void platform_dputs_irq(const char* str, size_t len) {
    if (uart_disabled) {
        return;
    }
    uart_puts(str, len, false, true);
}

int platform_dgetc(char* c, bool wait) {
    if (uart_disabled) {
        return ZX_ERR_NOT_SUPPORTED;
    }
    int ret = uart_getc(wait);
    if (ret < 0)
        return ret;
    *c = static_cast<char>(ret);
        return 0;
}

size_t platform_recover_crashlog(size_t len, void* cookie,
                                 void (*func)(const void* data, size_t, size_t len, void* cookie)) {

    TODO();
    return 0;
}

size_t hw_rng_get_entropy(void* buf, size_t len, bool block) {
    TODO();
    return 0;
}

/* no built in framebuffer */
zx_status_t display_get_info(struct display_info* info) {
    TODO();
    return ZX_ERR_NOT_FOUND;
}

bool platform_serial_enabled(void) {
    TODO();
//  return !uart_disabled && uart_present();
    return true;
}

bool platform_early_console_enabled() {
  TODO();
  return false;
}
