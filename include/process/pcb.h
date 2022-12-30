#ifndef _PCB_H
#define _PCB_H

#include <stdint.h>

typedef struct {
    uint64_t x0;
    uint64_t x1;
    uint64_t x2;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t x8;
    uint64_t x9;
    uint64_t x10;
    uint64_t x11;
    uint64_t x12;
    uint64_t x13;
    uint64_t x14;
    uint64_t x15;
    uint64_t x16;
    uint64_t x17;
    uint64_t x18;
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t frame_pointer; //x28
    uint64_t stack_pointer; //x29
    uint64_t program_counter; //x30
    uint64_t exception_link_register;
    uint64_t saved_program_status_register;
} register_state;

struct pcb {
    register_state registers;
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
int new_kernel_thread(void *function, void *arg);
#endif