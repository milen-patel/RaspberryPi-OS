#ifndef _SYSR_GPIO_H
#define _SYSR_GPIO_H

// Define the MMIO addresses for the GPIO pins
#define GPFSEL1         (MMIO_BASE+0x00200004)
#define GPSET0          (MMIO_BASE+0x0020001C)
#define GPCLR0          (MMIO_BASE+0x00200028)
#define GPPUD           (MMIO_BASE+0x00200094)
#define GPPUDCLK0       (MMIO_BASE+0x00200098)

#endif