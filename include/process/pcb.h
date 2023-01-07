#ifndef _PCB_H
#define _PCB_H

#include <stdint.h>

struct register_state{
    long x0;
    long x1;
    long x2;
    long x3;
    long x4;
    long x5;
    long x6;
    long x7;
    long x8;
    long x9;
    long x10;
    long x11;
    long x12;
    long x13;
    long x14;
    long x15;
    long x16;
    long x17;
    long x18;
    long x19;
    long x20;
    long x21;
    long x22;
    long x23;
    long x24;
    long x25;
    long x26;
    long x27;
    long x28;
    long x29;
    long x30;
    long exception_link_register;
    long saved_program_status_register;
    long stack_pointer;
};

struct pcb {
    struct register_state registers;
    uint32_t pid;
    void *stack;
    uint8_t state;
    uint8_t priority;
    uint8_t numSlicesUsed;
};

struct pcb_list {
    struct pcb *pcb;
    struct pcb_list *next;
};

#define STATE_RUNNING 1
#define STATE_READY 2 
#define STATE_BLOCKED 3
#define STATE_ZOMBIE 4 


void init_scheduler();
void schedule();
void schedule_dump_state();
int new_kernel_thread(void *function, void *arg);
void print_pcb_state(struct pcb *pcb);
void terminate_interrupt();
#endif