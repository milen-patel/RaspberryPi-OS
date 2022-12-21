#ifndef	_MMIO_H
#define	_MMIO_H

#define MMIO_BASE 0x3F000000

// Define the MMIO addresses for the GPIO pins
#define GPFSEL1         (MMIO_BASE+0x00200004)
#define GPSET0          (MMIO_BASE+0x0020001C)
#define GPCLR0          (MMIO_BASE+0x00200028)
#define GPPUD           (MMIO_BASE+0x00200094)
#define GPPUDCLK0       (MMIO_BASE+0x00200098)

extern void delay ( unsigned long);
extern void put32 ( unsigned long, unsigned int );
extern unsigned int get32 ( unsigned long );

#endif  /*_MMIO_H */