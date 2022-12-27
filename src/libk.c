#include "libk/malloc.h"
#include "paging/alloc.h"
#include "paging/paging.h"
#include "libk/assert.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <math.h>

#define SUPER_BLOCK_SIZE 4096
#define SUPER_BLOCK_MASK (~(SUPER_BLOCK_SIZE-1))
#define MIN_ALLOC 32 /* Smallest real allocation.  Round smaller mallocs up */
#define MAX_ALLOC 2048 /* Fail if anything bigger is attempted.
                        * Challenge: handle big allocations */
#define RESERVE_SUPERBLOCK_THRESHOLD 2


void assert(bool cond) {
    if (!(cond)) {
        printf("Bad assertion!\n");
        __asm__ __volatile__ ("b proc_hang");
    }
}

void assertWithLine(bool cond, int line) {
    if (!(cond)) {
        printf("Bad assertion from line %d\n!", line);
        __asm__ __volatile__ ("b proc_hang");
    }
}

/* Object: One return from malloc/input to free. */
struct __attribute__((packed)) object {
    union {
        struct object *next; // For free list (when not in use)
        char * raw; // Actual data
    };
};

/* Super block bookeeping; one per superblock.  "steal" the first
 * object to store this structure
 */
struct __attribute__((packed)) superblock_bookkeeping {
    struct superblock_bookkeeping * next; // next super block
    struct object *free_list;
    // Free count in this superblock
    uint8_t free_count; // Max objects per superblock is 128-1, so a byte is sufficient
    uint8_t level;
};

/* Superblock: a chunk of contiguous virtual memory.
 * Subdivide into allocations of same power-of-two size. */
struct __attribute__((packed)) superblock {
    struct superblock_bookkeeping bkeep;
    void *raw;  // Actual data here
};


/* The structure for one pool of superblocks.
 * One of these per power-of-two */
struct superblock_pool {
    struct superblock_bookkeeping *next;
    uint64_t free_objects; // Total number of free objects across all superblocks
    uint64_t whole_superblocks; // Superblocks with all entries free
};

// 10^5 -- 10^11 == 7 levels
#define LEVELS 7
static struct superblock_pool levels[LEVELS] = {{NULL, 0, 0}, // 2^5 = 32
                                                {NULL, 0, 0}, // 2^6 = 64
                                                {NULL, 0, 0}, // 2^7 = 128
                                                {NULL, 0, 0}, // 2^8 = 256
                                                {NULL, 0, 0}, // 2^9 = 512
                                                {NULL, 0, 0}, // 2^10= 1024
                                                {NULL, 0, 0}};// 2^11= 2048

static inline int myPower(int a, int b) {
    // Define variables to keep track of result and power we are currently on
    int res = a;
    int curr_power = 1;

    // Keep multiplying res by a until we reach the target power (b)
    while (curr_power++ < b) res *= a;

    // Return final result to caller
    return res;
}

/* Convert the size to the correct power of two.
 * Recall that the 0th entry in levels is really 2^5,
 * the second level represents 2^6, etc.
 *
 * Return the index to the appropriate level (0..6), or
 * -1 if the size is too large.
 */

static inline int size2level (ssize_t size) {
    // Round up any allocation less than or equal to 32 bytes to 32 bytes, since that is our minimum allocation size
    if (size <= MIN_ALLOC) return 0;
    // Edge case
    if (size > MAX_ALLOC) return -1;

    // Generic Case, keep doubling allocation size (and incrementing table index)
    // until the requested allocation size can fit within the size of entries on
    // the current table row
    int currSize = MIN_ALLOC;
    int idx = 0; 
    while (size > currSize) {
        currSize *= 2;
        idx++;
    }
    return idx;
}

/* This function allocates and initializes a new superblock.
 *
 * Note that a superblock in this lab is only one 4KiB page, not
 * 8 KiB, as in the hoard paper.
 *
 * This design sacrifices the first entry in every superblock
 * to store a superblock_bookkeeping structure.  Yes,
 * it is a bit wasteful, but let's keep the exercise simple.
 *
 * power: the power of two to store in this superblock.  Note that
 *        this is offset by 5; so a power zero means 2^5.
 *
 * Return value: a struct superblock_bookkeeping, which is
 * embedded at the start of the superblock.  Or NULL on failure.
 */
