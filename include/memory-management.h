#ifndef _MEMORY_MANAGER_H
#define _MEMORY_MANAGER_H

// Used in boot.S to give the kernel 4 MB of stack space
// This should be enough for anything I am working on
#define START_OF_KERNEL_STACK          (1 << 22)

#endif