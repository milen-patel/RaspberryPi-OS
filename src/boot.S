#include "memory-management.h"
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
	cbz x0, master
	// If you make it here, you are not CPU0, so go into an infinite loop
	b proc_hang

proc_hang:
	// Spinning in an infinite loop is expensive, RPi is embedded so power is a concern. wfe helps with this
	wfe
	b proc_hang

master:
	// Clear out the bss section
	//adr x0, bss_begin
	//adr x1, bss_end
	//sub x1, x1, x0
	//bl memzero

	//// Set the stack pointer and jump to C
	//mov sp, #START_OF_KERNEL_STACK
	bl kmain
	b proc_hang
