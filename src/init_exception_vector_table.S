#include "interrupts/exception-vector-indices.h"

// push all of the EL0 registers onto the kernel stack so we dont mess up while handling interrupts
// See for why we cant just push: https://stackoverflow.com/questions/27941220/push-lr-and-pop-lr-in-arm-arch64
.macro push_kernel_state_onto_stack
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
    # MRS x0, ELR_EL1
    # MRS x1, SPSR_EL1
    # STP x0, x1, [sp, #-16]!

    # Now store all this information in a particular spot in memory
    # MOVZ X1, #0x4040   // Load the upper 16 bits of the constant
    # MOVK X1, #0x0000, LSL #16   // Load the lower 16 bits of the constant
    mov x1, #0x4040
    lsl x1, x1, #8

     # Reserved for x1
     STR x2, [x1, #2 * 16]
     STR x3, [x1, #3 * 16]
     STR x4, [x1, #4 * 16]
     STR x5, [x1, #5 * 16]
     STR x6, [x1, #6 * 16]
     STR x7, [x1, #7 * 16]
     STR x8, [x1, #8 * 16]
     STR x9, [x1, #9 * 16]
     STR x10, [x1, #10 * 16]
     STR x11, [x1, #11 * 16]
     STR x12, [x1, #12 * 16]
     STR x13, [x1, #13 * 16]
     STR x14, [x1, #14 * 16]
     STR x15, [x1, #15 * 16]
     STR x16, [x1, #16 * 16]
     STR x17, [x1, #17 * 16]
     STR x18, [x1, #18 * 16]
     STR x19, [x1, #19 * 16]
     STR x20, [x1, #20 * 16]
     STR x21, [x1, #21 * 16]
     STR x22, [x1, #22 * 16]
     STR x23, [x1, #23 * 16]
     STR x24, [x1, #24 * 16]
     STR x25, [x1, #25 * 16]
     STR x26, [x1, #26 * 16]
     STR x27, [x1, #27 * 16]
     STR x28, [x1, #28 * 16]
     STR x29, [x1, #29 * 16]
     STR x30, [x1, #30 * 16]

     # You get immediates out of range after this point so just update the value in x1
     MOV x3, #30
     MOV x4, #16
     MUL x2, x3, x4

     ADD x1, x1, x2

     MRS x3, ELR_EL1
     MRS x4, SPSR_EL1
     STR x3, [x1, #16 * 1]
     STR x4, [x1, #16 * 2]


.endm

.macro restore_kernel_state_from_stack_and_exit
    # Pop the system state off of the stack and restore to the correct system registers
    # LDP x0, x1, [sp], #16
    # MSR ELR_EL1, x0
    # MSR SPSR_EL1, x1

    LDP x29, x30, [sp], #16 // Special case, sp must always be 16 bit aligned so we have to deal with this 
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
    b handle_el1_irq

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
    

handle_el1_irq:
    push_kernel_state_onto_stack
    bl handle_irq
    restore_kernel_state_from_stack_and_exit