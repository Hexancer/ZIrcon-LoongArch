#include <dev/timer/loongarch_generic.h>

#include <debug.h>
#include <platform.h>
#include <platform/timer.h>
#include <zircon/types.h>

zx_time_t current_time(void) {
  return 0;
}
zx_ticks_t current_ticks(void) {
  return 0;
}