#include <arch/loongarch64/debug.h>

#include <stdarg.h>
#include <stdio.h>
#include <zircon/types.h>

static volatile uint8_t* uart_reg_dat = (uint8_t *)(0x800000001fe001e0UL);          // Data register 0x1fe001e0
static void uart_pputc(char c)
{
    *uart_reg_dat = c;//sending data
}


static void uart_puts(const char* str) {
    char ch;
    while ((ch = *str++)) {
        if (ch == '\n')
            uart_pputc('\r');
        uart_pputc(ch);
    }
}

static int uart_putsn(const char *str, size_t len, void *state) {
    size_t n;
    char ch;
    while ((ch = *str++) && (n < len)) {
        if (ch == '\n')
            uart_pputc('\r');
        uart_pputc(ch);
        n++;
    }
    return (int)n;
}

static void uart_print_hex(uint64_t value) {
    const char digits[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                             '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    for (int i = 60; i >= 0; i -= 4) {
        uart_pputc(digits[(value >> i) & 0xf]);
    }
}

int laprintf(const char *fmt, ...)
{
    va_list ap;
    int err;

    va_start(ap, fmt);
    err = _printf_engine(uart_putsn, NULL, fmt, ap);
    va_end(ap);

    return err;
}
