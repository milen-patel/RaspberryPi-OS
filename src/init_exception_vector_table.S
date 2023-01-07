#include "interrupts/exception-vector-indices.h"

// push all of the EL0 registers onto the kernel stack so we dont mess up while handling interrupts
// See for why we cant just push: https://stackoverflow.com/questions/27941220/push-lr-and-pop-lr-in-arm-arch64
.macro push_kernel_state_onto_stack
    # Now store all this information in a particular spot in memory

    # Store x1 onto the stack and then load the base address of where we want to store regstate into memory to x1
    STR x1, [sp, #-8]!
    mov x1, #0x4040
    lsl x1, x1, #8

     # Now push x0
     STR x0, [x1, #0 * 8] 

     # Now we have x0's value saved, we can pop x1 off the stack into x0 and store that into memory
     LDR x0, [sp], #8
     STR x0, [x1, #1 * 8]

     STR x2, [x1, #2 * 8]
     STR x3, [x1, #3 * 8]
     STR x4, [x1, #4 * 8]
     STR x5, [x1, #5 * 8]
     STR x6, [x1, #6 * 8]
     STR x7, [x1, #7 * 8]
     STR x8, [x1, #8 * 8]
     STR x9, [x1, #9 * 8]
     STR x10, [x1, #10 * 8]
     STR x11, [x1, #11 * 8]
     STR x12, [x1, #12 * 8]
     STR x13, [x1, #13 * 8]
     STR x14, [x1, #14 * 8]
     STR x15, [x1, #15 * 8]
     STR x16, [x1, #16 * 8]
     STR x17, [x1, #17 * 8]
     STR x18, [x1, #18 * 8]
     STR x19, [x1, #19 * 8]
     STR x20, [x1, #20 * 8]
     STR x21, [x1, #21 * 8]
     STR x22, [x1, #22 * 8]
     STR x23, [x1, #23 * 8]
     STR x24, [x1, #24 * 8]
     STR x25, [x1, #25 * 8]
     STR x26, [x1, #26 * 8]
     STR x27, [x1, #27 * 8]
     STR x28, [x1, #28 * 8]
     STR x29, [x1, #29 * 8]
     STR x30, [x1, #30 * 8]

     MRS x3, ELR_EL1
     MRS x4, SPSR_EL1
     STR x3, [x1, #31 * 8]
     STR x4, [x1, #32 * 8]
     mov x3, sp
     STR x3, [x1, #33 * 8]

     MRS x3, ELR_EL1
     CBZ x3, show_unknown_interrupt_msg 

.endm

.macro restore_kernel_state_from_stack_and_exit
    # Load the base address back into x1, the deal is that schedule should have updated all the values here for the next process to run
    mov x1, #0x4040
    lsl x1, x1, #8

    # Start by loading the system registers
    LDR x3, [x1, #31 * 8]
    LDR x4, [x1, #32 * 8]
    MSR ELR_EL1, x3
    MSR SPSR_EL1, x4 
    LDR x3, [x1, #33 * 8]
    mov SP, x3

     # Now load the general registers
    LDR x0, [x1, #0 * 8]
    # == We Intentionally Skip x1 since we are using that to hold the read address ==
    LDR x2, [x1, #2 * 8]
    LDR x3, [x1, #3 * 8]
    LDR x4, [x1, #4 * 8]
    LDR x5, [x1, #5 * 8]
    LDR x6, [x1, #6 * 8]
    LDR x7, [x1, #7 * 8]
    LDR x8, [x1, #8 * 8]
    LDR x9, [x1, #9 * 8]
    LDR x10, [x1, #10 * 8]
    LDR x11, [x1, #11 * 8]
    LDR x12, [x1, #12 * 8]
    LDR x13, [x1, #13 * 8]
    LDR x14, [x1, #14 * 8]
    LDR x15, [x1, #15 * 8]
    LDR x16, [x1, #16 * 8]
    LDR x17, [x1, #17 * 8]
    LDR x18, [x1, #18 * 8]
    LDR x19, [x1, #19 * 8]
    LDR x20, [x1, #20 * 8]
    LDR x21, [x1, #21 * 8]
    LDR x22, [x1, #22 * 8]
    LDR x23, [x1, #23 * 8]
    LDR x24, [x1, #24 * 8]
    LDR x25, [x1, #25 * 8]
    LDR x26, [x1, #26 * 8]
    LDR x27, [x1, #27 * 8]
    LDR x28, [x1, #28 * 8]
    LDR x29, [x1, #29 * 8]
    LDR x30, [x1, #30 * 8]
    # Now load x1
    LDR x1, [x1, #1 * 8]

    # Now that we have restored register state, return to exactly where we left off before the interrupt happened
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

.global terminate_interrupt
terminate_interrupt:
    restore_kernel_state_from_stack_and_exit