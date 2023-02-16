#include "libk/memcpy.h"
#include <stddef.h>

void memcpy(void *src, void *dest, size_t num_bytes) {
    char *destPtr = (char *) dest;
    char *srcPtr = (char *) src;
    for (int i = 0; i < num_bytes; ++i)
        destPtr[i]=srcPtr[i];
}