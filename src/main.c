#include "uart.h"
#include "libk/printf.h"
#include "get-el.h"
#include "interrupts/interrupt.h"
#include "timer/timer.h"
#include "interrupts/toggle.h"
#include "mmio.h"
#include "sys-registers/timer.h"
#include "paging/alloc.h"
#include "process/pcb.h"
#include <stdbool.h>
#include <stdint.h>

// Once boot.S finishes setting up the C runtime environment, it will defer execution to kmain
bool shouldSecondCPUStart = false;
void kmain2() {
  while (!shouldSecondCPUStart);
  //char c = 'x';
  //printf("CPU 2 Kernel Stack Local Variable Address: %p\n", &c);
}

void simple_spin_function() {
  while (1) {
    kprintf("SPIN\n");
  }
}

void kmain(void) {
  uart_init();
  kprintf("===============================================================================\n");
  kprintf("[time = %d] UART has been initialized...\n", get32(TIMER_CLO));
  kprintf("[time = %d] Current Kernel Exception Level: %d\n", get32(TIMER_CLO), getExceptionLevel());

  init_interrupt_request_table();
  kprintf("[time = %d] Interrupt Request Table has been set up\n", get32(TIMER_CLO));

  init_timer();
  kprintf("[time = %d] Raspberry Pi Hardware Timer Has been set up\n", get32(TIMER_CLO));

  init_rpi_interrupt_handler();
  kprintf("[time = %d] Raspberry Pi interrupt controller has been configured \n", get32(TIMER_CLO));

  enable_irq();   
  kprintf("[time = %d] Interrupt Requests Have Been Enabled\n", get32(TIMER_CLO));

  init_paging();
  kprintf("[time = %d] Paging has been initialized\n", get32(TIMER_CLO));

  init_scheduler();
  kprintf("[time = %d] Scheduler has been initialized\n", get32(TIMER_CLO));
  kprintf("===============================================================================\n");

  //shouldSecondCPUStart = true;
  //delay(10000);

  new_kernel_thread(simple_spin_function, "A");
  new_kernel_thread(simple_spin_function, "B");

  while (1) {
    kprintf("[time=%d] c=%c\n", get32(TIMER_CLO), uart_recv());
	}
}
