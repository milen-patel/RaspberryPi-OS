#ifndef	_UART_H
#define	_UART_H

#include "mmio.h"

// Addresses of various UART MMIO Registers
// Specified in Page 8 of the BBM2837 Manual
#define AUX_ENABLES     (MMIO_BASE+0x00215004)
#define AUX_MU_IO_REG   (MMIO_BASE+0x00215040)
#define AUX_MU_IER_REG  (MMIO_BASE+0x00215044)
#define AUX_MU_IIR_REG  (MMIO_BASE+0x00215048)
#define AUX_MU_LCR_REG  (MMIO_BASE+0x0021504C)
#define AUX_MU_MCR_REG  (MMIO_BASE+0x00215050)
#define AUX_MU_LSR_REG  (MMIO_BASE+0x00215054)
#define AUX_MU_MSR_REG  (MMIO_BASE+0x00215058)
#define AUX_MU_SCRATCH  (MMIO_BASE+0x0021505C)
#define AUX_MU_CNTL_REG (MMIO_BASE+0x00215060)
#define AUX_MU_STAT_REG (MMIO_BASE+0x00215064)
#define AUX_MU_BAUD_REG (MMIO_BASE+0x00215068)

// The following functions are to be used to interact with the miniUART
// You shouldn't rely on using these functions as printf() is clean wrapper around them

// Setup the uart for read/write, this must be called before any call to uart_recv or uart_send
void uart_init ( void );

// Reads one character from the UART
char uart_recv ( void );

// Sends one character to the UART
void uart_send ( char c );

// Sends a null-terminated string to the UART. This is a wrapper around uart_send
// It is imperative that the end string is null terminated ('\0')
void uart_send_string(char* str);

#endif  /*_UART_H */