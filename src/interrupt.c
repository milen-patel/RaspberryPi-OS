#include "interrupts/interrupt.h"
#include "sys-registers/interrupt.h"
#include "timer/timer.h"
#include "sys-registers/timer.h"
#include "printf.h"
#include "mmio.h"

/*
 * This function is called from the exception vector table if an interrupt was generated
 * from an unexpected source (combination of EL + interrupt type (IRQ, FRQ, etc.))
 * 
 * This function will be the last thing the kernel executes as it will hang after return.
 */
void show_unknown_interrupt_msg(int index, unsigned long cause, unsigned long address) {
    printf("Unexpected Interrupt! Index %d, Cause %d, Address %d\n", index, cause, address);
}

/*
 * This is the function that will be called from the exception vector table that
 * was defined in the assembly code. This function will need to determine the source
 * of the exception and handle it accordingly
 */
void handle_irq() {
    printf("[time=%d] IRQ Recieved\n", get32(TIMER_CLO));
    unsigned int irq = get32(IRQ_PENDING_1);
    switch (irq) {
        case ((1 << 1)):
            handle_timer_irq();
            break;
        default:
            printf("Unknown pending irq: %d\n", irq);
    }
    return;
}

// Enables System Timer #1 IRQ
void init_rpi_interrupt_handler() {
    put32(IRQ_ER_1, 0x2);
}