#ifndef	_MMIO_H
#define	_MMIO_H

#define MMIO_BASE 0x3F000000

// Define the MMIO addresses for the GPIO pins
#define GPFSEL1         (MMIO_BASE+0x00200004)
#define GPSET0          (MMIO_BASE+0x0020001C)
#define GPCLR0          (MMIO_BASE+0x00200028)
#define GPPUD           (MMIO_BASE+0x00200094)
#define GPPUDCLK0       (MMIO_BASE+0x00200098)

// Spins the CPU for the number of clock cycles specified in the parameter
// This must be implemented in assembly itself to avoid the compiler optimizing 
// the code out entirely.
extern void delay ( unsigned long);

// Stores to the address specified in the first parameter, a 32 bit value specified 
// in the second parameter
extern void put32 ( unsigned long, unsigned int );

// Reads a 32 bit value from the specified address
// Used for interacting with MMIO devices (along with put32 and delay)
extern unsigned int get32 ( unsigned long );

#endif  /*_MMIO_H */