#ifndef _PAGING_H
#define _PAGING_H

#include "sys-registers/mmio_base.h"
#include "memory-management.h"
#define PAGE_SIZE 4096

#define NUM_FREE_BYTES (MMIO_BASE - START_OF_KERNEL_STACK_CPU2)
#define NUM_PAGES (NUM_FREE_BYTES / 4096)

#endif

