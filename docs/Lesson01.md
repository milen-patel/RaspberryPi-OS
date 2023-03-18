---
layout: default
title: Lesson 01
parent: Build an Operating System
---
# **Lesson 01** - Understanding the Rasberry Pi Boot Sequence

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
