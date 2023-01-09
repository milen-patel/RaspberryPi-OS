#include "process/pcb.h"
#include "libk/malloc.h"
#include "libk/printf.h"
#include "libk/memzero.h"
#include "sys-registers/spsr.h"
#include "paging/alloc.h"
#include "paging/paging.h"

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
    pcb->registers.stack_pointer = (long) stack + PAGE_SIZE;
    pcb->registers.x30 = (long) function;
    pcb->registers.exception_link_register = (long) function;
    pcb->registers.saved_program_status_register = RETURN_TO_EL1 | DISABLE_FIQ;
    
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
    print_pcb_state(pcb);
    return pcb->pid;
}

void do_switch_register_state() {
    long *base = (long *) 4210688;
    *(base + 0) = currProc->registers.x0;
    *(base + 1) = currProc->registers.x1;
    *(base + 2) = currProc->registers.x2;
    *(base + 3) = currProc->registers.x3;
    *(base + 4) = currProc->registers.x4;
    *(base + 5) = currProc->registers.x5;
    *(base + 6) = currProc->registers.x6;
    *(base + 7) = currProc->registers.x7;
    *(base + 8) = currProc->registers.x8;
    *(base + 9) = currProc->registers.x9;
    *(base + 10) = currProc->registers.x10;
    *(base + 11) = currProc->registers.x11;
    *(base + 12) = currProc->registers.x12;
    *(base + 13) = currProc->registers.x13;
    *(base + 14) = currProc->registers.x14;
    *(base + 15) = currProc->registers.x15;
    *(base + 16) = currProc->registers.x16;
    *(base + 17) = currProc->registers.x17;
    *(base + 18) = currProc->registers.x18;
    *(base + 19) = currProc->registers.x19;
    *(base + 20) = currProc->registers.x20;
    *(base + 21) = currProc->registers.x21;
    *(base + 22) = currProc->registers.x22;
    *(base + 23) = currProc->registers.x23;
    *(base + 24) = currProc->registers.x24;
    *(base + 25) = currProc->registers.x25;
    *(base + 26) = currProc->registers.x26;
    *(base + 27) = currProc->registers.x27;
    *(base + 28) = currProc->registers.x28;
    *(base + 29) = currProc->registers.x29;
    *(base + 30) = currProc->registers.x30;
    *(base + 31) = currProc->registers.exception_link_register;
    *(base + 32) = currProc->registers.saved_program_status_register;
    *(base + 33) = currProc->registers.stack_pointer;
}

void schedule() {
    kprintf("Schedule has been called\n");
    schedule_dump_state();
    currProc->numSlicesUsed++;
    kprintf("The current running process (%d) has elapsed %d slices\n", currProc->pid, currProc->numSlicesUsed);
    struct pcb_list *curr = runqueue;
    if (numProcesses == 1) {
        return;
    }
    runqueue = runqueue->next;
    curr->next = 0x0;

    struct pcb_list *entry;
    for (entry = runqueue; entry->next; entry = entry->next);
    entry->next = curr;

    kprintf("Process %d has been moved to the back of the list\n", curr->pcb->pid);
    currProc = runqueue->pcb;

    kprintf("The scheduler has decided to run process: %d\n", currProc->pid);
    do_switch_register_state();
    //terminate_interrupt(); TODO see if we can speed things up by using this
}

void schedule_dump_state() {
    kprintf("=========================SCHEDULER INFO=================\n");
    kprintf("Current Proc Pid: %d\n", currProc->pid);
    kprintf("Run Queue: ");
    struct pcb_list *l = runqueue;
    while (l) {
        kprintf("%d", l->pcb->pid);
        l = l->next;
        if (l) kprintf("->");
    }
    kprintf("\n");
    l = runqueue;
    while (l) {
        print_pcb_state(l->pcb);
        l = l->next;
    }

    kprintf("=========================SCHEDULER INFO=================\n");
}

void print_pcb_state(struct pcb *pcb) {
    kprintf("{\n\tPID=%d\n", pcb->pid);
    kprintf("\tStack Base=%p\n", pcb->stack);
    kprintf("\tElapsed Time Slices=%d\n", pcb->numSlicesUsed);
    kprintf("\tx0=%d\n", pcb->registers.x0);
    kprintf("\tx1=%d\n", pcb->registers.x1);
    kprintf("\tx2=%d\n", pcb->registers.x2);
    kprintf("\tx3=%d\n", pcb->registers.x3);
    kprintf("\tx4=%d\n", pcb->registers.x4);
    kprintf("\tx5=%d\n", pcb->registers.x5);
    kprintf("\tx6=%d\n", pcb->registers.x6);
    kprintf("\tx7=%d\n", pcb->registers.x7);
    kprintf("\tx8=%d\n", pcb->registers.x8);
    kprintf("\tx9=%d\n", pcb->registers.x9);
    kprintf("\tx10=%d\n", pcb->registers.x10);
    kprintf("\tx11=%d\n", pcb->registers.x11);
    kprintf("\tx12=%d\n", pcb->registers.x12);
    kprintf("\tx13=%d\n", pcb->registers.x13);
    kprintf("\tx14=%d\n", pcb->registers.x14);
    kprintf("\tx15=%d\n", pcb->registers.x15);
    kprintf("\tx16=%d\n", pcb->registers.x16);
    kprintf("\tx17=%d\n", pcb->registers.x17);
    kprintf("\tx18=%d\n", pcb->registers.x18);
    kprintf("\tx19=%d\n", pcb->registers.x19);
    kprintf("\tx20=%d\n", pcb->registers.x20);
    kprintf("\tx21=%d\n", pcb->registers.x21);
    kprintf("\tx22=%d\n", pcb->registers.x22);
    kprintf("\tx23=%d\n", pcb->registers.x23);
    kprintf("\tx24=%d\n", pcb->registers.x24);
    kprintf("\tx25=%d\n", pcb->registers.x25);
    kprintf("\tx26=%d\n", pcb->registers.x26);
    kprintf("\tx27=%d\n", pcb->registers.x27);
    kprintf("\tx28=%d\n", pcb->registers.x28);
    kprintf("\tx29=%d\n", pcb->registers.x29);
    kprintf("\tx30=%d\n", pcb->registers.x30);
    kprintf("\tException Link Register=%d\n", pcb->registers.exception_link_register);
    kprintf("\tSaved Program Status Register=%d\n", pcb->registers.saved_program_status_register);
    kprintf("\tStack Pointer=%d\n", pcb->registers.stack_pointer);
    kprintf("}\n");
}