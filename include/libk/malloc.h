#ifndef _K_MALLOC_H
#define _K_MALLOC_H

#include <stddef.h>

void *kmalloc(size_t size);
void kfree(void *ptr);
void kmalloc_test();

#endif