static inline
struct superblock_bookkeeping * alloc_super (int power) {
    void *page;
    struct superblock* sb;
    int free_objects = 0, bytes_per_object = 0;
    char *cursor;
    // Your code here
    // Allocate a page of anonymous memory
    // WARNING: DO NOT use brk---use mmap, lest you face untold suffering
    // Request a page
    page = alloc_page();

    // Handle bad result from mmap()
    if (page == 0x0) {
        printf("Internel Error in KMalloc()");
        assert(page);
    }

    // Cast our page to a superblock struct
    sb = (struct superblock*) page;

    // Put this one the list. Prepend to Linked List.
    sb->bkeep.next = levels[power].next;
    levels[power].next = &sb->bkeep;

    // This entire superblock is free, so record it
    levels[power].whole_superblocks++;

    // Set remaining fields in struct
    sb->bkeep.level = power;
    sb->bkeep.free_list = NULL;

    // Your code here: Calculate and fill the number of free objects in this superblock
    
    // There are 7 levels in the superblock_pool (where level 0 consists of 32-bit chunks), so to get the true power of 2, do power + 5 and then 2^truePower to get the bytes_per_objects
    int truePower = power + 5;
    bytes_per_object = myPower(2, truePower);

    // The total number of objects is the (super block size - object size) / object size (need to subtract object size from super block size for the first object which is reserved for bookkeeping)
    int bytes_in_superblock = SUPER_BLOCK_SIZE;
    bytes_in_superblock -= bytes_per_object; // for the bookkeeping entry
    free_objects = ((int) (bytes_in_superblock / bytes_per_object));
    //  Be sure to add this many objects to levels[power]->free_objects, reserving
    //  the first one for the bookkeeping.
    levels[power].free_objects += free_objects;
    sb->bkeep.free_count = free_objects;
    // Be sure to set free_objects and bytes_per_object to non-zero values.

    // The following loop populates the free list with some atrocious
    // pointer math.  You should not need to change this, provided that you
    // correctly calculate free_objects.

    cursor = (char *) sb;
    // skip the first object
    for (cursor += bytes_per_object; free_objects--; cursor += bytes_per_object) {
        // Place the object on the free list
        struct object* tmp = (struct object *) cursor;
        tmp->next = sb->bkeep.free_list;
        sb->bkeep.free_list = tmp;
    }
    return &sb->bkeep;
}

void *kmalloc(size_t size) {
    struct superblock_pool *pool;
    struct superblock_bookkeeping *bkeep;
    void *rv = NULL;
    int power = size2level(size);

    // Handle bigger allocations with mmap, and a simple list
    if (size > MAX_ALLOC) {
        printf("Cannot alloc with a size this large!\n");
        return 0x0;
    }


    pool = &levels[power];

    if (!pool->free_objects) {
        bkeep = alloc_super(power);
    } else {
        bkeep = pool->next;
    }

    for ( ; bkeep != NULL; bkeep = bkeep->next) {
        if (bkeep->free_count) {
            struct object *cursor = bkeep->free_list;
            /* Remove an object from the free list. */
            //
            // NB: If you take the first object out of a whole
            //     superblock, decrement levels[power]->whole_superblocks

            // Move to next object in free list 
            rv = cursor;
            bkeep->free_list = cursor->next;

            // We are using a new superblock, so decrement size of whole_superblocks
            if (bkeep->free_count + 1 == (SUPER_BLOCK_SIZE/ ((int) myPower(2,(power + 5))))) {
                pool->whole_superblocks--;
            }

            // Update the book-keeping information in the superblock_bookkeeping
            pool->free_objects--;
            bkeep->free_count--;

            break;
        }
    }

    // assert that rv doesn't end up being NULL at this point
    if (rv == NULL) {
        printf("Something went wrong with kmalloc!\n");
        assert(rv != NULL);
    }

    return rv;
}

static inline
struct superblock_bookkeeping * obj2bkeep (void *ptr) {
    uint64_t addr = (uint64_t) ptr;
    addr &= SUPER_BLOCK_MASK;
    return (struct superblock_bookkeeping *) addr;
}

