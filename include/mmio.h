#ifndef	_MMIO_H
#define	_MMIO_H

#include "sys-registers/mmio_base.h"
#include "sys-registers/gpio.h"


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