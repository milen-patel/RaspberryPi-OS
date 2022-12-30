#include "timer/timer.h"
#include "mmio.h"
#include "sys-registers/timer.h"
#include "interrupts/interrupt.h"
#include "libk/printf.h"
#include "libk/memcpy.h"
#include "process/pcb.h"

// Determines the frequency of timer interrupts
const unsigned int interval = 200000 * 15;
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
    char * base = (char *) 4210688;
    for (int i = 0; i <= 30; ++i)
        kprintf("x%d=%d\n", i, *(base + 16*i));
    kprintf("ELR_EL1=%d\nSPSR_EL1=%d\n", *(base + 31*16), *(base + 32*16));

    memcpy(base, currProc, sizeof(uint64_t) * 33);
    kprintf("x0=%d,x5=%d,x30=%d,elr=%d,spsr=%d\n", currProc->registers.x0, currProc->registers.x5, currProc->registers.program_counter, currProc->registers.exception_link_register, currProc->registers.saved_program_status_register);
    // Save it into the PCB of the currently running process
    
    kprintf("======REGISTER STATE OF CURRENTLY INTERRUPTED PROCESS=======\n");
    schedule();
}