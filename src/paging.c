#include "paging/paging.h"
#include "paging/alloc.h"
#include "kprintf.h"
#include <stdbool.h>
#include <stdint.h>



static page_info paging_map_arr [NUM_PAGES] = {}; 
extern uint8_t __paging_start;

void init_paging() {
    kprintf("Given that a page size is %d bytes and there are %d free bytes, there are %d free pages\n", PAGE_SIZE, NUM_FREE_BYTES, NUM_PAGES);
    kprintf("The calculation for NUM_FREE_BYTES is %d-%d=%d\n", MMIO_BASE, START_OF_KERNEL_STACK_CPU2, NUM_FREE_BYTES);
    kprintf("Paging Start Address: %d\n", &__paging_start);

    // First page is reserved for CPU 2 Kernel Stack
    paging_map_arr[0].isAllocated = true;
}

void *alloc_page() {
    kprintf("Alloc page called....");
    for (int i = 0; i < NUM_PAGES; ++i) {
        if (paging_map_arr[i].isAllocated) {
            continue;
        }
        kprintf("giving page %d to the caller with page address %p\n", i, (&__paging_start + i*PAGE_SIZE));
        paging_map_arr[i].isAllocated = true;
        return (void *) &__paging_start + i*PAGE_SIZE;
    }
    return 0;
}


void free_page(void *page_adr) {
    int idx = (page_adr - (void *) &__paging_start) / PAGE_SIZE;
    kprintf("Free page called with page address %p, this seems to have been from index %d\n", page_adr, idx);
    paging_map_arr[idx].isAllocated = false;
}