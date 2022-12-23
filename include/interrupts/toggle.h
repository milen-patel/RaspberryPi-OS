#ifndef _TOGGLE_H
#define _TOGGLE_H

// Once the interrupt vector table has been set up with proper instructions in each of the 16 slots,
// this function can be called and will place the address of the base of this table into the correct
// system register
void init_interrupt_request_table();

// Used to enable IRQs after an interrupt handler finishes
void enable_irq();

// Used to disable IRQs while an interrupt handler is running
void disable_irq();
#endif