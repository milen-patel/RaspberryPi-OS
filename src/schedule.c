#include "process/pcb.h"
#include "process/schedule.h"
#include "interrupts/toggle.h"
#include "printf.h"
#include <stdbool.h>

struct task_struct init_task = {{0,0,0,0,0,0,0,0,0,0,0,0,0}, 0,0,1, 0 };
struct task_struct *curr = &init_task;

struct task_struct *all_tasks[MAX_TASKS] = {&init_task, };
int num_running_tasks = 1;

void disable_preemption() {
    curr->preempt_count++;
}

void enable_preemption() {
    curr->preempt_count--;
}

void switch_to(struct task_struct *new_task) {
    // If the scheduler decides to run the process we are already running, then we don't need to do a context switch
    if (curr == new_task) return;

    // Perform an actual context switch to the new task
    // Recall: From curr's POV cpu_switch_to will return and curr can resume execution (after some function frames return followed by an eret in the case of an interrupt)
    struct task_struct *prev = curr;
    curr = new_task;
    cpu_switch_to(prev, new_task);
    return;
}

void schedule() {
    // Handles case where a kthread yields by calling schedule, it's time slice hasn't changed so without this it would get scheduled again
    // This is also safe in the case where a timer interrupt happens since we only call schedule from timer_tick when the counter == 0
    curr->counter = 0;

    // For reasons we explained earlier, we dont want this process to get swapped while in the middle of scheduling if an IRQ arrives, so use our software trick to prevent this
    // TODO see if we can just disable IRQs in the interrupt handler and create a seperate yield() function for the other case that wraps around schedule with disabling preemption. OR just make schedule disable IRQS entirely until it finishes...
    disable_preemption(); 

    // First, Are all the tasks out of time slots?
    bool allTasksEmpty = true;
    for (int i = 0; i < num_running_tasks; i++) { // TODO this fails to let processes terminate
        if (all_tasks[i]->counter) {
            allTasksEmpty = false;
            break;
        }
    }
    if (allTasksEmpty) {
        printf("Scheduler detects that all time slices are empty....\n");
        // Reset time slices
        for (int i = 0; i < num_running_tasks; i++) {
            all_tasks[i]->counter = all_tasks[i]->priority;
        }
    }

    // Now, pick the task with the highest counter to run
    int maxIdx = 0;
    for (int i = 1; i < num_running_tasks; ++i) {
        if (all_tasks[i]->counter > all_tasks[maxIdx]->counter) maxIdx = i;
    }
    printf("Schedule has decided on running task %d\n", maxIdx);
    switch_to(all_tasks[maxIdx]);
    enable_preemption();
}


void timer_tick() {
    // The current process has used another time slice
    --curr->counter;

    // IF the current process is out of time slices, then its time for another schedule to run. If it has more left, then keep it running
    // If the current process is out of time slices but its in some critical section, leave it alone (even if that means it gets more time than it deserves)
	if (curr->counter>0 || curr->preempt_count >0) {
		return;
	}

    // In case counter is negative, reset it to 0. In most cases, it'll already be 0 by virtue of expiring its quantum
	curr->counter=0;

	enable_irq();
	schedule();
    // TODO cannot figure out why we disable IRQ's and how this lets us continue to get them...
    // TODO see if we can modify schedule such that we just disable IRQ's entirely
	disable_irq();
}