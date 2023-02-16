#include "memory-management.h"
#include "exception-level.h"
# This is where the kernel starts executing when the RPi turns on!
# Our goal is to get out of assembly and start executing C code

# This ensures that this assembly code is loaded at the start of the kernel image
# Reference link.ld to see how this is done
.section ".text.boot"  

# This is to be used by the linker, it is used to mark that this should be the entry
# point for all the compiled files
# See: https://stackoverflow.com/questions/17898989/what-is-global-start-in-assembly-language
.global _start

_start:
	# Get the processor id from the system register
	mrs x0, mpidr_el1
	and x0, x0, #0xFF

	# Only CPU0 should continue
	cbz x0, init_exception_level
	sub x0, x0, #1
	cbz x0, cpu2

	# If you make it here, you are not CPU0, so go into an infinite loop
	b proc_hang

cpu2:
	wfe 
	b cpu2
	mov sp, #START_OF_KERNEL_STACK
	add sp, sp, #4096
	bl kmain2


.global proc_hang
proc_hang:
	# Spinning in an infinite loop is expensive, RPi is embedded so power is a concern. wfe helps with this
	wfe
	b proc_hang

init_exception_level:
	# In EL1, we want to disable the MMU, instruct processes to use little endian, and for now also disable caching
	ldr x0, =SCTLR_VALUE_MMU_DISABLED
	msr sctlr_el1, x0

	# The Execution state for EL1 is AArch64
	ldr x0, =HCR_VALUE
	msr hcr_el2, x0

	# Ensure low EL levels (i.e. 0 and 1) operate in non-secure world
	ldr    x0, =SCR_VALUE
    msr    scr_el3, x0

	# Disable interrupts in EL1 and let EL1 use its own stack pointer instead of the EL0 sp
	# This is also how we 'trick' the CPU into changing EL, part of the saved program register
	# state includes the exception level that the exception was taken from. Right now we are in EL3,
	# but if we say that the exception came from EL1 then when we eret, we will end up in EL1.
	#
	# See page 393 of the manual to exlain this and why we use 5 to get to EL1 and 9 for EL2
	ldr    x0, =SPSR_VALUE
    msr    spsr_el3, x0

	# Now, we want to goto master and eret will also change the EL
	adr x0, master
	msr elr_el3, x0
	eret

master:
	# Clear out the bss section
	adr x0, bss_begin
	adr x1, bss_end
	sub x1, x1, x0
	bl memzero
	# Without paging, it would be at this point where we jump to our C code, however we must now first set up the paging tables AND enable paging before making that jump

	# First, clear out the 3 pages we reserved for the kernel paging tables
	adrp x0, kernel_page_table_start
	mov x1, #4096 * 3
	bl memzero

	# Now, we treat the first reserved page as the PGD table, the second table as PUD, and the third as PMD table
	# To initialize the PGD table, we need to create one entry which points to the PUD table (which is kernel_page_table_start + PAGE_SIZE)
	# PGD[0] = &PUD (But we cannot just set the value to the address of PUD, we must put it into descriptor format)
	adrp x0, kernel_page_table_start 
	add x0, x0, #PAGE_SIZE 
	orr x0, x0, 0b11 

	# Now x0 = entry to place into PGD table
	adrp x1, kernel_page_table_start 
	str x0, [x1]

	# Now we must go into the PUD table and place the address of the PMD table
	adrp x0, kernel_page_table_start
	add x0, x0, #PAGE_SIZE
	add x1, x0, #PAGE_SIZE
	orr x1, x1, 0b11
	str x1, [x0]

	# Before we populate the entries of the PMD table, we must set up the MAIR register so we can pass the correct descriptor flags to all of our PMD entries
	ldr    x0, =(MAIR_VALUE)
    msr    mair_el1, x0

	# Now we must go through the PMD table and fill every entry with a physical section address
	# Lets map virtual addresses starting at 0xFFFF000000000000
	# x0 = current index into the PMD table
	mov x0, #0
	# x1 = base address of the PMD table
	adrp x1, kernel_page_table_start
	add x1, x1, #PAGE_SIZE
	add x1, x1, #PAGE_SIZE
	# x2 = physical address to go in the next section of the PMD table
	mov x2, #0

	# Create Descriptor for first entry
	mov x3, #MMU_FLAGS
	orr x2, x2, x3
vmap_loop:
	# Store descriptor into correct offset into table
	str x2, [x1, x0, lsl #3]

	# Update x2 to point to the next section address
	mov x9, #1
	lsl x9, x9, #21
	add x2, x2, x9

	# See if we need to populate the next index in the table, if so, branch back to the top of the loop
	add x0, x0, #1
	mov x9, #512 
	cmp x0, x9
	b.ls vmap_loop

	# Set the stack pointer to use a virtual address for when we jump to C
	mov x0, #VA_START
	mov x1, #START_OF_KERNEL_STACK
	add sp, x0, x1


	# Set the page table register to the physical address of the PGD table
	adrp x0, kernel_page_table_start
	msr ttbr1_el1, x0

	# Set configuration parameters for the MMU as a whole (TODO)
	ldr    x0, =(TCR_VALUE)
    msr    tcr_el1, x0

	# Set the value of the MAIR register (TODO)
	ldr    x0, =(MAIR_VALUE)
    msr    mair_el1, x0

	bl init_interrupt_request_table
	mov sp, #START_OF_KERNEL_STACK
    bl kmain

	ldr x0, =kmain
	mov x1, #SCTLR_MMU_ENABLED
	msr    sctlr_el1, x1
	br x0

	# We shouldn't ever make it here, but if we somehow do, just spin the CPU
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