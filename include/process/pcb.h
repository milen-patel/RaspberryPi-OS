#ifndef _PCB_H
#define _PCB_H

#define MAX_TASKS 100
// TODO fix this later

/*
 * An interesting question is why do we not save x0-x18 onto this struct?
 *
 * Recall that a context switch only happens when a task calls cpu_switch_to.
 * cpu_switch_to() is the function that actually uses these structs.
 * 
 * How does this function get called in the first place?
 * 
 * Option 1: A process yields by calling schedule()
 *      Then, schedule() identifies the next process to run and switches to that process by calling cpu_switch_to
 *      The idea is that, from that processes POV, when it returns it doesn't know that another process has ran but it know it can resume its execution
 *      So, all the yielding is doing is calling a function. When we call a function, the caller cannot assume x0-x18 are going to be the same after return
 *      So, we want the original process to behave exactly as it did right before it called cpu_switch_to as it will when it returns
 *      All we need to guarantee for execution to resume normally is that x19 onwards is the same, along with the fp/sp and PC
 *
 * You also do not need to save the Link Register, since for us to switch back to this process, another process ought to have called cpu_switch_to from schedule() which means it correctly set the link register
 *
 * Option 2: An interrupt happens
 *      Then, the interrupt handler starts by dumping ALL Registers onto the stack of the process
 *      Then, assembly -> handle_irq -> handle_timer_irq -> timer_tick -> schedule -> cpu_switch_to
 *      This is a function call, so for the same reason we ought not store all the registers
 *      Then, when its the original processes turn to run again, we restore all the registers at the moment it called cpu_switch_to
 *      All the functions return in reverse order, eventually we are back to the interrupt handler, the interrupt stack has collapsed
 *      The kernel_exit macro restores all of the register state x0-x30 which was the processes state at the time of the interrupt
 *      Then we can execute the eret instruction and process A continues running as if nothing ever happened
 *
 *      Pretty smart...
 */
struct cpu_register_state {
    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
    unsigned long fp; // x29
    unsigned long sp;
    unsigned long pc; // x30
};

// Represents a runnable task in the system (technically a thread for now)
struct task_struct {
    // Register state of the system, its important this is the first member in the struct or else the assmebly won't behave as expected
    struct cpu_register_state registers;

    // For now, this will always be constant as every program is always runnable
    long state;

    // How many time slices the process has remaining to run before we move to another process
    long counter;

    // Used to determine how many time slices the process should get each time the kernel resets all of the counters
    long priority;

    // If > 0, means that the program is doing something important and should not be interrupted
    // Thus, when a program is doing something important, it should increment this value (i.e. call preempt_enable) and when it is done decrement it (call preempt_disable)
    // When a timer interrupt happens, we end up in timer_tick, if it sees the value is non-zero it returns which means the same process is scheduled again!
    long preempt_count;

    void *stack;
    int flags;
};

// Used by a program to ensure it is not scheduled over while executing something important
// For now, this is only used in the schedule function to ensure that if we are in the schedule function and an interrupt happens, we should not switch processes or this could interfere the schedule function when it returns depending on race condiitons
// Another interesting question is why not just disable IRQs while we are handling an interrupt: the issue is if we are in schedule() and every task is blocked, we are clearly waiting on an interrupt so we should allow interrupts to occur to update this state
void disable_preemption();
void enable_preemption();

// Determines which process to runs next and updates curr and other state accordingly
// This is the magic..if a handler/yielding process calls this, it has no idea another process(es) is scheduled and run
// It just magically knows that it will return after some period of time and when it does return it has control
void schedule();

// Represents the currently running task
extern struct task_struct *curr;

// Used to store information about all running tasks
// TODO in the future this should be altered into a linked list to allow for infinite(!) processes to be scheduled
extern struct task_struct *all_tasks[];

// Used to identify how many tasks are in all_tasks
extern int num_running_tasks;

#endif