#ifndef _MEMORY_MANAGER_H
#define _MEMORY_MANAGER_H

#include "paging/paging.h"
// Used in boot.S to give the kernel 4 MB of stack space
// This should be enough for anything I am working on
#define START_OF_KERNEL_STACK          (1 << 22)
#define START_OF_KERNEL_STACK_CPU2     (START_OF_KERNEL_STACK + PAGE_SIZE)

#endif