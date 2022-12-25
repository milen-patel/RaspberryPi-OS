.global ret_from_fork
ret_from_fork:
    bl    enable_preemption
    mov    x0, x20
    blr    x19         //should never return


// Gets called from schedule()
// Recall that a function should leave x19 onwards alone, so we save it to the PCB for thread src
// What we do is pretty smart, since even though we are clobbering these values, when its the src threads turn to execute again
// , whatever context we switched from back to src will have restored srcs registers. So we are doing the reverse. If we are scheudling
// process K, then we are restoring Ks registers and then we execute the ret and now proc K is back to running in the instruction following
// cpu_switch_to. When thread K returns to us, itll pop the values we pushed to our PCB and execute ret and now proc A is executing the line after
// the call to cpu_switch_to. Pretty smart!
.global cpu_switch_to
cpu_switch_to:
    mov    x10, 0
    add    x8, x0, x10                // x8 = cpu_register_state base address for process curr
    mov    x9, sp                     // Handle the stack pointer separately
    stp    x19, x20, [x8], #16        // store callee-saved registers
    stp    x21, x22, [x8], #16
    stp    x23, x24, [x8], #16
    stp    x25, x26, [x8], #16
    stp    x27, x28, [x8], #16
    stp    x29, x9, [x8], #16
    str    x30, [x8]
    add    x8, x1, x10
    ldp    x19, x20, [x8], #16        // restore callee-saved registers
    ldp    x21, x22, [x8], #16
    ldp    x23, x24, [x8], #16
    ldp    x25, x26, [x8], #16
    ldp    x27, x28, [x8], #16
    ldp    x29, x9, [x8], #16
    ldr    x30, [x8]
    mov    sp, x9
    ret
