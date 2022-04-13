// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#define HAS_DEVICE_TREE 1
#define USE_DEVICE_TREE_CPU_COUNT 1
#define USE_DEVICE_TREE_GIC_VERSION 0
#define PRINT_DEVICE_TREE 0

#define MAX_CPU_COUNT 1 // Only uniprocessor supported now
static size_t cpu_count = 0;

static const zbi_mem_range_t mem_config[] = {
    {
        .type = ZBI_MEM_RANGE_RAM,
        .paddr = 0x90000000UL,
        .length = 0x40000000UL,
    },
    {
        .type = ZBI_MEM_RANGE_PERIPHERAL,
        .paddr = 0,
        .length = 0x100000000UL, // DMA range, can we use it?
    },
};

static const dcfg_simple_t uart_driver = {
    .mmio_phys = 0x1fe001e0,
    .irq = 66, // TODO: irq number
};

static const dcfg_loongarch_extioi_pic_driver_t extioi_pic_driver = {
};

static const dcfg_loongarch_generic_timer_driver_t timer_driver = {
    .irq_phys = 27, // TODO: irq number
};

static const zbi_platform_id_t platform_id = {
    .vid = PDEV_VID_QEMU,
    .pid = PDEV_PID_QEMU,
    .board_name = "ls7a",
};

static void add_cpu_topology(zbi_header_t* zbi) {
   zbi_topology_node_t nodes[MAX_CPU_COUNT];

   // clamp to the max cpu
   if (cpu_count > MAX_CPU_COUNT) {
       cpu_count = MAX_CPU_COUNT;
   }

   for (size_t index = 0; index < cpu_count; index++) {
       nodes[index] = (zbi_topology_node_t){
           .entity_type = ZBI_TOPOLOGY_ENTITY_PROCESSOR,
           .parent_index = ZBI_TOPOLOGY_NO_PARENT,
           .entity = {
               .processor = {
                   .logical_ids = {index},
                   .logical_id_count = 1,
                   .flags = (index == 0) ? ZBI_TOPOLOGY_PROCESSOR_PRIMARY : 0,
                   .architecture = ZBI_TOPOLOGY_ARCH_LOONGARCH,
                   .architecture_info = {
                       .loongarch = {}}}}};
   }

   append_boot_item(zbi, ZBI_TYPE_CPU_TOPOLOGY,
                    sizeof(zbi_topology_node_t), // Extra
                    &nodes, sizeof(zbi_topology_node_t) * cpu_count);
}

static void append_board_boot_item(zbi_header_t* bootdata) {
   add_cpu_topology(bootdata);

   // add memory configuration
   append_boot_item(bootdata, ZBI_TYPE_MEM_CONFIG, 0, &mem_config,
                    sizeof(zbi_mem_range_t) * countof(mem_config));

   // add kernel drivers
   append_boot_item(bootdata, ZBI_TYPE_KERNEL_DRIVER, KDRV_DW8250_UART, &uart_driver,
                    sizeof(uart_driver));

   // append the gic information from the specific gic version we detected from the
   // device tree.
    append_boot_item(bootdata, ZBI_TYPE_KERNEL_DRIVER, KDRV_LOONGARCH_EXTIOI_PIC, &extioi_pic_driver,
                    sizeof(extioi_pic_driver));

//    append_boot_item(bootdata, ZBI_TYPE_KERNEL_DRIVER, KDRV_ARM_PSCI, &psci_driver,
//                     sizeof(psci_driver));
    append_boot_item(bootdata, ZBI_TYPE_KERNEL_DRIVER, KDRV_LOONGARCH_GENERIC_TIMER, &timer_driver,
                    sizeof(timer_driver));

   // add platform ID
   append_boot_item(bootdata, ZBI_TYPE_PLATFORM_ID, 0, &platform_id, sizeof(platform_id));
}

static void set_cpu_count(uint32_t new_count) {
    if (new_count > 0) {
        cpu_count = new_count;
    }
}
