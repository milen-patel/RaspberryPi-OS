#include <stddef.h>
#include <stdint.h>
#include <uart.h>

void main(uint32_t r0, uint32_t r1, uint32_t atags)
{
    (void) r0;
    (void) r1;
    (void) atags;

    uart_init();
    uart_puts("Hello, kernel World!\r\n");
    uart_puts("The current processor id is: \r\n");
    const char *x = (const char *) &r0;
    uart_puts(x);
    uart_puts("Done\r\n");
    if ((0b1111 & r0) == 5) {
        uart_puts("Is 5\r\n");
    } else {
        uart_puts("Not is not\r\n");
    }

    while (1) {
        uart_putc('a');
        uart_putc('\n');
    }
}
