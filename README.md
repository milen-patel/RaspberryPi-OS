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