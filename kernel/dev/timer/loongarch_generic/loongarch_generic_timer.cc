#include <dev/timer/loongarch_generic.h>

#include <arch.h>
#include <arch/loongarch64/csr.h>
#include <assert.h>
#include <debug.h>
#include <dev/interrupt.h>
#include <inttypes.h>
#include <lib/fixed_point.h>
#include <pdev/driver.h>
#include <platform.h>
#include <platform/timer.h>
#include <trace.h>
#include <zircon/boot/driver-config.h>
#include <zircon/types.h>

#define LOCAL_TRACE 0

static int timer_irq;

struct fp_32_64 cntpct_per_ns;
struct fp_32_64 ns_per_cntpct;

static uint64_t zx_time_to_cntpct(zx_time_t zx_time) {
    DEBUG_ASSERT(zx_time >= 0);
    return u64_mul_u64_fp32_64(zx_time, cntpct_per_ns);
}

zx_time_t cntpct_to_zx_time(uint64_t cntpct) {
    return u64_mul_u64_fp32_64(cntpct, ns_per_cntpct);
}

static uint32_t read_cntfrq(void) {
	uint32_t res;
	uint32_t base_freq;
	uint32_t cfm, cfd;

	res = read_cpucfg(LOONGARCH_CPUCFG2);
	if (!(res & CPUCFG2_LLFTP))
		return 0;

	base_freq = read_cpucfg(LOONGARCH_CPUCFG4);
	res = read_cpucfg(LOONGARCH_CPUCFG5);
	cfm = res & 0xffff;
	cfd = (res >> 16) & 0xffff;

	if (!base_freq || !cfm || !cfd)
		return 0;
	else
		return (base_freq * cfm / cfd);
}

static inline void write_ctl(uint32_t val) {
    uint64_t timer_config;
	timer_config = csr_readq(LOONGARCH_CSR_TCFG);
    timer_config = (timer_config & ~CSR_TCFG_EN) | (val & CSR_TCFG_EN);
	csr_writeq(timer_config, LOONGARCH_CSR_TCFG);
}

static inline void write_cval(uint64_t val) {
    uint64_t timer_config;
	timer_config = csr_readq(LOONGARCH_CSR_TCFG);
    // Clear least 2 bits
    timer_config = (timer_config & ~CSR_TCFG_VAL) | (val & CSR_TCFG_VAL);
	csr_writeq(timer_config, LOONGARCH_CSR_TCFG);
}

static inline void write_tval(uint32_t val) {
    // TODO: We don't have both tval and cval
    TODO();
}

static uint64_t read_ct(void) {
    uint64_t cntpct = drdtime();
    LTRACEF_LEVEL(3, "cntpct: 0x%016" PRIx64 ", %" PRIu64 "\n",
                  cntpct, cntpct);
    return cntpct;
}

static interrupt_eoi platform_tick(void* arg) {
    TODO();
    // write_ctl(0);
    // timer_tick(current_time());
    // return IRQ_EOI_DEACTIVATE;
}

zx_status_t platform_set_oneshot_timer(zx_time_t deadline) {
     DEBUG_ASSERT(arch_ints_disabled());

    if (deadline < 0) {
        deadline = 0;
    }

    // Add one to the deadline, since with very high probability the deadline
    // straddles a counter tick.
    const uint64_t cntpct_now = current_ticks();
    const uint64_t cntpct_deadline = zx_time_to_cntpct(deadline) + 1;

    uint64_t cntpct_delta = cntpct_deadline - cntpct_now;

    write_cval(cntpct_delta);
    write_ctl(CSR_TCFG_EN);

    return 0;
}

void platform_stop_timer(void) {
    write_ctl(0);
}

zx_time_t current_time(void) {
    return cntpct_to_zx_time(current_ticks());
}

zx_ticks_t current_ticks(void) {
    return read_ct();
}

zx_ticks_t ticks_per_second(void) {
    return u64_mul_u32_fp32_64(1000 * 1000 * 1000, cntpct_per_ns);
}

static uint64_t abs_int64(int64_t a) {
    return (a > 0) ? a : -a;
}

static void test_time_conversion_check_result(uint64_t a, uint64_t b, uint64_t limit) {
    if (a != b) {
        uint64_t diff = abs_int64(a - b);
        if (diff <= limit)
            LTRACEF("ROUNDED by %" PRIu64 " (up to %" PRIu64 " allowed)\n", diff, limit);
        else
            TRACEF("FAIL, off by %" PRIu64 "\n", diff);
    }
}

