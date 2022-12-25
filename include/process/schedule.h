#ifndef _PSCHED_H
#define _PSCHED_H

#include "pcb.h"

void cpu_switch_to(struct task_struct *old, struct task_struct *new);
void timer_tick();

#endif