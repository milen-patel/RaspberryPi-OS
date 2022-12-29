#include "process/fork.h"
#include "process/pcb.h"
#include "paging/alloc.h"
#include "paging/paging.h"
#include "libk/memzero.h"
#include "kprintf.h"

struct pt_regs {
	unsigned long regs[31];
	unsigned long sp;
	unsigned long pc;
	unsigned long pstate;
};

struct pt_regs * task_pt_regs(void *tsk) {
    // tsk points to the bottom of the page of the task stack
    return (struct pt_regs *) (tsk + PAGE_SIZE - sizeof(struct pt_regs));
}

int fork(int fork_flags, void *function, void *arg, void *stack) {
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
        return -1;
    }

    // Zero out the CPU state and initialization register state
    memzero(ptr, sizeof(struct task_struct));
    memzero(task_pt_regs(ptr), sizeof(struct pt_regs));

    // Give the new thread the same priority as its parents
    ptr->priority = curr->priority;
    ptr->state = 1;
    ptr->counter = curr->priority;

    // We will also ensure that the first time the thread gets scheduled, it will not be preempted until it can finish the init work and begin execution of the function as specified by the parameter to this method
    ptr->preempt_count = 1;

    if (fork_flags) {
        // Arbitrarily store the function and arg to the function in x19 and x20, respectively
        ptr->registers.x19 = (long unsigned int) function;
        ptr->registers.x20 = (long unsigned int) arg;
    } else {
        struct pt_regs * cur_regs = task_pt_regs(curr);
        struct pt_regs * childregs = task_pt_regs(ptr);
        *childregs = *cur_regs;

        childregs->regs[0] = 0;
        childregs->sp = (unsigned long int) stack + PAGE_SIZE;
        ptr->stack = stack;
    }

    // Set the PC to this other function ret_from_fork
    // This means that when the new tasks gets scheduled for the first time, it will end up in ret_from_fork
    // All ret_from_fork does is (1) enable premption, (2) move x20 into x0 and then call the function in x19
    // TODO why do we need to disable preemption on the new process if the first thing we do when we schedule it is enable it. see if we can remove this
    // TODO not sure ret_from_fork is needed, cant we create some short assmebly route to move x19 and x20 in the correct registers and then leave it from here
    ptr->registers.pc = (long unsigned int)ret_from_fork;

    // Set the stack pointer for this process to the top of the page we allocated for its stack since the stack grows down
    kprintf("Computing sp for new process. %p page base gets translated to %p\n", ptr, task_pt_regs(ptr));
    ptr->registers.sp = (long unsigned int) task_pt_regs(ptr);

    // Add it to our list of tasks so that schedule can handle it
    // TODO it isnt safe to do it like this as tasks may decrease and we dont check the num running tasks > LIMIT, make this safer
    all_tasks[num_running_tasks++] = ptr; 

    // Okay, we have added the new task to the list of runnable tasks, so we are out of the critical section, so now the parent thread can be preempted
    enable_preemption();

    // If we make it here, then we surely got a new process in the runqueue!
    return num_running_tasks - 1;
}

void move_kernel_thread_to_user_space(void *new_function_to_execute) {
    kprintf("ENTER\n");
    struct pt_regs *regs = task_pt_regs(curr);
    memzero(regs, sizeof(*regs));
    regs->pc = (unsigned long int) new_function_to_execute;
    regs->pstate = 0x0;
    unsigned long stack = (unsigned long int) alloc_page(); //allocate new user stack
    if (!stack) {
        return;
    }
    regs->sp = stack + PAGE_SIZE;
    curr->stack = (void *) stack;
    kprintf("EXIT\n");
}