static void test_zx_time_to_cntpct(uint32_t cntfrq, zx_time_t zx_time) {
    uint64_t cntpct = zx_time_to_cntpct(zx_time);
    const uint64_t nanos_per_sec = ZX_SEC(1);
    uint64_t expected_cntpct = ((uint64_t)cntfrq * zx_time + nanos_per_sec / 2) / nanos_per_sec;

    test_time_conversion_check_result(cntpct, expected_cntpct, 1);
    LTRACEF_LEVEL(2, "zx_time_to_cntpct(%" PRIi64 "): got %" PRIu64
                     ", expect %" PRIu64 "\n",
                  zx_time, cntpct, expected_cntpct);
}

static void test_cntpct_to_zx_time(uint32_t cntfrq, uint64_t expected_s) {
    zx_time_t expected_zx_time = ZX_SEC(expected_s);
    uint64_t cntpct = (uint64_t)cntfrq * expected_s;
    zx_time_t zx_time = cntpct_to_zx_time(cntpct);

    test_time_conversion_check_result(zx_time, expected_zx_time, (1000 * 1000 + cntfrq - 1) / cntfrq);
    LTRACEF_LEVEL(2, "cntpct_to_zx_time(%" PRIu64
                     "): got %" PRIi64 ", expect %" PRIi64 "\n",
                  cntpct, zx_time, expected_zx_time);
}

static void test_time_conversions(uint32_t cntfrq) {
    test_zx_time_to_cntpct(cntfrq, 0);
    test_zx_time_to_cntpct(cntfrq, 1);
    test_zx_time_to_cntpct(cntfrq, 60 * 60 * 24);
    test_zx_time_to_cntpct(cntfrq, 60 * 60 * 24 * 365);
    test_zx_time_to_cntpct(cntfrq, 60 * 60 * 24 * (365 * 10 + 2));
    test_zx_time_to_cntpct(cntfrq, 60ULL * 60 * 24 * (365 * 100 + 2));
    test_zx_time_to_cntpct(cntfrq, 1ULL << 60);
    test_cntpct_to_zx_time(cntfrq, 0);
    test_cntpct_to_zx_time(cntfrq, 1);
    test_cntpct_to_zx_time(cntfrq, 60 * 60 * 24);
    test_cntpct_to_zx_time(cntfrq, 60 * 60 * 24 * 365);
    test_cntpct_to_zx_time(cntfrq, 60 * 60 * 24 * (365 * 10 + 2));
    test_cntpct_to_zx_time(cntfrq, 60ULL * 60 * 24 * (365 * 100 + 2));
}

static void loongarch_generic_timer_init_conversion_factors(uint32_t cntfrq) {
    fp_32_64_div_32_32(&cntpct_per_ns, cntfrq, ZX_SEC(1));
    fp_32_64_div_32_32(&ns_per_cntpct, ZX_SEC(1), cntfrq);
    dprintf(SPEW, "cntpct_per_ns: %08x.%08x%08x\n", cntpct_per_ns.l0, cntpct_per_ns.l32, cntpct_per_ns.l64);
    dprintf(SPEW, "ns_per_cntpct: %08x.%08x%08x\n", ns_per_cntpct.l0, ns_per_cntpct.l32, ns_per_cntpct.l64);
}

static void loongarch_generic_timer_init(uint32_t freq_override) {
    uint32_t cntfrq;

    if (freq_override == 0) {
        cntfrq = read_cntfrq();

        if (!cntfrq) {
            TRACEF("Failed to initialize timer, frequency is 0\n");
            return;
        }
    } else {
        cntfrq = freq_override;
    }

    dprintf(INFO, "loongarch generic timer freq %u Hz\n", cntfrq);

#if LOCAL_TRACE
    LTRACEF("Test min cntfrq\n");
    loongarch_generic_timer_init_conversion_factors(1);
    test_time_conversions(1);
    LTRACEF("Test max cntfrq\n");
    loongarch_generic_timer_init_conversion_factors(~0);
    test_time_conversions(~0);
    LTRACEF("Set actual cntfrq\n");
#endif
    loongarch_generic_timer_init_conversion_factors(cntfrq);
    test_time_conversions(cntfrq);

    LTRACEF("register irq %d on cpu %u\n", timer_irq, arch_curr_cpu_num());
    zx_status_t status = register_int_handler(timer_irq, &platform_tick, NULL);
    DEBUG_ASSERT(status == ZX_OK);
    unmask_interrupt(timer_irq);
}


static void loongarch_generic_timer_pdev_init(const void* driver_data, uint32_t length) {
    // ASSERT(length >= sizeof(dcfg_loongarch_generic_timer_driver_t));
    // auto driver = static_cast<const dcfg_loongarch_generic_timer_driver_t*>(driver_data);

    // TODO: setup irq

    loongarch_generic_timer_init(0);
}

LK_PDEV_INIT(loongarch_generic_timer_pdev_init, KDRV_LOONGARCH_GENERIC_TIMER, loongarch_generic_timer_pdev_init, LK_INIT_LEVEL_PLATFORM_EARLY)
