#include "uart.h"

void kmain(void) {
    uart_init();
    uart_send('c');
    uart_send_string("Hello World");

    while (1) {
		uart_send(uart_recv());
	}
}