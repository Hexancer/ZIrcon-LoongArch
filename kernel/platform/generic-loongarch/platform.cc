#include <debug.h>
#include <zircon/types.h>

#include <dev/display.h>
#include <dev/hw_rng.h>

#include <platform.h>
#include <platform/timer.h>

zx_status_t platform_set_oneshot_timer(zx_time_t deadline) {
//  DEBUG_ASSERT(arch_ints_disabled());
//
//  if (deadline < 0) {
//    deadline = 0;
//  }
//
//  // Add one to the deadline, since with very high probability the deadline
//  // straddles a counter tick.
//  const uint64_t cntpct_deadline = zx_time_to_cntpct(deadline) + 1;
//
//  // Even if the deadline has already passed, the ARMv8-A timer will fire the
//  // interrupt.
//  write_cval(cntpct_deadline);
//  write_ctl(1);

  return 0;
}

void platform_stop_timer(void) {
//  write_ctl(0);
}

void platform_dputs_thread(const char* str, size_t len) {
//     if (uart_disabled) {
//         return;
//     }
//     uart_puts(str, len, true, true);
}

void platform_dputs_irq(const char* str, size_t len) {
//  if (uart_disabled) {
//    return;
//  }
//  uart_puts(str, len, false, true);
}

int platform_dgetc(char* c, bool wait) {
//  if (uart_disabled) {
//    return ZX_ERR_NOT_SUPPORTED;
//  }
//  int ret = uart_getc(wait);
//  if (ret < 0)
//    return ret;
//  *c = static_cast<char>(ret);
  return 0;
}
size_t platform_recover_crashlog(size_t len, void* cookie,
                                 void (*func)(const void* data, size_t, size_t len, void* cookie)) {

    return 0;
}

size_t hw_rng_get_entropy(void* buf, size_t len, bool block) {
  return 0;
}

/* no built in framebuffer */
zx_status_t display_get_info(struct display_info* info) {
  return ZX_ERR_NOT_FOUND;
}

bool platform_serial_enabled(void) {
//  return !uart_disabled && uart_present();
  return true;
}

bool platform_early_console_enabled() {
  return false;
}
