#include "process/fork.h"
#include "process/pcb.h"
#include "paging/alloc.h"
#include "paging/paging.h"

bool fork(void *function, void *arg) {
    // When a process calls fork we want to ensure that, if a timer interrupt happens, another process is not scheduled
    // Why? Suppose procA is interrupted in the middle of a fork() call, then procB runs and calls fork(). It is possible that,
    // depending on execution order, procB's new process could get written into slot 8 of the array and then procA's new process
    // also gets written into the same slot...this is bad.
    // 
    // A similar issue could arise in alloc_page()
    // 
    // So, we disable preemption to avoid this
    // TODO consider disabling IRQs in this scenario
    // TODO consider adding locks around alloc_page() and adding to the task struct and then see if we can remove this in the future
    disable_preemption(); 

    // Create a new page for the task's stack
    // At the bottom of the page, we will store the task_struct info
    // This should be fine, assuming the thread's stack doesn't get too large, since the stack will be at the top of the page and grow down towards the task_struct
    // TODO move this onto the kernels heap
    struct task_struct *ptr;
    ptr =  (struct task_struct *) alloc_page();

    // If we failed to allocate a page for the thread's stack, we cannot continue
    if (ptr == 0x0) {
        return false;
    }

    // Give the new thread the same priority as its parents
    ptr->priority = curr->priority;
    ptr->state = 1;
    ptr->counter = curr->priority;

    // We will also ensure that the first time the thread gets scheduled, it will not be preempted until it can finish the init work and begin execution of the function as specified by the parameter to this method
    ptr->preempt_count = 1;

    // Arbitrarily store the function and arg to the function in x19 and x20, respectively
    ptr->registers.x19 = (long unsigned int) function;
    ptr->registers.x20 = (long unsigned int) arg;

    // Set the PC to this other function ret_from_fork
    // This means that when the new tasks gets scheduled for the first time, it will end up in ret_from_fork
    // All ret_from_fork does is (1) enable premption, (2) move x20 into x0 and then call the function in x19
    // TODO why do we need to disable preemption on the new process if the first thing we do when we schedule it is enable it. see if we can remove this
    // TODO not sure ret_from_fork is needed, cant we create some short assmebly route to move x19 and x20 in the correct registers and then leave it from here
    ptr->registers.pc = (long unsigned int)ret_from_fork;

    // Set the stack pointer for this process to the top of the page we allocated for its stack since the stack grows down
    ptr->registers.sp = (long unsigned int) ((void *) ptr + PAGE_SIZE);

    // Add it to our list of tasks so that schedule can handle it
    // TODO it isnt safe to do it like this as tasks may decrease and we dont check the num running tasks > LIMIT, make this safer
    all_tasks[num_running_tasks++] = ptr; 

    // Okay, we have added the new task to the list of runnable tasks, so we are out of the critical section, so now the parent thread can be preempted
    enable_preemption();

    // If we make it here, then we surely got a new process in the runqueue!
    return true;
}