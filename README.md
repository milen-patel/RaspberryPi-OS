# RaspberryPi-OS
Building an Operating System for the RPI From Scratch!

# Task List
1. Create Makefile to correctly compile stand-alone kernel
2. In boot.S, disable all non-primary cores
3. Set up GPIO Pins, initialize the miniUART
4. Create interface for reading and writing to/from the UART
5. Implement printf


# Running Tasks
* Figure out why uart_recv and uart_send can't have their while loops optimized
* Add library function for reading uart line, uart_printf, etc.


# Some Explanations
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
