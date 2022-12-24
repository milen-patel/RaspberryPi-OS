#ifndef _PAGING_ALLOC_H
#define _PAGING_ALLOC_H

#include <stdbool.h>

typedef struct {
    bool isAllocated;
} page_info;

void init_paging();
void *alloc_page();
void free_page(void *page_adr);

#endif