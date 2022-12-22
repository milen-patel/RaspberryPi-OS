#include "uart.h"

// Once boot.S finishes setting up the C runtime environment, it will defer execution to kmain
void kmain(void) {
  // Initialize the UART and print a string to make sure we are working
  uart_init();
  uart_send('X');
  uart_send_string("Hello World");

  while (1) {
		uart_send(uart_recv());
	}
}