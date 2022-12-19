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
	// TODO might need to disable other cores

    // Initialize the stack pointer for the C runtime environment
	// The hardware does not load the kernel to address 0, instead it loads
	// it to adress 0x80000 and then works towards higher addresses.
	//
	// So, we can set our stack point to this same address and use all the space
	// down to address 0 since that is unused by the kernel but exists!
    ldr     x5, =_start
    mov     sp, x5

    // Clean the BSS section, expected by C runtime environment
    ldr     x1, =__bss_start     // Start address
    ldr     w2, =__bss_size      // Size of the section
	// Compare and branch on 0
3:  cbz     w2, 4f               
	// Store 0 (xzr is the zero register) to address in x1 in memory
	// This also increments x1 (see https://stackoverflow.com/questions/37706031/difference-between-commands-with-and-without-in-arm-assembly)
    str     xzr, [x1], #8
	// Decrease remaining bss size needed to be cleared by 1
    sub     w2, w2, #1
    cbnz    w2, 3b               // Loop if non-zero

    // Jump to our main() routine in C (make sure it doesn't return)
4:  mrs x0, mpidr_el1
	bl      main
    // In case it does return, halt the master core too
halt:
	// This is ARM's no-op
	wfe
	b halt
