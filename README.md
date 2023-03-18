---
layout: default
title: Build an Operating System
nav_order: 1
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

# Lesson 0 - Introduction and Motivation

During my last semester at school, I took a course on Operating Systems which, although very interesting, left more questions than answers. I wanted to know if it was possible to build an actual operating system, so one day after class I asked the professor who told me it wasn't feasible and that getting even the most basic operating system would be too demanding of a task.

That answer was unsatisfying to me, so I did my own research which led me to find a handful of posts from people who managed to write an operating system for the Raspberry Pi from scratch. Fortunately, I had one laying around so I immediately got to work on reading others' tutorials and playing around on my own. Although I found lots of interesting resources, I felt like most of the tutorials were either dated, skipped over key parts, or made otherwise confusing decisions for me to understand. So, I wanted to combine the best aspects of all the resources out there while adding in my own direction which has led me to creating this tutorial.

Although I'm just getting started on writing these, by the time they are finished, I hope they can be of value to anybody else who is either interested in OS development or, like me, had so many unanswered curious questions after taking an OS course.

# Lesson 1 - Understanding the Rasberry Pi Boot Sequence

Although we are writing the kernel for the Raspberry Pi, theres actually a lot more going on behind the scenes. Over the next sequence of tutorials we will be writing code, compiling it into a single file, `kernel8.img`, moving it to an SD card, putting that SD card into the Pi, and then powering up the Pi.

But what exactly happens when we power on the USB? How does the kernel get loaded from the SD card into memory and begin executing? It turns out that the CPU isn't the first thing to get control when we plug in the device.

Instead, the ARM Core is initially turned off and the GPU is turned on. The GPU reads code from a ROM (Read Only Memory) on the hardware itself. This code is the initial bootloader and is closed source which means we can't inspect that code nor can we realistically change the source code on that ROM. The ROM contains the instructions that reads the SD card looking for the yet another bootloader (i.e. second stage bootloader) by finding the file named `bootcode.bin`.

Eventually, `start.elf` is loaded from the SD card which reads our `config.txt` file and importantly `kernel8.img`. The entire kernel image (the part that we will be developing in the coming tutorials), is loaded into memory at address 0 (more precisely, the addresss specified in the linker file but more on that later). It is at this point where execution is transferred to the kernel at the start address. 

As you can see, there is a non-trivial amount of work that happens before the Pi even begins execution of our kernel. It is not as important to understand the specifics of each of the phases for the sake of our project, but I thought it was worth mentioning since I was initially confused as to how the kernel makes it from the SD card to memory. It's also worth noting that all of our execution is happening in memory and we won't have a way to interact with the SD card (eventually this will be considered when we look into file system implementations but there is much more to be done before this).

I intentionally omitted full details in parts of this section because things can get very technical very fast. If you are interested in reading more about the boot process, then feel free to check out some of these links:

 * https://www.nayab.xyz/rpi3b-elinux/embedded-linux-rpi3-030-boot-process
 * https://www.lions-wing.net/maker/raspberry-1/boot.html
 * http://www.diy.ind.in/raspberry-pi/55-run-scripts-on-startup-in-raspberry-pi

 It's also worth noting that this process is largely different for each model of the Raspberry Pi. For the sake of this tutorial, I will be using a Raspberry Pi Model 3. Although I wanted to purchase a Model 4 to keep things up to date, I couldn't justify the $100+ resale price at the time when my Model 3 was performing flawlessly.

# Lesson 2 - Cross Compiling from x86 to ARMv8

I am working on an Intel Mac which natively uses Intel's x86 CPU architecture. One implication is that when I compile any C or assembly code using `gcc` or `clang`, the outputted binary contains x86 instructions. This is not going to work for our project because the Raspberry Pi uses an entirely different instruction set, ARMv8. Here are some key differences between the two:

 * Architecture: ARM is a Reduced Instruction Set Computer (RISC) architecture, while x86 is a Complex Instruction Set Computer (CISC) architecture. RISC architectures have a smaller number of instructions, which are simpler and easier to execute, while CISC architectures have a larger number of instructions, which can be more complex and powerful.
 * Register size: ARM processors have 32-bit registers, while x86 processors have either 32-bit or 64-bit registers. This means that x86 processors can handle more data in a single instruction, but ARM processors can use more registers in parallel.
 * Instruction set: The instruction sets of ARM and x86 are not compatible, which means that code written for one architecture cannot be executed on the other without some form of translation.
 * Power consumption: ARM processors are known for their low power consumption, making them well suited for mobile and embedded devices, while x86 processors are more powerful and tend to consume more power.
 * Market share: x86 processors are widely used in personal computers and servers, while ARM processors are widely used in mobile devices, embedded systems and IoT devices.

