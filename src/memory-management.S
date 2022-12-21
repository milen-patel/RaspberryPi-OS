.global memzero

/*
This code assumes that the first parameter (the memory address) is stored in register x0 and the second parameter (the number of bytes to zero out) is stored in register x1. It uses a loop to zero out the memory, 8 bytes at a time, until the entire block has been zeroed.

Note that this code uses the "cbnz" (compare and branch if not zero) instruction, which is available in the ARMv8 architecture. This instruction compares the value in its first operand (in this case, register x3) to zero and, if the value is not zero, branches to the specified label. If the value is zero, the instruction does nothing and execution continues to the next instruction. This allows us to easily create a loop that continues until the loop counter reaches zero.
*/
memzero:
    // Set up loop counter
    mov x3, x1       // x3 = n
    mov x4, #0       // x4 = 0 (for storing zeroed value)

zero_memory_loop:
    // Zero out memory
    str x4, [x0], #8 // *x0 = 0; x0 += 8

    // Decrement loop counter and check for loop termination
    sub x3, x3, #8   // x3 -= 8
    cbnz x3, zero_memory_loop  // if x3 != 0, go to zero_memory_loop
