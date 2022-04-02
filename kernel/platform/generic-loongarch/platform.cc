#include <debug.h>
#include <zircon/types.h>




void platform_dputs_thread(const char* str, size_t len) {
//     if (uart_disabled) {
//         return;
//     }
//     uart_puts(str, len, true, true);
}

size_t platform_recover_crashlog(size_t len, void* cookie,
                                 void (*func)(const void* data, size_t, size_t len, void* cookie)) {

    return 0;
}