#ifndef _PAGING_ALLOC_H
#define _PAGING_ALLOC_H

#include <stdbool.h>

// Contains the bookkeeping information we need to know about every page in RAM
typedef struct {
    bool isAllocated;
} page_info;

void init_paging();

// Allocates a physical page from RAM and returns a pointer to the base address or 0x0 if no more pages are free
void *alloc_page();

// Yields a page back to the kernel from a process by passing the base address of the page (i.e. the exact ptr given from alloc_page)
void free_page(void *page_adr);

#endif