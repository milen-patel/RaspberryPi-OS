#ifndef _SYSCALL_H
#define _SYSCALL_H

// TODO revise all of these, fix names (kprintf/printf) and make then behave like linux
void pprintf(char *fmt, ...);
void *mmap();
void exit();
void fork(void *function, void *arg, void *stack);

#endif