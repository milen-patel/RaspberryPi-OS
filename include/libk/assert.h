#ifndef _ASSERT_H
#define _ASSERT_H

#include <stdbool.h>

// Asserts condition cond is true, if not, CPU core is killed (i.e. put into a spin loop)
void assert(bool cond);
void assertWithLine(bool cond, int line);

#endif