There are multiple versions of ARM, but the Raspberry Pi specifically uses ARMv8 which is synonymous with aarch64. This is the link for the documentation on aarch64. This website is incredibly useful and you should spend some time learning about the instruction set. The website is not exhaustive and infact covers only a subset of topics at a high level. If you are interested in seeing the full manual, that is also linked below. Both of these will become invaluable and although the manual contains everything on the website, I often find that the website is more coherently organized and can be a good starting point for when you need help.
 * aarch64 Programmers Guide: https://developer.arm.com/documentation/den0024/a
 * aarch64 Full Manual: https://yurichev.com/mirrors/ARMv8-A_Architecture_Reference_Manual_(Issue_A.a).pdf

So, the bad news is that the `gcc` we have on our computers isn't capable of producing aarch64 machine code; however, the good news is that cross-compilers exist. A cross compiler is a compiler that is capable of generating executable code for a platform other than the one on which the compiler is running. This allows developers to write code on one platform (such as a personal computer) and then build and run the code on another platform (such as a mobile device or embedded system). Cross compilers are commonly used in embedded systems and mobile development, where developers often work on a personal computer and then deploy the code to a target device that has limited resources and a different architecture. Cross compilers are also used for building for different architectures, for example, a developer may use a x86 host to build a binary for an ARM target, which is common in mobile and embedded systems.

Unfortunately for me, it probably took me 2-4 hours trying to find a cross compiler that would work. I was getting the compilers to work but the final kernel wouldn't boot on my Pi, it took lots of debugging to find out that the issue was with my compiler and not the code I was writing or the hardware itself.

If you are developing on Windows/Linux then you will need to read around and find a suitable cross compiler. For Mac users, I will detail the one I installed:

1. If you do not already have brew installed, visit this page and download it (https://brew.sh/). Brew is a really convenient package manager for Mac and this probably won't be your last time using it.
2. Execute `brew install aarch64-unknown-linux-gnu` at the command line. This will probably take a few minutes and produce a lot of output on your screen.
3. Execute `brew info aarch64-unknown-linux-gnu` after the download completes. In the output, you will see a path to where it was downloaded. In my case the path is `/usr/local/Cellar/aarch64-unknown-linux-gnu/11.2.0`. Save this path as we will need it to compile code.

Now that we have the cross-compiler installed, lets try compile a small program to make sure that everything appears to be working. I have made a new file called `hello.c` with the following source code.

```
// This code doesn't produce any output, we are more interested in seeing that it gets compiled to aarch64 instructions
int main() {
        int i = 0;
        while (i < 100) 
                ++i;
}
```

We can now use the new compiler we downloaded to convert this C program to an object file with the following command: `/usr/local/Cellar/aarch64-unknown-linux-gnu/11.2.0/bin/aarch64-unknown-linux-gnu-gcc hello.c -o hello.o -nostdlib`. You are probably familiar with the -o flag, but what is the other? Although not necessary, it will help with reducing the clutter of the output when we try to inspect the `hello.o` object file. The `-nostdlib` flag tells gcc to not link in the standard C library. Since we are not calling printf or including externel libraries, there is no need to link them in.

We can then look at the assembly instructions themselves by using the following command: `objdump -d hello.o`. If your compiler installed correctly, you should see something along the lines of this:

```
hello.o:        file format elf64-littleaarch64

Disassembly of section .text:

00000000004000e8 <main>:
  4000e8: ff 43 00 d1   sub     sp, sp, #16
  4000ec: ff 0f 00 b9   str     wzr, [sp, #12]
  4000f0: 04 00 00 14   b       0x400100 <main+0x18>
  4000f4: e0 0f 40 b9   ldr     w0, [sp, #12]
  4000f8: 00 04 00 11   add     w0, w0, #1
  4000fc: e0 0f 00 b9   str     w0, [sp, #12]
  400100: e0 0f 40 b9   ldr     w0, [sp, #12]
  400104: 1f 8c 01 71   cmp     w0, #99
  400108: 6d ff ff 54   b.le    0x4000f4 <main+0xc>
  40010c: 00 00 80 52   mov     w0, #0
  400110: ff 43 00 91   add     sp, sp, #16
  400114: c0 03 5f d6   ret
```

As you can see, there are infact aarch64 instructions and not x86 instructions! If you don't believe me, try compile the same program `cross-compiler-gcc hello.c` and then verify that executing `./a.out` fails since our x86 computer cannot make sense of this foreign assembly code.

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

