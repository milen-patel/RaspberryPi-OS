
.global init_interrupt_request_table
init_interrupt_request_table:
    // Load the address of the exception vector table into x0
    adr x0, exception_vector_list

    // Pass it to the correct system register (Vector Base Address Register)
    msr vbar_el1, x0

// Used to temporarily disable interrupts when handling them. bit 2 is used to mask IRQs
.global enable_irq
enable_irq:
    msr    daifclr, #2
    ret

.global disable_irq
disable_irq:
    msr    daifset, #2
    ret