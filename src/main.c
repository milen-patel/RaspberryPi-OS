#include "uart.h"
#include "printf.h"
#include "get-el.h"

// Once boot.S finishes setting up the C runtime environment, it will defer execution to kmain
void kmain(void) {
  uart_init();
  printf("\n\nKernel Has Loaded...\n");
  printf("Current Kernel Exception Level: %d\n", getExceptionLevel());

  while (1) {
    printf("%c", uart_recv());
	}
}