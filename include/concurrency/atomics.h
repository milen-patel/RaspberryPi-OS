#ifndef _ATOMICS_H
#define _ATOMICS_H
#include <stdint.h>

/*
 * This function takes a pointer to an int64_t value as its only parameter and 
 * returns an int64_t value. The function performs an atomic decrement of the value 
 * at the specified address and returns the new value.
 * 
 * Used to implement spin locks
 */
int64_t atomic_decrement(int64_t *ptr);


#endif