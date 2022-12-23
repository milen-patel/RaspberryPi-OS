#ifndef _SUIM_H
#define _SUIM_H

// Both of these functions are called based on if the IRQ was from an expected source configuration or not
void show_unknown_interrupt_msg(int index, unsigned long cause, unsigned long address);
void handle_irq();

// Initialize the RPi interrupt request handler device
void init_rpi_interrupt_handler();

#define PRIMARY_TIMER_IRQ (1 << 1)
#define SECONDARY_TIMER_IRQ (1 << 3)
#endif