#include "timer/timer.h"
#include "mmio.h"
#include "sys-registers/timer.h"
#include "interrupts/interrupt.h"
#include "libk/printf.h"
#include "libk/memcpy.h"
#include "process/pcb.h"

// Determines the frequency of timer interrupts
const unsigned int interval = 200000 * 5;
unsigned long curVal = 0;

void init_timer() {
    // Set the time of the next interrupt
    curVal = get32(TIMER_CLO);
    curVal += interval;

    kprintf("Expect Next Timer Interrupt at: %d\n", curVal);
    put32(TIMER_C1, curVal); 

    // Set the secondary timer to go off once as sake of illustration
    // put32(TIMER_C3, curVal + 8);
}

// Needed to be able to reference the variable defined in schedule.c
extern struct pcb *currProc;

void handle_timer_irq() {
    // Set the time of the next interrupt
    curVal += interval;
    kprintf("Expect next timer interrupt at %d\n", curVal);

    // Inform the timer of when we want this next interrupt
    put32(TIMER_C1, curVal);

    // Tell the timer that we have acknowledged this interrupt
    // If you didnt include this, then as soon as the handler returns
    // the interrupt would get called again and you would end up back here
    // since the timer doesn't know that you have already handled it.
    put32(TIMER_CS, PRIMARY_TIMER_IRQ); 

    // Choose the next process to run and swap to it
    kprintf("======REGISTER STATE OF CURRENTLY INTERRUPTED PROCESS=======\n");
    long * base = (long *) 4210688;
    for (int i = 0; i <= 30; ++i)
        kprintf("x%d=%d\n", i, *(base + i));

    currProc->registers.x0 = *(base + 0);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 0, *(base + 0), currProc->registers.x0); 
    currProc->registers.x1 = *(base + 1); 
    kprintf("If register state moved correctly, x%d:%d==%d\n", 1, *(base + 1), currProc->registers.x1); 
    currProc->registers.x2 = *(base + 2);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 2, *(base + 2), currProc->registers.x2); 
    currProc->registers.x3 = *(base + 3);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 3, *(base + 3), currProc->registers.x3); 
    currProc->registers.x4 = *(base + 4);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 4, *(base + 4), currProc->registers.x4); 
    currProc->registers.x5 = *(base + 5);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 5, *(base + 5), currProc->registers.x5); 
    currProc->registers.x6 = *(base + 6);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 6, *(base + 6), currProc->registers.x6); 
    currProc->registers.x7 = *(base + 7);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 7, *(base + 7), currProc->registers.x7); 
    currProc->registers.x8 = *(base + 8);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 8, *(base + 8), currProc->registers.x8); 
    currProc->registers.x9 = *(base + 9);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 9, *(base + 9), currProc->registers.x9); 
    currProc->registers.x10 = *(base + 10);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 10, *(base + 10), currProc->registers.x10); 
    currProc->registers.x11 = *(base + 11);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 11, *(base + 11), currProc->registers.x11); 
    currProc->registers.x12 = *(base + 12);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 12, *(base + 12), currProc->registers.x12); 
    currProc->registers.x13 = *(base + 13);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 13, *(base + 13), currProc->registers.x13); 
    currProc->registers.x14 = *(base + 14);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 14, *(base + 14), currProc->registers.x14); 
    currProc->registers.x15 = *(base + 15);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 15, *(base + 15), currProc->registers.x15); 
    currProc->registers.x16 = *(base + 16);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 16, *(base + 16), currProc->registers.x16); 
    currProc->registers.x17 = *(base + 17);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 17, *(base + 17), currProc->registers.x17); 
    currProc->registers.x18 = *(base + 18);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 18, *(base + 18), currProc->registers.x18); 
    currProc->registers.x19 = *(base + 19);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 19, *(base + 19), currProc->registers.x19); 
    currProc->registers.x20 = *(base + 20);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 20, *(base + 20), currProc->registers.x20); 
    currProc->registers.x21 = *(base + 21);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 21, *(base + 21), currProc->registers.x21); 
    currProc->registers.x22 = *(base + 22);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 22, *(base + 22), currProc->registers.x22); 
    currProc->registers.x23 = *(base + 23);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 23, *(base + 23), currProc->registers.x23); 
    currProc->registers.x24 = *(base + 24);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 24, *(base + 24), currProc->registers.x24); 
    currProc->registers.x25 = *(base + 25);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 25, *(base + 25), currProc->registers.x25); 
    currProc->registers.x26 = *(base + 26);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 26, *(base + 26), currProc->registers.x26); 
    currProc->registers.x27 = *(base + 27);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 27, *(base + 27), currProc->registers.x27); 
    currProc->registers.x28 = *(base + 28);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 28, *(base + 28), currProc->registers.x28); 
    currProc->registers.x29 = *(base + 29);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 29, *(base + 29), currProc->registers.x29); 
    currProc->registers.x30 = *(base + 30);
    kprintf("If register state moved correctly, x%d:%d==%d\n", 30, *(base + 30), currProc->registers.x30); 
    currProc->registers.exception_link_register = *(base + 31);
    kprintf("If register state moved correctly, elr:%d==%d\n", *(base + 31), currProc->registers.exception_link_register); 
    currProc->registers.saved_program_status_register = *(base + 32);
    kprintf("If register state moved correctly, spsr:%d==%d\n", *(base + 32), currProc->registers.saved_program_status_register); 
    currProc->registers.stack_pointer = *(base + 33);
    kprintf("If register state moved correctly, sp:%d==%d\n", *(base + 33), currProc->registers.stack_pointer); 

    kprintf("======REGISTER STATE OF CURRENTLY INTERRUPTED PROCESS=======\n");
    schedule();
}