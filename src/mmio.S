.global delay
delay:
    sub x0, x0, #1
    cbnz x0, delay
    ret

.global get32
get32:
    ldr w0, [x0]
    ret

.global put32
put32:
    str w1, [x0], #1
    ret