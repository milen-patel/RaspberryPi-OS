#include "libk/printf.h"
#include "libk/log.h"
#include "uart.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

bool LOGGING_ENABLED = true;

// Prints a digit as a string
void print_digits(unsigned long n) {
  if (n < 0) {
    uart_send('-');
    n = -n;
  }
  if (n >= 10) {
    print_digits(n / 10);
  }
  uart_send(n % 10 + '0');
}

// See: https://publications.gbdirect.co.uk//c_book/chapter9/stdarg.html (For handling an unknown number of variables)

void kwriter(char *str, va_list ap) {

    bool isNextSpecial = false;
    while (*str != '\0') {
        if (*str == '%') {
            isNextSpecial = true;
            str++;
            continue;
        }
        if (isNextSpecial) {
            isNextSpecial = false;
            if (*str == 'd') {
                long val = va_arg(ap, long);
                print_digits(val);
            } else if (*str == 'c') {
                char c = (char) va_arg(ap, int);
                uart_send(c);
            } else if (*str == 's') {
                char *s = va_arg(ap, char *);
                uart_send_string(s);   
            } else if (*str == 'p') {
                void *ptr = va_arg(ap, void *);
                print_digits((int64_t) ptr);
            } else {
                uart_send_string("ERROR: Bad formatting sent to printf...\n");
                return;
            }
            str++;
            continue;
        }
        uart_send(*str);
        str++;
    }

}

void kprintf(char *str, ...) {
    va_list ap;
    va_start(ap, str);
    kwriter(str, ap);
}

void klog(char *str, ...) {
    if (!LOGGING_ENABLED) return;
    va_list ap;
    va_start(ap, str);
    kwriter(str, ap);
}

void test_printf() {
  kprintf("Simple String\n");
  kprintf("This is a digit: %d\n", 5);
  kprintf("This is two digits: %d and %d\n", 6, 7);
  kprintf("This is a negative digit: %d\n", -6);
  kprintf("This is a wide digit:%d\n", 123456789);
  kprintf("This is a char: %c and this is a digit: %d\n", 'c', 34);
  kprintf("This is a string: %s\n", "...sample string...");

  int a = 4;
  int b = 5;
  kprintf("&a=%p\n&b=%p\n", &a, &b);
  kprintf("Debug Line...\n");
}