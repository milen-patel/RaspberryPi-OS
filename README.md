---
layout: default
title: Build an Operating System
nav_order: 1
permalink: /
has_children: true
---

# A Raspberry Pi Operating Sytem

# Tutorials
* **Lesson 00** - Introduction and Motivation
* **Lesson 01** - Understanding the Rasberry Pi Boot Sequence
* **Lesson 02** - Cross Compiling from x86 to ARMv8
* **Lesson XX** - Booting the Pi
* **Lesson XX** - Setting up the C Runtime Environment
* **Lesson XX** - Setting up UART
* **Lesson XX** - Simplying Compilation with a Makefile
* **Lesson XX** - Implementing `printf()` 
* **Lesson XX** - Understanding Exception Levels
* **Lesson XX** - Setting up Interrupt Handlers
* **Lesson XX** - Enabling the System Timer
* **Lesson XX** - Dividing RAM into smaller pages
* **Lesson XX** - Implementing `malloc()` and `free()`
* **Lesson XX** - Understanding threads and processes
* **Lesson XX** - Context Switching
* **Lesson XX** - User Threads vs. Kernel Threads
* **Lesson XX** - Virtualizing Kernel Memory
* **Lesson XX** - Virtualizing User Space
* **Lesson XX** - System Calls
* **Lesson XX** - Spinlocks



# Task List
1. Create Makefile to correctly compile stand-alone kernel
2. In boot.S, disable all non-primary cores and switch exception level to EL1 (whereas EL3 is the default)
3. Set up GPIO Pins, initialize the miniUART
4. Create interface for reading and writing to/from the UART
5. Implement printf
6. Set up Interrupt Vector Table
7. Enable Raspberry Pi Hardware Timer and Configure for interrupts
8. Wrangle open RAM Space into 4kb Pages, create functions for reserving/freeing pages
9. Ability to create kernel threads
10. Implement a memory manager for the kernel (kmalloc/kree)

# Running Tasks
* Figure out why uart_recv and uart_send can't have their while loops optimized
* Add library function for reading uart line, uart_printf, etc.


# Some Explanations
## Why do we have to name the final output file kernel8.img?
To be completed...

## Understanding LED indicators on the Rasberry Pi
To be completed...

## Accessing the Exception Level

In the ARMv8 architecture, the current exception level (EL) is determined by the value of the Current Exception Level (CURRENTEL) register, which is a read-only register that holds the current EL.

You can obtain the current EL by reading the CURRENTEL register using an instruction such as MRS, which is used to read system registers. Here is an example of how you can use the MRS instruction to read the CURRENTEL register and store the result in a general-purpose register:
```
MRS x0, CURRENTEL
```

This instruction will read the CURRENTEL register and store the result in register x0. The value of the CURRENTEL register is encoded as a 2-bit field, with the following possible values:

EL0: This is the lowest exception level, and is used for normal, unprivileged code.
EL1: This is the exception level used for privileged code, such as the operating system kernel.
EL2: This is an optional exception level that is used for hypervisors or other specialized systems.
EL3: This is the highest exception level, and is used for secure monitors or other trusted code.
You can use the value of the CURRENTEL register to determine the current exception level and execute different code paths depending on the exception level. For example, you might want to execute different code depending on whether you are running in EL0 (unprivileged) or EL1 (privileged) mode.

Note that the CURRENTEL register is a read-only register, so you cannot directly modify its value. To change the exception level, you must use an instruction such as MSR to write to the Exception Level (ELR) register, which holds the exception level that will be entered when returning from an exception.

The CURRENTEL register is encoded as a 2-bit field, which means that it is necessary to shift the value right by two bits in order to extract the EL value.

You can read further into this here: https://developer.arm.com/documentation/ddi0595/2021-06/AArch64-Registers/CurrentEL--Current-Exception-Level

## Context Switching
One of the most difficult things to understand in the kernel is how we switch between two processes. The following section gives some explanation that you may find useful as you read through the code

1. Timer Interrupt happens
2. All of the general purpose registers are dumped onto the stack 
	(https://github.com/s-matyukevich/raspberry-pi-os/blob/master/src/lesson04/src/entry.S)
3. Assembly calls handle_irq
4. handle_irq sees that the IRQ came from the system timer and calls handle_timer_irq
5. Handle_timer_irq sets up the clock for the next interrupt and then calls timer_tick
	It also acknowledges the interrupt has been handled so the next one can come in
6. Timer tick determines if the current process should keep running
	YES - it returns - all of the functions return - OG registers popped from stack and execution resumes
	NO - schedule() is called
7. schedule() determines the next thread to run
8. Schedule calls switch_to()
	
NOTE: All of this logic for the past few function calls has been triggered from an interrupt but technically 
the kernel is using A's stack for its variables so the stack looks like this

```
interrupt handler stack
-----------------------
process dumped register state
-----------------------
process stack up to the interrupt
```


9. switch_to() updates the prev and curr tasks variables
10. switch_to() calls cpu_switch_to(old process, new process)
11. cpu_switch_to stores all of the x19-x30 registers into the old processes PCB entry 
	This includes the stack pointer and return function address
	TODO make sure you know why not x0-x18
12. cpu_switch_to takes all the registers from the new processes PCB and puts them into the actual registers
13. cpu_switch_to executes the ret instruction 

Now this means that we are fully executing B (assuming we swapped from A to B)

A is in this somewhat odd state since it has a lot more info on its stack than at the moment it was interrupted



NOW, suppose b runs for a while and then it gets an interrupt, the same set of steps happens again

That is: B's registers are dumped onto its stack, the schedular gets called
If the scheduler chooses to run B again, its the not-so-interesting case.
Suppose the scheduler then wants to return to A.

We then have B's stack looking like this:

```
interrupt handler stack for B
-----------------------
process dumped register state of B
-----------------------
process stack up to the interrupt of B
```

And now its kind of in the exact same position that A was in. Okay so then what happens when we swap back to A?

1. cpu_switch_to restores A's registers back from the PCB entry 
	These aren't the A registers from the programs execution but what they were from the interrupt handler
2. cpu_switch_to executes the `ret` which contains the address of switch_to() since switch_to() called cpu_switch_to()
3. Execution is back in switch_to() but there is no more instruction so we return from switch_to
	Notice how the stack is decreasing getting closer back to the original A stack
4. switch_to() returns and now we are back in schedule()
5. schedule() returns since there is nothing else left to do
6. timer_tick() disables IRQS (TODO when are they reenabled) and returns
7. handle_timer_irq() returns
8. handle_irq() returns
9. we are back in assembly...we restore the OG A registers that we initally pushed onto the stack
10. we return finally execution to A and it continues as expected

