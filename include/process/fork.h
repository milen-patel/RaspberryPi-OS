#ifndef _FORK_H
#define _FORK_H
#include <stdbool.h>

// Creates a kernel thread, allocates a stack for it, sets the execution point to be the function whose address is `arg` and adds it to the ready queue
#define FORK_KERNEL_THREAD 1 
int fork(int fork_flags, void *function, void *arg, void *stack);
void move_kernel_thread_to_user_space(void *function);
void ret_from_fork();
#endif