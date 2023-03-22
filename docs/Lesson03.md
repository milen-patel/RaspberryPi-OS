---
layout: default
title: Makefiles
parent: Build an Operating System
nav_order: 4
---
# **Lesson 03** - Makefiles

In this lesson, I'm going to divert slightly from the script but with well intentions. You're probably reading this to learn how to build an OS, but it would be wrong for me to skip over the essential tools that I used to write my code. The goal for these tutorials is to be able to follow along from scratch, so I feel this tutorial is necessary.

Although we haven't started writing the source code for our kernel, when we do, it will get incredibly messy incredibly fast. For that reason, we will be splitting our project into two different directories. 
* `/src` will contain all of the source code files - this includes both C (.c) and assembly (.S) files.
* `/include` will contain all of our header files (.h). Header files are going to be very useful as we split our project into multiple files. I am assuming that you have enough knowledge of the C language to deal with these. If you don't, there are plenty of materials freely available online, so don't stress!
* `/build` If we have 100 source files, then using gcc, we will have to compile each of these source files into a corresponding object (.o) file before we can link them all together. If we didn't group all of these object files into a folder together, our project would get really messy. For that exact reason, we are going to create an empty build directory. Every time we need to recompile the project, we will compile all of the object files and instruct gcc to store them in our build directory. Then, when it becomes time to link them together, we can ignore the mess inside this directory and simply examine the singular kernel file which will be the result of linking all of the object files together.

I will follow these directory conventions for the remainder of the project, with very little deviation. Although not necessary, these directories will help increase the readability of our directory and this is generally considered good programming practice. 

Previously, I mentioned that our project will quickly grow both in complexity and in number of files. Every time we want to recompile our project, it would be tedious to have to recompile each source file and then link them together. Fortunately, [GNU's Make](https://www.gnu.org/software/make/manual/make.html) is a powerful solution to this problem. We will have to create one Makefile, and once we have it correctly initialized, we will always be able to recompile our entire kernel using one command, regardless of the number of source/header files!

Lets look at an example of a Makefile:

```
CC=gcc
CFLAGS=-Wall -g

all: my_program

my_program: main.c helper.c
	$(CC) $(CFLAGS) -o my_program main.c helper.c

clean:
	rm -f my_program
```

This Makefile has three targets:

* `all` is the default target, which is built when no target is specified. In this case, it builds my_program.
* `my_program` is the main target. It depends on main.c and helper.c, which are the source files. The Makefile uses the $(CC) and $(CFLAGS) variables to specify the compiler and its flags. Finally, it creates an executable binary named my_program.
* `clean` is a utility target that removes the binary my_program.

To compile a program with make, you can simple execute `make (rule)` in your command line. So to compile our C program (which is composed of two C files), we can execute `make all`. And to delete the program, we can execute `make clean`.

If you looked at the Make documentation I linked earlier in this tutorial, you can see that there are countless features and capabilities for this tool. Since Make can be used with any type of software project, the majority of its features will be irrelevant for our work. So, lets look at the Makefile we will use for all future lessons

```
# Don't use normal gcc, use the arm cross compiler from Lesson 02
ARMGNU = /usr/local/Cellar/aarch64-unknown-linux-gnu/11.2.0/bin/aarch64-unknown-linux-gnu

COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only -g 
ASMOPS = -Iinclude -g

BUILD_DIR = build
SRC_DIR = src

all : clean kernel8.img

clean :
	rm -rf $(BUILD_DIR) *.img 
	rm -f binary.txt

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img

binary: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES) -g 
	objdump -d --source build/kernel8.elf >binary.txt
	open binary.txt
```

I would like to give credit for this Makefile to [this](https://github.com/s-matyukevich/raspberry-pi-os) source. I have modified it slightly and added my own targets but they deserve the majority of the credit.

Lets look at what this Makefile does line-by-line:

0. `ARMGNU` variable specifies the path to the ARM cross-compiler.
1. `COPS` variable specifies the compiler options that will be passed to the C compiler.
2. `ASMOPS` variable specifies the assembler options that will be passed to the assembler.
3. `BUILD_DIR` variable specifies the directory where the build artifacts will be stored.
4. `SRC_DIR` variable specifies the directory where the source files are located.
5. The `all` target depends on `clean` and `kernel8.img` targets. The clean target removes the build artifacts and kernel8.img target creates the kernel image.
6. The `clean` target removes the build directory, kernel image, and binary.txt file.
7. The following two rules define how to compile C and assembly source files into object files. These rules use the gcc compiler to compile C and assembly source files into object files with -MMD option that will create dependency files, which are used to track dependencies between source files.
```
$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
    mkdir -p $(@D)
    $(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
    mkdir -p $(@D)
    $(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@
```

8. `C_FILES`, `ASM_FILES`, and `OBJ_FILES` variables are defined using the wildcard function to automatically find all C and assembly source files and object files. `DEP_FILES` variable contains the corresponding dependency files.
9. `-include $(DEP_FILES)` is a special directive that includes the dependency files into the Makefile.
10. The kernel8.img target depends on the linker script $(SRC_DIR)/linker.ld and all object files. 
11. The `ld` command links the object files into an ELF file and objcopy command creates a binary image that can be loaded onto the Raspberry Pi's SD card.
12. The `binary` target is used to generate a binary dump of the kernel image and opens it in a text editor. This target uses objdump to generate a dump of the kernel image and open command to open the generated binary.txt file. This will be very useful for debugging once we reach the tutorial on exception since exceptions can indicate the line number for the code which generated an exception (in the case of an exception being generated by problematic code).

Lastly, I wanted to explain what some of the flags we are passing to gcc mean:
1. -Wall : This flag enables all warning messages that the compiler can generate. It is generally good practice to use this flag while developing code as it can help you catch potential issues and bugs.
2. -nostdlib : This flag tells the compiler not to use the standard system libraries during the linking stage. This is typically used when building embedded systems or other specialized applications that do not require the standard libraries.
3. -nostartfiles : This flag tells the compiler not to use the standard startup files during the linking stage. These startup files are typically used to set up the program's execution environment, and are not required in all cases.
4. -ffreestanding : This flag tells the compiler that the resulting executable will not run in a full-featured operating system environment, and that certain assumptions about the environment can be made. This flag is typically used when building code for embedded systems or other specialized environments.
5. -Iinclude : This flag adds the include directory to the list of directories that the compiler searches for header files.
6. -mgeneral-regs-only : This flag tells the compiler to generate code that only uses general-purpose registers, and to avoid using any other CPU-specific features. This is useful when building code that is intended to run on a variety of different hardware platforms.
7. -g : This flag tells the compiler to include debug information in the resulting executable. This can be useful when debugging code, as it allows you to step through the program's execution and inspect the values of variables and other program state. This will be used for the binary target in our Makefile.

If not everything makes sense to you, that is totally fine. Understanding Makefiles is largely irrelelvant to the subsequent tutorials, but is something you will likely encounter later in your career as a developer, so I thought they were worth discussing in a separate chapter. The only thing left to answer, is where do we put our Makefile? For the sake of our project, we will let it sit at the top level directory, so our final project structure will look something like this:
```
.
├── Makefile
├── build
├── include
└── src
```

If you feel lost, your one takeaway from this tutorial should be that you can compile the OS at any point by executing `make`, regardless of the number of files.