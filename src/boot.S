#include "memory-management.h"
#include "exception-level.h"
// This is where the kernel starts executing when the RPi turns on!
// Our goal is to get out of assembly and start executing C code

// This ensures that this assembly code is loaded at the start of the kernel image
// Reference link.ld to see how this is done
.section ".text.boot"  

// This is to be used by the linker, it is used to mark that this should be the entry
// point for all the compiled files
// See: https://stackoverflow.com/questions/17898989/what-is-global-start-in-assembly-language
.global _start

_start:
	// Get the processor id from the system register
	mrs x0, mpidr_el1
	and x0, x0, #0xFF

	// Only CPU0 should continue
	cbz x0, init_exception_level
	sub x0, x0, #1
	cbz x0, cpu2

	// If you make it here, you are not CPU0, so go into an infinite loop
	b proc_hang

cpu2:
	mov sp, #START_OF_KERNEL_STACK
	add sp, sp, #4096
	bl kmain2


.global proc_hang
proc_hang:
	// Spinning in an infinite loop is expensive, RPi is embedded so power is a concern. wfe helps with this
	wfe
	b proc_hang

init_exception_level:
	// In EL1, we want to disable the MMU, instruct processes to use little endian, and for now also disable caching
	ldr x0, =SCTLR_VALUE_MMU_DISABLED
	msr sctlr_el1, x0

	// The Execution state for EL1 is AArch64
	ldr x0, =HCR_VALUE
	msr hcr_el2, x0

	// Ensure low EL levels (i.e. 0 and 1) operate in non-secure world
	ldr    x0, =SCR_VALUE
    msr    scr_el3, x0

	// Disable interrupts in EL1 and let EL1 use its own stack pointer instead of the EL0 sp
	// This is also how we 'trick' the CPU into changing EL, part of the saved program register
	// state includes the exception level that the exception was taken from. Right now we are in EL3,
	// but if we say that the exception came from EL1 then when we eret, we will end up in EL1.
	//
	// See page 393 of the manual to exlain this and why we use 5 to get to EL1 and 9 for EL2
	ldr    x0, =SPSR_VALUE
    msr    spsr_el3, x0

	// Now, we want to goto master and eret will also change the EL
	adr x0, master
	msr elr_el3, x0
	eret

master:
	// Clear out the bss section
	adr x0, bss_begin
	adr x1, bss_end
	sub x1, x1, x0
	bl memzero

	// Set the stack pointer and jump to C
	mov sp, #START_OF_KERNEL_STACK
	bl kmain
	// We shouldn't ever make it here, but if we somehow do, just spin the CPU
	b proc_hang

.global getExceptionLevel
getExceptionLevel: 
	mrs x0, CURRENTEL
	lsr x0, x0, #2
	ret

.globl memcpy
	memcpy:
		ldr x3, [x1], #8
		str x3, [x0], #8
		subs x2, x2, #8
		b.gt memcpy
		ret