#ifndef _PCB_H
#define _PCB_H

#include <stdint.h>

typedef struct {
    uint32_t x0;
    uint32_t x1;
    uint32_t x2;
    uint32_t x4;
    uint32_t x5;
    uint32_t x6;
    uint32_t x7;
    uint32_t x8;
    uint32_t x9;
    uint32_t x10;
    uint32_t x11;
    uint32_t x12;
    uint32_t x13;
    uint32_t x14;
    uint32_t x15;
    uint32_t x16;
    uint32_t x17;
    uint32_t x18;
    uint32_t x19;
    uint32_t x20;
    uint32_t x21;
    uint32_t x22;
    uint32_t x23;
    uint32_t x24;
    uint32_t x25;
    uint32_t x26;
    uint32_t x27;
    uint32_t x28;
    uint32_t x29;
    uint32_t x30;
} register_state;

struct pcb {
    register_state registers;
    uint32_t pid;
    void *stack;
    uint8_t state;
    uint8_t priority;
    uint8_t numSlicesUsed;
    struct pcb *next;
};

#define STATE_RUNNING 1
#define STATE_READY 2 
#define STATE_BLOCKED 3
#define STATE_ZOMBIE 4 

#endif