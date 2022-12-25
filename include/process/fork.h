#ifndef _FORK_H
#define _FORK_H
#include <stdbool.h>

// Creates a kernel thread, allocates a stack for it, sets the execution point to be the function whose address is `arg` and adds it to the ready queue
bool fork(void *function, void *arg);
void ret_from_fork();
#endif