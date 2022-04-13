#include <arch/loongarch64/debug.h>
#include <dev/interrupt.h>
#include <pdev/driver.h>
#include <pdev/interrupt.h>
#include <lk/init.h>
#include <zircon/boot/driver-config.h>

static const struct pdev_interrupt_ops gic_ops = {
    .mask = NULL, //gic_mask_interrupt,
    .unmask = NULL, //gic_unmask_interrupt,
    .configure = NULL, //gic_configure_interrupt,
    .get_config = NULL, //gic_get_interrupt_config,
    .is_valid = NULL, //gic_is_valid_interrupt,
    .get_base_vector = NULL, //gic_get_base_vector,
    .get_max_vector = NULL, //gic_get_max_vector,
    .remap = NULL, //gic_remap_interrupt,
    .send_ipi = NULL, //gic_send_ipi,
    .init_percpu_early = NULL, //gic_init_percpu_early,
    .init_percpu = NULL, //gic_init_percpu,
    .handle_irq = NULL, //gic_handle_irq,
    .handle_fiq = NULL, //gic_handle_fiq,
    .shutdown = NULL, //gic_shutdown,
    .shutdown_cpu = NULL, //gic_shutdown_cpu,
    .msi_is_supported = NULL, //arm_gicv2m_msi_is_supported,
    .msi_supports_masking = NULL, //arm_gicv2m_msi_supports_masking,
    .msi_mask_unmask = NULL, //arm_gicv2m_msi_mask_unmask,
    .msi_alloc_block = NULL, //arm_gicv2m_msi_alloc_block,
    .msi_free_block = NULL, //arm_gicv2m_msi_free_block,
    .msi_register_handler = NULL, //arm_gicv2m_msi_register_handler,
};

static void extioi_pic_init(const void* driver_data, uint32_t length) {
    TODO();
    // ASSERT(length >= sizeof(dcfg_arm_gicv3_driver_t));
    // auto driver = static_cast<const dcfg_arm_gicv3_driver_t*>(driver_data);
    // ASSERT(driver->mmio_phys);

    // LTRACE_ENTRY;

    // // If a GIC driver is already registered to the GIC interface it's means we are running GICv2
    // // and we do not need to initialize GICv3. Since we have added both GICv3 and GICv2 in board.mdi,
    // // both drivers are initialized
    // if (arm_gic_is_registered()) {
    //     return;
    // }

    // if (driver->mx8_gpr_phys) {
    //     printf("arm-gic-v3: Applying Errata e11171 for NXP MX8!\n");
    //     mx8_gpr_virt = periph_paddr_to_vaddr(driver->mx8_gpr_phys);
    //     ASSERT(mx8_gpr_virt);
    // }

    // arm_gicv3_gic_base = periph_paddr_to_vaddr(driver->mmio_phys);
    // ASSERT(arm_gicv3_gic_base);
    // arm_gicv3_gicd_offset = driver->gicd_offset;
    // arm_gicv3_gicr_offset = driver->gicr_offset;
    // arm_gicv3_gicr_stride = driver->gicr_stride;
    // ipi_base = driver->ipi_base;

    // arm_gicv3_gic_base = periph_paddr_to_vaddr(driver->mmio_phys);
    // ASSERT(arm_gicv3_gic_base);

    // if (gic_init() != ZX_OK) {
    //     if (driver->optional) {
    //         // failed to detect gic v3 but it's marked optional. continue
    //         return;
    //     }
    //     printf("GICv3: failed to detect GICv3, interrupts will be broken\n");
    //     return;
    // }

    // dprintf(SPEW, "detected GICv3\n");

    // pdev_register_interrupts(&gic_ops);

    // zx_status_t status =
    //     gic_register_sgi_handler(MP_IPI_GENERIC + ipi_base, &mp_mbx_generic_irq);
    // DEBUG_ASSERT(status == ZX_OK);
    // status =
    //     gic_register_sgi_handler(MP_IPI_RESCHEDULE + ipi_base, &mp_mbx_reschedule_irq);
    // DEBUG_ASSERT(status == ZX_OK);
    // status = gic_register_sgi_handler(MP_IPI_INTERRUPT + ipi_base, &mp_mbx_interrupt_irq);
    // DEBUG_ASSERT(status == ZX_OK);
    // status = gic_register_sgi_handler(MP_IPI_HALT + ipi_base, &arm_ipi_halt_handler);
    // DEBUG_ASSERT(status == ZX_OK);

    // gicv3_hw_interface_register();

    // LTRACE_EXIT;
}

LK_PDEV_INIT(extioi_pic_init, KDRV_LOONGARCH_EXTIOI_PIC, extioi_pic_init, LK_INIT_LEVEL_PLATFORM_EARLY)
