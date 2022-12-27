#include "interrupts/interrupt.h"
#include "sys-registers/interrupt.h"
#include "timer/timer.h"
#include "sys-registers/timer.h"
#include "kprintf.h"
#include "mmio.h"

/*
 * This function is called from the exception vector table if an interrupt was generated
 * from an unexpected source (combination of EL + interrupt type (IRQ, FRQ, etc.))
 * 
 * This function will be the last thing the kernel executes as it will hang after return.
 */
void show_unknown_interrupt_msg(int index, unsigned long cause, unsigned long address) {
    kprintf("Unexpected Interrupt! Index %d, Cause %d, Address %d\n", index, cause, address);
}

/*
 * This is the function that will be called from the exception vector table that
 * was defined in the assembly code. This function will need to determine the source
 * of the exception and handle it accordingly
 */
void handle_irq() {
    kprintf("[time=%d] IRQ Recieved\n", get32(TIMER_CLO));
    unsigned int irq = get32(IRQ_PENDING_1);
        if (irq & PRIMARY_TIMER_IRQ) {
            handle_timer_irq();
            return;
        }
        if (irq & SECONDARY_TIMER_IRQ) {
            kprintf("Secondary Timer Interrupt Recieved, This can be used in the future\n");
            put32(TIMER_CS, SECONDARY_TIMER_IRQ);
            return;
        }
            kprintf("Unknown pending irq: %d\n", irq);
    return;
}

// Enables System Timer #1 and #3 IRQ
// #2 and #4 are reserved for the GPU (see https://embedded-xinu.readthedocs.io/en/latest/arm/rpi/BCM2835-System-Timer.html)
void init_rpi_interrupt_handler() {
    put32(IRQ_ER_1, (1 << 1) | (1 << 3));
}