#ifndef _SPINLOCK_H
#define _SPINLOCK_H

struct spinlock {
    int64_t lockVal
}

void init_spinlock(struct spinlock *lock);
void lock_spinlock(struct spinlock *lock);
void unlock_spinlock(struct spinlock *lock);

#endif