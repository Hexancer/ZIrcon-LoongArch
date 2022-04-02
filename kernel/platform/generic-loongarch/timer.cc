#include <arch.h>

#include <sys/types.h>
#include <zircon/types.h>

#include <platform.h>

zx_ticks_t ticks_per_second(void) {
    TODO();
    return 1000000;
}