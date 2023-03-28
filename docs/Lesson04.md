---
layout: default
title: Booting the Raspberry Pi
parent: Build an Operating System
nav_order: 5
---
# **Lesson 04** - Booting the Raspberry Pi

By this point, we have motivated the context for this project and explained what happens from the moment you plug in your Raspberry Pi to the time at which `kernel8.img` is loaded from the SD Card into memory and given execution. The next logical step is to go ahead and build a simple kernel and get it running on our device. It's worth noting, that for the first number of tutorials, we will be building out fundamental building blocks of our operating system before we even begin exploring various OS concepts.

You are probably used to writing code in C (or likely an even higher level language) but we begin our journey with good old assembly. In this tutorial, we will eventually initialize the C runtime environment and make the transition to C code, but the initial lines of the kernel must be written in aarch64 assembly.

I will create a new file called `boot.S` whose contents appear as such:

```
.global _start

_start:
	wfe
 	b _start
```

This trivial program, if compiled, would run on the Rasberry Pi and serve as a minimal (yet useless) kernel. So, what exactly is this doing? The first line is is used to define a symbol that is visible to the linker. In this case, the symbol being defined is `_start`, which is typically the entry point for an AArch64 program (which is precisely how we are using it here).

The `_start` label defines the beginning of the code section and tells the assembler that this is where program execution should begin. The next instructions in the code section are executed sequentially until the program terminates.

In this specific code snippet, the `wfe` instruction stands for "wait for event" and is a processor hint instruction. It causes the processor to enter a low-power state and wait for an event to occur, such as an interrupt or an exception.

The `b _start` instruction is a branch instruction that redirects program execution back to the _start label. This creates an infinite loop, where the processor waits for an event to occur, and then immediately returns to the beginning of the code section to wait again. This is commonly used as a placeholder in embedded programs where the main loop is triggered by an interrupt or some other external event. In our case, it serves absolutely no use. Surely enough, however, if we were to compile this and put it into our Rasberry Pi, things would be happening.

Clearly, we are at a bottle neck, so now would be a convenient time to figure out how to jump to C code so we can do something like `printf("Hello OS\n")`. Fortunately, there is little work required to get out of assmebly: All we need to do is clear the BSS section. But what is this?

The BSS (Block Started by Symbol) section is a portion of the data segment in a program's memory layout that contains uninitialized static variables and global variables that are initialized to zero. The BSS section is set up by the program loader at runtime to ensure that these variables are initialized to zero before the program starts executing.

When jumping from assembly code to C code, it is essential to ensure that the BSS section is properly initialized to zero before the C code begins executing. This is because C code often relies on these variables being initialized to zero, and if they are not properly initialized, the program may behave unexpectedly or crash.

This can be done in C code using a static initialization or by explicitly setting the BSS section to zero in assembly code.

We will add the following lines of code to our boot file:
```
    // Clean the BSS section, expected by C runtime environment
    ldr     x1, =__bss_start     // Start address
    ldr     w2, =__bss_size      // Size of the section

	// Compare and branch on 0
3:  cbz     w2, 4f               
	// Store 0 (xzr is the zero register) to address in x1 in memory
    str     xzr, [x1], #8

	// Decrease remaining bss size needed to be cleared by 1
    sub     w2, w2, #1
    cbnz    w2, 3b               // Loop if non-zero
```
The code starts by loading the start address of the BSS section into register x1 and the size of the section into register w2. The program then enters a loop, labeled as 3, that will continue until all the memory in the BSS section has been cleared.

Inside the loop, the code uses the cbz instruction to check whether the remaining BSS size, stored in w2, is zero. If w2 is zero, the program jumps to the label 4, which is presumably where the program's main function begins. Otherwise, the program stores zero to the address stored in x1, which points to the current memory location in the BSS section, using the str instruction. It then increments the value in x1 by eight bytes to move to the next memory location in the BSS section.

Finally, the program decrements w2 by one to indicate that one more memory location has been cleared, and uses the cbnz instruction to loop back to the label 3 if w2 is still non-zero. If w2 is zero, the program continues execution at label 4.

You may have on question: where do the `__bss_start` and `__bss_size` constants come from. These will come from our linker file, but more will come on that shortly.

Now that we have cleared the BSS section, we are ready to jump to our c code, so I will add one final line to the boot file which does so:
```
bl main
```

We then create a new file, `kernel.c`, which provides an implementation for the aforementioned main method:
```c
void main() {
    // ???
}
```

We would like to add a call to `printf` in our main method, but we have no standard library! Since we are building the operating system, we must define our own implementation of printf (and all other C library functions that we require). But, we don't even have a means of printing to a screen yet! Read on to see how we solve this issue