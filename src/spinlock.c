#include "concurrency/spinlock.h"
#include "concurrency/atomics.h"

void init_spinlock(struct spinlock *lock) {
    lock->lockVal = 0;
}
void lock_spinlock(struct spinlock *lock) {
    while (atomic_decrement(&lock->lockVal) != 0) {
        do {
            __asm__("wfe");
        } while (lock->lockVal <= 0);
    }
}
void unlock_spinlock(struct spinlock *lock) {
    lock->lockVal = 1;
}