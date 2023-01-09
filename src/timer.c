#include "timer/timer.h"
#include "mmio.h"
#include "sys-registers/timer.h"
#include "interrupts/interrupt.h"
#include "libk/printf.h"
#include "libk/log.h"
#include "libk/memcpy.h"
#include "process/pcb.h"

// Determines the frequency of timer interrupts
const unsigned int interval = 200000 * 10;
unsigned long curVal = 0;

void init_timer() {
    // Set the time of the next interrupt
    curVal = get32(TIMER_CLO);
    curVal += interval;

    klog("Expect Next Timer Interrupt at: %d\n", curVal);
    put32(TIMER_C1, curVal); 

    // Set the secondary timer to go off once as sake of illustration
    // put32(TIMER_C3, curVal + 8);
}

// Needed to be able to reference the variable defined in schedule.c
extern struct pcb *currProc;

void handle_timer_irq() {
    // Set the time of the next interrupt
    curVal += interval;
    klog("Expect next timer interrupt at %d\n", curVal);

    // Inform the timer of when we want this next interrupt
    put32(TIMER_C1, curVal);

    // Tell the timer that we have acknowledged this interrupt
    // If you didnt include this, then as soon as the handler returns
    // the interrupt would get called again and you would end up back here
    // since the timer doesn't know that you have already handled it.
    put32(TIMER_CS, PRIMARY_TIMER_IRQ); 

    // Choose the next process to run and swap to it
    klog("======REGISTER STATE OF CURRENTLY INTERRUPTED PROCESS=======\n{");
    long * base = (long *) 4210688;
    for (int i = 0; i <= 30; ++i)
        kprintf("\t[x%d=%d]\n", i, *(base + i));
    klog("}\n======REGISTER STATE OF CURRENTLY INTERRUPTED PROCESS=======\n");

    currProc->registers.x0 = *(base + 0);
    currProc->registers.x1 = *(base + 1); 
    currProc->registers.x2 = *(base + 2);
    currProc->registers.x3 = *(base + 3);
    currProc->registers.x4 = *(base + 4);
    currProc->registers.x5 = *(base + 5);
    currProc->registers.x6 = *(base + 6);
    currProc->registers.x7 = *(base + 7);
    currProc->registers.x8 = *(base + 8);
    currProc->registers.x9 = *(base + 9);
    currProc->registers.x10 = *(base + 10);
    currProc->registers.x11 = *(base + 11);
    currProc->registers.x12 = *(base + 12);
    currProc->registers.x13 = *(base + 13);
    currProc->registers.x14 = *(base + 14);
    currProc->registers.x15 = *(base + 15);
    currProc->registers.x16 = *(base + 16);
    currProc->registers.x17 = *(base + 17);
    currProc->registers.x18 = *(base + 18);
    currProc->registers.x19 = *(base + 19);
    currProc->registers.x20 = *(base + 20);
    currProc->registers.x21 = *(base + 21);
    currProc->registers.x22 = *(base + 22);
    currProc->registers.x23 = *(base + 23);
    currProc->registers.x24 = *(base + 24);
    currProc->registers.x25 = *(base + 25);
    currProc->registers.x26 = *(base + 26);
    currProc->registers.x27 = *(base + 27);
    currProc->registers.x28 = *(base + 28);
    currProc->registers.x29 = *(base + 29);
    currProc->registers.x30 = *(base + 30);
    currProc->registers.exception_link_register = *(base + 31);
    currProc->registers.saved_program_status_register = *(base + 32);
    currProc->registers.stack_pointer = *(base + 33);

    schedule();
}