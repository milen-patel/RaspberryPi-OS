#ifndef	_UART_H
#define	_UART_H

#include "mmio.h"
#include "sys-registers/uart.h"


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