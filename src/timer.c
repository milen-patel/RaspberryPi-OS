#include "timer/timer.h"
#include "mmio.h"
#include "sys-registers/timer.h"
#include "interrupts/interrupt.h"
#include "printf.h"
#include "process/schedule.h"

// Determines the frequency of timer interrupts
const unsigned int interval = 200000;
unsigned int curVal = 0;

void init_timer() {
    // Set the time of the next interrupt
    curVal = get32(TIMER_CLO);
    curVal += interval;

    printf("Expect Next Timer Interrupt at: %d\n", curVal);
    put32(TIMER_C1, curVal); 

    // Set the secondary timer to go off once as sake of illustration
    put32(TIMER_C3, curVal + 8);
}

void handle_timer_irq() {
    // Set the time of the next interrupt
    curVal += interval;
    printf("Expect next timer interrupt at %d\n", curVal);

    // Inform the timer of when we want this next interrupt
    put32(TIMER_C1, curVal);

    // Tell the timer that we have acknowledged this interrupt
    // If you didnt include this, then as soon as the handler returns
    // the interrupt would get called again and you would end up back here
    // since the timer doesn't know that you have already handled it.
    put32(TIMER_CS, PRIMARY_TIMER_IRQ); 


    timer_tick();
}