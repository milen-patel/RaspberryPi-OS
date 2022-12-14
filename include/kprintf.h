#ifndef _PRINTF_H
#define _PRINTF_H

/* 
 * A simple printf implementation.
 *
 * Currently supports pattern matching for:
 *   char (%c)
 *   int (%d)
 *   strings (%s)
 *   pointers (%p)
 */
void kprintf(char *s, ...);

/* 
 * Simple function that demonstrates example usage of printf to verify behaviour
 */
void test_printf();

#endif