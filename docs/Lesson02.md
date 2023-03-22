---
layout: default
title: Lesson 02
parent: Build an Operating System
---
# **Lesson 02** - Cross Compiling from x86 to ARMv8

I am working on an Intel Mac which natively uses Intel's x86 CPU architecture. One implication is that when I compile any C or assembly code using `gcc` or `clang`, the outputted binary contains x86 instructions. This is not going to work for our project because the Raspberry Pi uses an entirely different instruction set, ARMv8. Here are some key differences between the two:

 * Architecture: ARM is a Reduced Instruction Set Computer (RISC) architecture, while x86 is a Complex Instruction Set Computer (CISC) architecture. RISC architectures have a smaller number of instructions, which are simpler and easier to execute, while CISC architectures have a larger number of instructions, which can be more complex and powerful.
 * Register size: ARM processors have 32-bit registers, while x86 processors have either 32-bit or 64-bit registers. This means that x86 processors can handle more data in a single instruction, but ARM processors can use more registers in parallel.
 * Instruction set: The instruction sets of ARM and x86 are not compatible, which means that code written for one architecture cannot be executed on the other without some form of translation.
 * Power consumption: ARM processors are known for their low power consumption, making them well suited for mobile and embedded devices, while x86 processors are more powerful and tend to consume more power.
 * Market share: x86 processors are widely used in personal computers and servers, while ARM processors are widely used in mobile devices, embedded systems and IoT devices.

There are multiple versions of ARM, but the Raspberry Pi specifically uses ARMv8 which is synonymous with aarch64. This is the link for the documentation on aarch64. This website is incredibly useful and you should spend some time learning about the instruction set. The website is not exhaustive and infact covers only a subset of topics at a high level. If you are interested in seeing the full manual, that is also linked below. Both of these will become invaluable and although the manual contains everything on the website, I often find that the website is more coherently organized and can be a good starting point for when you need help.
 * [aarch64 Programmers Guide](https://developer.arm.com/documentation/den0024/a)
 * [aarch64 Full Manual](https://yurichev.com/mirrors/ARMv8-A_Architecture_Reference_Manual_(Issue_A.a).pdf)

So, the bad news is that the `gcc` we have on our computers isn't capable of producing aarch64 machine code; however, the good news is that cross-compilers exist. A cross compiler is a compiler that is capable of generating executable code for a platform other than the one on which the compiler is running. This allows developers to write code on one platform (such as a personal computer) and then build and run the code on another platform (such as a mobile device or embedded system). Cross compilers are commonly used in embedded systems and mobile development, where developers often work on a personal computer and then deploy the code to a target device that has limited resources and a different architecture. Cross compilers are also used for building for different architectures, for example, a developer may use a x86 host to build a binary for an ARM target, which is common in mobile and embedded systems.

Unfortunately for me, it probably took me 2-4 hours trying to find a cross compiler that would work. I was getting the compilers to work but the final kernel wouldn't boot on my Pi, it took lots of debugging to find out that the issue was with my compiler and not the code I was writing or the hardware itself.

If you are developing on Windows/Linux then you will need to read around and find a suitable cross compiler. For Mac users, I will detail the one I installed:

1. If you do not already have brew installed, visit [this page](https://brew.sh/) and download it. Brew is a really convenient package manager for Mac and this probably won't be your last time using it.
2. Execute `brew install aarch64-unknown-linux-gnu` at the command line. This will probably take a few minutes and produce a lot of output on your screen.
3. Execute `brew info aarch64-unknown-linux-gnu` after the download completes. In the output, you will see a path to where it was downloaded. In my case the path is `/usr/local/Cellar/aarch64-unknown-linux-gnu/11.2.0`. Save this path as we will need it to compile code.

Now that we have the cross-compiler installed, lets try compile a small program to make sure that everything appears to be working. I have made a new file called `hello.c` with the following source code.

```c
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