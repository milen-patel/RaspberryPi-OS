#include "uart.h"
#include "printf.h"
#include "get-el.h"
#include "interrupts/interrupt.h"
#include "timer/timer.h"
#include "interrupts/toggle.h"
#include "mmio.h"
#include "sys-registers/timer.h"
#include "paging/alloc.h"
#include "process/fork.h"
#include "process/pcb.h"
#include "libk/malloc.h"
#include <stdbool.h>
#include <stdint.h>

// Once boot.S finishes setting up the C runtime environment, it will defer execution to kmain
bool shouldSecondCPUStart = false;
void kmain2() {
  while (!shouldSecondCPUStart);
  //char c = 'x';
  //printf("CPU 2 Kernel Stack Local Variable Address: %p\n", &c);
}

void loop(char *arr) {
  unsigned long curr = 0;
  while (1) {
    curr++;
    if (curr % 10000 == 0) {
      printf("Process %s is at value %d\n", arr, curr);
    }
    if (arr[4] == 'a' && curr == 100000) {
      if(!fork(loop, "PROCc")) printf("Failed to fork!\n"); 
    }
  }
}

void kmain(void) {
  uart_init();
  printf("===============================================================================\n");
  printf("[time = %d] UART has been initialized...\n", get32(TIMER_CLO));
  printf("[time = %d] Current Kernel Exception Level: %d\n", get32(TIMER_CLO), getExceptionLevel());

  init_interrupt_request_table();
  printf("[time = %d] Interrupt Request Table has been set up\n", get32(TIMER_CLO));

  init_rpi_interrupt_handler();
  printf("[time = %d] Raspberry Pi interrupt controller has been configured \n", get32(TIMER_CLO));

  enable_irq();   
  printf("[time = %d] Interrupt Requests Have Been Enabled\n", get32(TIMER_CLO));

  init_paging();
  printf("[time = %d] Paging has been initialized\n", get32(TIMER_CLO));

  kmalloc_test();
  printf("[time = %d] kmalloc() appears to be working as expected\n", get32(TIMER_CLO));

  init_timer();
  printf("[time = %d] Raspberry Pi Hardware Timer Has been set up\n", get32(TIMER_CLO));
  printf("===============================================================================\n");


  // Create two new runnable kthreads 
  if(!fork(loop, "PROCa")) printf("Failed to fork!\n");
  if(!fork(loop, "PROCb")) printf("Failed to fork!\n");

  shouldSecondCPUStart = true;
  delay(10000);

  while (1) {
    //char c = uart_recv();
    //printf("[time=%d]%c\n", get32(TIMER_CLO), c);
    // Main thread has nothing to do, so yield to the other functions we have
    printf("INIT task has control, calling schedule()\n");
    schedule();
	}
}
