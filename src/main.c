#include "uart.h"
#include "printf.h"

// Once boot.S finishes setting up the C runtime environment, it will defer execution to kmain
static int t = 234;
static int y;
void kmain(void) {
  uart_init();
  int temp = 0;
  printf("Address of kernel stack: %p\n", &temp);
  printf("Address of t: %p\n", &t);
  printf("Address of y: %p\n", &y);
  // Initialize the UART and print a string to make sure we are working
  uart_send_string("Hello World");
  test_printf();

  while (1) {
    printf("%c", uart_recv());
	}
}