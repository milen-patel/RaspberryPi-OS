#include "uart.h"
#include "printf.h"
#include "get-el.h"
#include "interrupts/interrupt.h"
#include "timer/timer.h"
#include "interrupts/toggle.h"
#include "mmio.h"
#include "sys-registers/timer.h"
#include "paging/alloc.h"
#include <stdbool.h>
#include <stdint.h>

// Once boot.S finishes setting up the C runtime environment, it will defer execution to kmain
bool shouldSecondCPUStart = false;
void kmain2() {
  while (!shouldSecondCPUStart);
  //char c = 'x';
  //printf("CPU 2 Kernel Stack Local Variable Address: %p\n", &c);
}

void kmain(void) {
  uart_init();
  printf("===============================================================================\n");
  printf("[time = %d] UART has been initialized...\n", get32(TIMER_CLO));
  printf("[time = %d] Current Kernel Exception Level: %d\n", get32(TIMER_CLO), getExceptionLevel());

  init_interrupt_request_table();
  printf("[time = %d] Interrupt Request Table has been set up\n", get32(TIMER_CLO));

  init_timer();
  printf("[time = %d] Raspberry Pi Hardware Timer Has been set up\n", get32(TIMER_CLO));

  init_rpi_interrupt_handler();
  printf("[time = %d] Raspberry Pi interrupt controller has been configured \n", get32(TIMER_CLO));

  enable_irq();   
  printf("[time = %d] Interrupt Requests Have Been Enabled\n", get32(TIMER_CLO));

  init_paging();
  printf("[time = %d] Paging has been initialized\n", get32(TIMER_CLO));
  printf("===============================================================================\n");

  shouldSecondCPUStart = true;
  delay(10000);

  void *a = alloc_page();
  void *b = alloc_page();
  void *c = alloc_page();
  free_page(b);
  b = alloc_page();
  free_page(a);
  free_page(b);
  free_page(c);
  while (1) {
    char c = uart_recv();
    printf("[time=%d]%c\n", get32(TIMER_CLO), c);
	}
}
