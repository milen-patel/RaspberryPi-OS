#include "uart.h"
#include "kprintf.h"
#include "get-el.h"
#include "interrupts/interrupt.h"
#include "timer/timer.h"
#include "interrupts/toggle.h"
#include "mmio.h"
#include "libk/memzero.h"
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

#include "syscall/syscall.h"
void user_child_fn(char *name) {
  int i = 0;
  while (1) {
    if (i % 5000 == 0) pprintf(name);
    i++;
  }
}
void user_fn() {
  kprintf("Inside of user function...\n");
  void *x = mmap();
  kprintf("*x = %p\n", x);
  pfork(user_child_fn, "X_A\n", x);

  x = mmap();
  kprintf("*y = %p\n", x);
  pfork(user_child_fn, "X_B\n", x);
  int i = 0;
  while (1) {
    if (i % 5000 == 0) {
      pprintf("user_fn\n");
    }
    i++;
  }
}

void kernel_thread_fn() {
  kprintf("The new kernel thread is running at exeption level: %d\n", getExceptionLevel());
  move_kernel_thread_to_user_space(user_fn);
}


void spin_fn(char *txt) {
  kprintf("In the spin_fn\n");
  while (1) {
    kprintf("in spin fn\n");
  }
}

void user_fn_2() {
  kprintf("We are in user_fn_2\n");
  kprintf("user_fn_2 is getting a new page to fork off of\n");
  void *page = mmap();
  kprintf("User_fn_2 has gotten the page whose base address is %p\n", page);
  char *str_a = "A";
  kprintf("in user_fn_2, calling pfork() syscall with function=%p, arg=%p, stack=%p", spin_fn, str_a, page);
  pfork(spin_fn, str_a, page);
  kprintf("user_fn_2 has returned from fork\n");
  while (1) {
    kprintf("user_fn_2 spin loop\n");
  }
}
void kernel_thread_fn_2(void *arg) {
  kprintf("The running thread is %s\n", arg);
  move_kernel_thread_to_user_space(user_fn_2);
}





void kmain(void) {
  uart_init();
  kprintf("===============================================================================\n");
  kprintf("[time = %d] UART has been initialized...\n", get32(TIMER_CLO));
  kprintf("[time = %d] Current Kernel Exception Level: %d\n", get32(TIMER_CLO), getExceptionLevel());

  init_interrupt_request_table();
  kprintf("[time = %d] Interrupt Request Table has been set up\n", get32(TIMER_CLO));

  init_rpi_interrupt_handler();
  kprintf("[time = %d] Raspberry Pi interrupt controller has been configured \n", get32(TIMER_CLO));

  enable_irq();   
  kprintf("[time = %d] Interrupt Requests Have Been Enabled\n", get32(TIMER_CLO));

  init_paging();
  kprintf("[time = %d] Paging has been initialized\n", get32(TIMER_CLO));

  //kmalloc_test();
  //kprintf("[time = %d] kmalloc() appears to be working as expected\n", get32(TIMER_CLO));

  init_timer();
  kprintf("[time = %d] Raspberry Pi Hardware Timer Has been set up\n", get32(TIMER_CLO));
  kprintf("===============================================================================\n");

  shouldSecondCPUStart = true;
  delay(10000);

  if (!fork(1, kernel_thread_fn_2, "Thread 1", 0x0)) {
    kprintf("Failed to fork...\n");
  }
  //if (!fork(1, kernel_thread_fn_2, "Thread 2", 0x0)) {
  //  kprintf("Failed to fork...\n");
  //}
  //if (!fork(1, kernel_thread_fn_2, "Thread 3", 0x0)) {
  //  kprintf("Failed to fork...\n");
  //}

  while (1) {
    //char c = uart_recv();
    //printf("[time=%d]%c\n", get32(TIMER_CLO), c);
    // Main thread has nothing to do, so yield to the other functions we have
    kprintf("INIT task has control, calling schedule()\n");
    schedule();
	}
}