void kfree(void *ptr) {
    // Just ignore free of a null ptr
    if (ptr == NULL) return;
    
    struct superblock_bookkeeping *bkeep = obj2bkeep(ptr);
    int power = bkeep->level;

    // Update number of free objects
    bkeep->free_count++;

    // Append free block into the list again
    struct object *obj = (struct object *) ptr;
    obj->next = bkeep->free_list;
    bkeep->free_list = obj;

    // Update superblock level  information
    levels[bkeep->level].free_objects++;

    // If whole superblock is available, increment the total
    if ((bkeep->free_count + 1) == (SUPER_BLOCK_SIZE/myPower(2, 5 + bkeep->level))) {
        levels[bkeep->level].whole_superblocks++;
    }
   

    while (levels[power].whole_superblocks > RESERVE_SUPERBLOCK_THRESHOLD) {
        // Remove a whole superblock from the level
        // Return that superblock to the kernel

        // Decrement whole_superblocks for specific level of the superblock pool
        levels[power].whole_superblocks--;

        // Okay, first we have to find the superblock which is entirely free
        // Set the pointer to the first superblock in the specific level of the superblock pool
        // Determine number of items a superblock needs to have in order to be considered free
        struct superblock_bookkeeping *ptr = levels[power].next;
        int numItemsToBeFullyFree = SUPER_BLOCK_SIZE/myPower(2, power + 5) - 1;

        // Is it the first item in the list? If it is the first item in the list, unmap that superblock and set levels[power].next to the next available superblock
        // Else, go through all the superblocks and find the first superblock which is eligible to be freed
        if (ptr->free_count == numItemsToBeFullyFree) {
            struct superblock_bookkeeping *copy = ptr;
            levels[power].next = ptr->next;
            free_page(copy);
        } else {
             // Okay, somehwere else in the list
            struct superblock_bookkeeping *prev = ptr;
            ptr = ptr->next;
            while (ptr->free_count != numItemsToBeFullyFree) {
                ptr = ptr->next;
                prev = prev->next;
            }
            prev->next = ptr->next;
            free_page(ptr);
        }
    }
}

void kmalloc_test() {
    void *prev = NULL;
    for (int i = 0; i < 2500; ++i) {
        void *new = kmalloc(64);
        if (i == 0) prev = new+64;
        if (prev - new != 64) 
            printf("There should have been a call to alloc_page()");
        prev = new;
    }

    void *a = kmalloc(32);
    void *b = kmalloc(32);
    void *c = kmalloc(32);
    void *d = kmalloc(32);
    void *e = kmalloc(32);
    void *f = kmalloc(32);
    void *g = kmalloc(32);
    void *h = kmalloc(32);
    void *i = kmalloc(32);
    void *j = kmalloc(32);
    void *k = kmalloc(32);
    kfree(a);
    assert(a == kmalloc(32));
    a = kmalloc(32);
    kfree(k);
    assert(k == kmalloc(32));
    k = kmalloc(32);
    kfree(b);
    kfree(c);
    kfree(d);
    kfree(e);
    kfree(f);
    kfree(g);
    kfree(h);
    kfree(i);
    kfree(j);
    assertWithLine(j == kmalloc(32), __LINE__);
    assertWithLine(i == kmalloc(32), __LINE__);
    assertWithLine(h == kmalloc(32), __LINE__);
    assertWithLine(g == kmalloc(32), __LINE__);
    assertWithLine(f == kmalloc(32), __LINE__);
    assertWithLine(e == kmalloc(32), __LINE__);
    assertWithLine(d == kmalloc(32), __LINE__);
    assertWithLine(c == kmalloc(32), __LINE__);
    assertWithLine(b == kmalloc(32), __LINE__);
    assertWithLine(kmalloc(64) - kmalloc(64) == 64, __LINE__);
    assertWithLine(kmalloc(128) - kmalloc(128) == 128, __LINE__);
    assertWithLine(kmalloc(256) - kmalloc(256) == 256, __LINE__);
    assertWithLine(kmalloc(512) - kmalloc(512) == 512, __LINE__);
    assertWithLine(kmalloc(1024) - kmalloc(1024) == 1024, __LINE__);
    assertWithLine(kmalloc(2048) - kmalloc(2048) == -PAGE_SIZE, __LINE__);
}