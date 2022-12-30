#include "interrupts/exception-vector-indices.h"

// push all of the EL0 registers onto the kernel stack so we dont mess up while handling interrupts
// See for why we cant just push: https://stackoverflow.com/questions/27941220/push-lr-and-pop-lr-in-arm-arch64
.macro push_kernel_state_onto_stack
    # Push the general purpose registers onto the stack
    STP x0, x1, [sp, #-16]!
    STP x2, x3, [sp, #-16]!
    STP x4, x5, [sp, #-16]!
    STP x6, x7, [sp, #-16]!
    STP x8, x9, [sp, #-16]!
    STP x10, x11, [sp, #-16]!
    STP x12, x13, [sp, #-16]!
    STP x14, x15, [sp, #-16]!
    STP x16, x17, [sp, #-16]!
    STP x18, x19, [sp, #-16]!
    STP x20, x21, [sp, #-16]!
    STP x22, x23, [sp, #-16]!
    STP x24, x25, [sp, #-16]!
    STP x26, x27, [sp, #-16]!
    STP x28, x29, [sp, #-16]!
    STP x29, x30, [sp, #-16]!

    # Push the system registers for the exception onto the stack
    # TODO need to handle ELR_ERX
    MRS x0, ELR_EL1
    MRS x1, SPSR_EL1
    STP x0, x1, [sp, #-16]
.endm

.macro restore_kernel_state_from_stack_and_exit
    # Pop the system state off of the stack and restore to the correct system registers
    LDP x0, x1, [sp], #16
    MSR ELR_EL1, x0
    MSR SPSR_EL1, x0

    # Now, pop the general purpose registers (i.e. the ones that the running program was using)
    LDP x29, x30, [sp], #16 
    LDP x29, x30, [sp], #16 
    LDP x28, x29, [sp], #16
    LDP x26, x27, [sp], #16
    LDP x24, x25, [sp], #16
    LDP x22, x23, [sp], #16
    LDP x20, x21, [sp], #16
    LDP x18, x19, [sp], #16
    LDP x16, x17, [sp], #16
    LDP x14, x15, [sp], #16
    LDP x12, x13, [sp], #16
    LDP x10, x11, [sp], #16
    LDP x8, x9, [sp], #16
    LDP x6, x7, [sp], #16
    LDP x4, x5, [sp], #16
    LDP x2, x3, [sp], #16
    LDP x0, x1, [sp], #16
    eret
.endm

// When we get an invalid interrupt type, want to print it to the screen and then spin!
.macro handle_invalid_entry type
   // Save the register state
   push_kernel_state_onto_stack 

    mov x0, #\type // i.e. index into the exception vector table

    // See page 2432 of the aarch64 manual
    mrs x1, esr_el1 // i.e. the cause of the exception

    // For synchronous exceptions, we would like to know the address that caused the exception
    mrs x2, elr_el1

    // Print error to screen
    bl show_unknown_interrupt_msg

    // Spin (for now)
    b proc_hang

.endm

// Initialize the exception vector table

.align 11 // (2^7 bytes/entry) * 2^4 entries = 2^11 total space
.global exception_vector_list
exception_vector_list:
    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry SYNCHRONOUS_EL1_SP_EL0

    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry IRQ_EL1_SP_EL0 
    
    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry FIQ_EL1_SP_EL0
    
    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry SERROR_EL1_SP_EL0
    
    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry SYNCHRONOUS_EL1_SP_EL1
    
    .align 7 // (2^7 bytes per entry)
    b el1_irq_handler

    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry FIQ_EL1_SP_EL1
    
    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry SERROR_EL1_SP_EL1
    
    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry SYNCHRONOUS_EL0_AARCH64
    
    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry IRQ_EL0_AARCH64 
    
    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry FIQ_EL0_AARCH64
    
    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry SERROR_EL0_AARCH64
    
    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry SYNCHRONOUS_EL0_AARCH32
    
    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry IRQ_EL0_AARCH32
    
    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry FIQ_EL0_AARCH32 
    
    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry SERROR_EL0_AARCH32


el1_irq_handler:
    push_kernel_state_onto_stack
    bl handle_irq
    restore_kernel_state_from_stack_and_exit

    