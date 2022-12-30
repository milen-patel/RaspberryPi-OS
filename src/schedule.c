#include "process/pcb.h"
#include "libk/malloc.h"
#include "libk/printf.h"
#include "libk/memzero.h"
#include "paging/alloc.h"

struct pcb_list *runqueue;
struct pcb *currProc;
int numProcesses = 0;
int nextPID = 0;

struct pcb_list *alloc_listNode() {
    void *ptr = kmalloc(sizeof(struct pcb_list));
    kprintf("Allocated new pcb list node at address %p\n", ptr);
    memzero(ptr, sizeof(struct pcb_list));
    return ptr;
}

struct pcb *alloc_pcb() {
    void *ptr = kmalloc(sizeof(struct pcb));
    kprintf("Allocated new pcb entry at address %p\n", ptr);
    memzero(ptr, sizeof(struct pcb));
    return ptr;
}

int getNextPID() {
    numProcesses++;
    return nextPID++;
}

void init_scheduler() {
    runqueue = alloc_listNode();

    struct pcb *main_pcb = alloc_pcb();
    main_pcb->pid = getNextPID();
    main_pcb->priority = 2;
    main_pcb->state = STATE_RUNNING;

    runqueue->pcb = main_pcb;
    currProc = main_pcb;
}

int new_kernel_thread(void *function, void *arg) {
    kprintf("A request to create a new kernel thread has been made\n");
    // Get a stack for the thread
    void *stack = alloc_page();
    struct pcb *pcb = alloc_pcb();
    pcb->pid = getNextPID();
    pcb->stack = stack;
    pcb->priority = currProc->priority;
    pcb->numSlicesUsed = 0;
    pcb->state = STATE_READY;
    pcb->registers.stack_pointer = (uint64_t) stack;
    pcb->registers.program_counter = (uint64_t) function;
    pcb->registers.exception_link_register = (uint64_t) function;
    
    // Add it to the queue
    struct pcb_list *node = alloc_listNode();
    node->pcb = pcb;
    
    struct pcb_list *traversal_curr = runqueue;
    while (traversal_curr->next) {
        traversal_curr = traversal_curr->next;
    }
    traversal_curr->next = node;
    node->next = 0x0;

    // TODO needs to handle the arg
    kprintf("New thread has been created and added to PCB list with pid=%d\n", pcb->pid);
    return pcb->pid;
}

void schedule() {
    kprintf("Schedule has been called\n");
    currProc->numSlicesUsed++;
    kprintf("The current running process (%d) has elapsed %d slices\n", currProc->pid, currProc->numSlicesUsed);
    struct pcb_list *curr = runqueue;
    if (numProcesses == 1) {
        // TODO
    }
    runqueue = runqueue->next;
    curr->next = 0x0;

    struct pcb_list *entry;
    for (entry = runqueue; entry->next; entry = entry->next);
    entry->next = curr;

    kprintf("Process %d has been moved to the back of the list\n", curr->pcb->pid);
    currProc = runqueue->pcb;

    kprintf("The scheduler has decided to run process: %d\n", currProc->pid);



}