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

extern char kernel_page_table_start;
void spinAndInc (void * arg) {
  char * prefix = (char *) arg;
  int i = 0;
  kprintf("\nSP=~%p\n", &i);
  while (1) {
    i++;
    kprintf("%s%d\n", prefix, i);
  }
}

void userFunction(char *arg) {
  while (1) kprintf("%sIn user function\n", arg);
}

void kmain(void) {
  uart_init();
  kprintf("===============================================================================\n");
  kprintf("[time = %d] UART has been initialized...\n", get32(TIMER_CLO));
  kprintf("[time = %d] Current Kernel Exception Level: %d\n", get32(TIMER_CLO), getExceptionLevel());

  // init_interrupt_request_table();
  // kprintf("[time = %d] Interrupt Request Table has been set up\n", get32(TIMER_CLO));

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
  
  kprintf("[time = %d] The kernel is operating in exception level %d\n", get32(TIMER_CLO), getExceptionLevel());
  kprintf("===============================================================================\n");

  //shouldSecondCPUStart = true;
  //delay(10000);
  kprintf("Address of kernel pt: %p\n", &kernel_page_table_start);
  kprintf("This address should point to the base of the PGD table\n");
  unsigned long *pgd = (unsigned long *) &kernel_page_table_start;
  kprintf("PGD Table Dump:\n");
  for (int i = 0; i < 512; i++) {
    unsigned long curr = *(pgd + i);
    kprintf("[%d] %d\n", i, curr);
  }

  pgd += 512;
  kprintf("PUD Table Dump:\n");
  for (int i = 0; i < 512; i++) {
    unsigned long curr = *(pgd + i);
    kprintf("[%d] %d\n", i, curr);
  }

  pgd += 512;
  kprintf("PMD Table Dump:\n");
  for (int i = 0; i < 512; i++) {
    unsigned long curr = *(pgd + i);
    kprintf("[%d] %d (%d)\n", i, curr, (curr >> 12));
  }

  asm volatile ("ldr x0, =kmain");
  asm volatile ("lsl x0, x0, #17");
  asm volatile ("lsr x0, x0, #17");
  asm volatile ("br x0");
  while (1) {
    kprintf("*");
  }
  /*

  char *context_switching_mem = (char *) alloc_page();
  kprintf("Address for context switching space: %p\n", context_switching_mem);
  for (int i = 0; i < 2048; i++) {
    *(context_switching_mem + i) = i;
  }

  new_kernel_thread(spinAndInc, "\t\t\t");
  new_kernel_thread(spinAndInc, "\t\t\t\t\t\t");
  new_kernel_thread(spinAndInc, "\t\t\t\t\t\t\t\t\t");
  new_user_thread(userFunction, "\t\t\t\t\t\t\t\t\t\t\t\t\t");
  spinAndInc("");
  */
}
