#include "interrupts/exception-vector-indices.h"
#include "syscall/table.h"

// push all of the EL0 registers onto the kernel stack so we dont mess up while handling interrupts
// See for why we cant just push: https://stackoverflow.com/questions/27941220/push-lr-and-pop-lr-in-arm-arch64
.macro push_kernel_state_onto_stack, el
  sub	sp, sp, #272
	stp	x0, x1, [sp, #16 * 0]
	stp	x2, x3, [sp, #16 * 1]
	stp	x4, x5, [sp, #16 * 2]
	stp	x6, x7, [sp, #16 * 3]
	stp	x8, x9, [sp, #16 * 4]
	stp	x10, x11, [sp, #16 * 5]
	stp	x12, x13, [sp, #16 * 6]
	stp	x14, x15, [sp, #16 * 7]
	stp	x16, x17, [sp, #16 * 8]
	stp	x18, x19, [sp, #16 * 9]
	stp	x20, x21, [sp, #16 * 10]
	stp	x22, x23, [sp, #16 * 11]
	stp	x24, x25, [sp, #16 * 12]
	stp	x26, x27, [sp, #16 * 13]
	stp	x28, x29, [sp, #16 * 14]

	.if	\el == 0
	mrs	x21, sp_el0
	.else
	add	x21, sp, #272
	.endif /* \el == 0 */

	mrs	x22, elr_el1
	mrs	x23, spsr_el1

	stp	x30, x21, [sp, #16 * 15] 
	stp	x22, x23, [sp, #16 * 16]
.endm

.macro restore_kernel_state_from_stack_and_exit, el
	ldp	x22, x23, [sp, #16 * 16]
	ldp	x30, x21, [sp, #16 * 15] 

	.if	\el == 0
	msr	sp_el0, x21
	.endif /* \el == 0 */

	msr	elr_el1, x22			
	msr	spsr_el1, x23


	ldp	x0, x1, [sp, #16 * 0]
	ldp	x2, x3, [sp, #16 * 1]
	ldp	x4, x5, [sp, #16 * 2]
	ldp	x6, x7, [sp, #16 * 3]
	ldp	x8, x9, [sp, #16 * 4]
	ldp	x10, x11, [sp, #16 * 5]
	ldp	x12, x13, [sp, #16 * 6]
	ldp	x14, x15, [sp, #16 * 7]
	ldp	x16, x17, [sp, #16 * 8]
	ldp	x18, x19, [sp, #16 * 9]
	ldp	x20, x21, [sp, #16 * 10]
	ldp	x22, x23, [sp, #16 * 11]
	ldp	x24, x25, [sp, #16 * 12]
	ldp	x26, x27, [sp, #16 * 13]
	ldp	x28, x29, [sp, #16 * 14]
	add	sp, sp, #272	
	eret
.endm

// When we get an invalid interrupt type, want to print it to the screen and then spin!
.macro handle_invalid_entry type
   // Save the register state
   push_kernel_state_onto_stack 1

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
        b el1_irq
     
    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry FIQ_EL1_SP_EL1
    
    .align 7 // (2^7 bytes per entry)
    handle_invalid_entry SERROR_EL1_SP_EL1
    
    .align 7 // (2^7 bytes per entry)
    b el0_sync
    
    .align 7 // (2^7 bytes per entry)
    b el0_irq

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
    
el0_irq:
    push_kernel_state_onto_stack 0
    bl handle_irq
    restore_kernel_state_from_stack_and_exit 0

el1_irq:
    push_kernel_state_onto_stack 1 
    bl handle_irq
    restore_kernel_state_from_stack_and_exit 1

el0_sync:
    # Push register state onto the stack
    push_kernel_state_onto_stack 0
    
    # Figure out where the exception came from
    mrs	x25, esr_el1				// read the syndrome register
	lsr	x24, x25, #26
	cmp	x24, #21
	b.eq	el0_svc
    handle_invalid_entry 69


sc_nr   .req    x25                  // number of system calls
scno    .req    x26                  // syscall number
stbl    .req    x27 

el0_svc:
    adr    stbl, sys_call_table      // load syscall table pointer
    uxtw   scno, w8                  // syscall number in w8
    mov    sc_nr, #NUM_SYSCALLS
    bl     enable_irq
    cmp    scno, sc_nr               // check upper syscall limit
    b.hs   ni_sys

    ldr    x16, [stbl, scno, lsl #3] // address in the syscall table
    blr    x16                       // call sys_* routine
    b      ret_from_syscall

ni_sys:
   handle_invalid_entry SYNCHRONOUS_EL0_AARCH64 

ret_from_syscall:
    bl    disable_irq
    str   x0, [sp, #0]             // returned x0
    restore_kernel_state_from_stack_and_exit 0


.global ret_from_fork
ret_from_fork:
    bl    enable_preemption
    cbz   x19, ret_to_user            // not a kernel thread
    mov   x0, x20
    blr   x19
ret_to_user:
    bl disable_irq
    restore_kernel_state_from_stack_and_exit 0