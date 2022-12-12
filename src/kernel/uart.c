#include <uart.h>
#include <stddef.h>
#include <stdint.h>

/*==========Used for Interacting with Memory Mapped I/o Devices===============*/

static inline void mmio_write(uint32_t reg, uint32_t data)
{
	// Explanation of the volatile keyword: https://barrgroup.com/embedded-systems/how-to/c-volatile-keyword
    *(volatile uint32_t*)reg = data;
}

static inline uint32_t mmio_read(uint32_t reg)
{
    return *(volatile uint32_t*)reg;
}

/*==========Used for Interacting with Memory Mapped I/o Devices===============*/

// Loop <delay> times in a way that the compiler won't optimize away
static inline void delay(int32_t count)
{
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
            : "=r"(count): [count]"0"(count) : "cc");
}

void uart_init()
{
	// Disable the UART
    mmio_write(UART0_CR, 0x00000000);

	// Indicate tht certain GPIO pins should be disabled
    mmio_write(GPPUD, 0x00000000);
    delay(150);

	// Now, specify that pins 14 and 15 should be disabled
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);

	// Makes the changes take effect
    mmio_write(GPPUDCLK0, 0x00000000);

	// Clear all the UART interrupts
    mmio_write(UART0_ICR, 0x7FF);

	// Set the baud rate
    mmio_write(UART0_IBRD, 1);
    mmio_write(UART0_FBRD, 40);

	// See page 184, set to FIFO mode with 8 bit transmission words
    mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

	// Disable UART interrupts
    mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
            (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));

	// Now, enable UART
    mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

void uart_putc(unsigned char c)
{
	// Spin while the transmit FIFO is full 
    while ( mmio_read(UART0_FR) & (1 << 5) ) { }
	// Write to the register, char in c is always 1 byte (8 bits) so this works nicely
    mmio_write(UART0_DR, c);
}

unsigned char uart_getc()
{
	// Bit 4 is set if and only if there is nothing to read
    while ( mmio_read(UART0_FR) & (1 << 4) ) { }
    return mmio_read(UART0_DR);
}

// Wrapper function to making printing strings convenient
void uart_puts(const char* str)
{
    for (size_t i = 0; str[i] != '\0'; i ++)
        uart_putc((unsigned char)str[i]);
}