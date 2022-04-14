#include <arch/loongarch64/debug.h>
#include <dev/interrupt.h>
#include <lib/ktrace.h>
#include <lk/init.h>
#include <pdev/driver.h>
#include <pdev/interrupt.h>
#include <trace.h>
#include <zircon/boot/driver-config.h>

#define LOCAL_TRACE 1

#define LOONGARCH_MAX_INTERRUPT 13

static bool pic_is_valid_interrupt(unsigned int vector, uint32_t flags) {
    return vector < LOONGARCH_MAX_INTERRUPT;
}

static uint32_t pic_get_base_vector() {
    TODO();
    return 0;
}

static uint32_t pic_get_max_vector() {
    TODO();
    return 0;
}

static void pic_init_percpu_early() {
    TODO();
}

static zx_status_t pic_mask_interrupt(unsigned int vector) {
    csr_xchgl(0, 1 << vector, LOONGARCH_CSR_ECFG);
    return ZX_OK;
}

static zx_status_t pic_unmask_interrupt(unsigned int vector) {
    csr_xchgl(1 << vector, 1 << vector, LOONGARCH_CSR_ECFG);
    return ZX_OK;
}

static zx_status_t pic_configure_interrupt(unsigned int vector,
                                           enum interrupt_trigger_mode tm,
                                           enum interrupt_polarity pol) {
    TODO();
    return ZX_OK;
}

static zx_status_t pic_get_interrupt_config(unsigned int vector,
                                            enum interrupt_trigger_mode* tm,
                                            enum interrupt_polarity* pol) {
    TODO();
    return ZX_OK;
}

static unsigned int pic_remap_interrupt(unsigned int vector) {
    TODO();
    return vector;
}

// called from assembly
static void pic_handle_irq(iframe_short_t* frame) {
    // // get the current vector
    // uint32_t iar = gic_read_iar();
    unsigned vector = 0;

    if (frame->csr[LOONGARCH_CSR_ESTAT] & CSR_ESTAT_IS_TIMER) {
        vector = CSR_ESTAT_IS_TIMER_SHIFT; // Timer interrupt
    } else if (frame->csr[LOONGARCH_CSR_ESTAT] & CSR_ESTAT_IS_IPI) {
        vector = CSR_ESTAT_IS_IPI_SHIFT;// Inter-processor interrupt
    }

    LTRACEF_LEVEL(2, "vector %u\n", vector);

    // if (vector >= 0x3fe) {
    //     // spurious
    //     // TODO check this
    //     return;
    // }

    // // tracking external hardware irqs in this variable
    // if (vector >= 32) {
    //     CPU_STATS_INC(interrupts);
    // }

    uint cpu = arch_curr_cpu_num();

    ktrace_tiny(TAG_IRQ_ENTER, (vector << 8) | cpu);

    // LTRACEF_LEVEL(2, "iar 0x%x cpu %u currthread %p vector %u pc %#" PRIxPTR "\n",
    //               iar, cpu, get_current_thread(), vector, (uintptr_t)IFRAME_PC(frame));

    // deliver the interrupt
    struct int_handler_struct* handler = pdev_get_int_handler(vector);
    interrupt_eoi eoi = IRQ_EOI_DEACTIVATE;
    if (handler->handler) {
        eoi = handler->handler(handler->arg);
    }
    // TODO: deactivate interrupt
    // gic_write_eoir(vector);
    if (eoi == IRQ_EOI_DEACTIVATE) {
    //     gic_write_dir(vector);
    }

    LTRACEF_LEVEL(2, "cpu %u exit\n", cpu);

    ktrace_tiny(TAG_IRQ_EXIT, (vector << 8) | cpu);
}

static void pic_handle_fiq(iframe_short_t* frame) {
    PANIC_UNIMPLEMENTED;
}

static zx_status_t pic_send_ipi(cpu_mask_t target, mp_ipi_t ipi) {
    TODO();
    return ZX_OK;
}

static void pic_init_percpu() {
    // TODO: support SMP & IPI
    // mp_set_curr_cpu_online(true);
    // unmask_interrupt(MP_IPI_GENERIC + ipi_base);
    // unmask_interrupt(MP_IPI_RESCHEDULE + ipi_base);
    // unmask_interrupt(MP_IPI_INTERRUPT + ipi_base);
    // unmask_interrupt(MP_IPI_HALT + ipi_base);
}

static void pic_shutdown() {
    TODO();
}

static void pic_shutdown_cpu() {
    TODO();
}

static bool pic_msi_is_supported() {
    return false;
}

static bool pic_msi_supports_masking() {
    return false;
}

static void pic_msi_mask_unmask(const msi_block_t* block, uint msi_id, bool mask) {
    PANIC_UNIMPLEMENTED;
}

static zx_status_t pic_msi_alloc_block(uint requested_irqs,
                                       bool can_target_64bit,
                                       bool is_msix,
                                       msi_block_t* out_block) {
    PANIC_UNIMPLEMENTED;
}

static void pic_msi_free_block(msi_block_t* block) {
    PANIC_UNIMPLEMENTED;
}

static void pic_msi_register_handler(const msi_block_t* block,
                                     uint msi_id,
                                     int_handler handler,
                                     void* ctx) {
    PANIC_UNIMPLEMENTED;
}


static const struct pdev_interrupt_ops pic_ops = {
    .mask = pic_mask_interrupt,
    .unmask = pic_unmask_interrupt,
    .configure = pic_configure_interrupt,
    .get_config = pic_get_interrupt_config,
    .is_valid = pic_is_valid_interrupt,
    .get_base_vector = pic_get_base_vector,
    .get_max_vector = pic_get_max_vector,
    .remap = pic_remap_interrupt,
    .send_ipi = pic_send_ipi,
    .init_percpu_early = pic_init_percpu_early,
    .init_percpu = pic_init_percpu,
    .handle_irq = pic_handle_irq,
    .handle_fiq = pic_handle_fiq,
    .shutdown = pic_shutdown,
    .shutdown_cpu = pic_shutdown_cpu,
    .msi_is_supported = pic_msi_is_supported,
    .msi_supports_masking = pic_msi_supports_masking,
    .msi_mask_unmask = pic_msi_mask_unmask,
    .msi_alloc_block = pic_msi_alloc_block,
    .msi_free_block = pic_msi_free_block,
    .msi_register_handler = pic_msi_register_handler,
};

static void extioi_pic_init(const void* driver_data, uint32_t length) {
    LTRACE_ENTRY;

    dprintf(SPEW, "detected EXTIOI PIC\n");

    pdev_register_interrupts(&pic_ops);

    LTRACE_EXIT;
}

LK_PDEV_INIT(extioi_pic_init, KDRV_LOONGARCH_EXTIOI_PIC, extioi_pic_init, LK_INIT_LEVEL_PLATFORM_EARLY)
