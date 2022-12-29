#include "syscall/table.h"

.global pprintf
pprintf:
 mov w8, #SYSCALL_INDEX_PRINT
 svc #0
 ret

.global mmap
mmap:
 mov w8, #SYSCALL_INDEX_MMAP
 svc #0
 ret

.global exit
exit:
 mov w8, #SYSCALL_INDEX_EXIT
 svc #0
 ret

.global pfork
pfork:
    /* Save args for the child.  */
    mov    x10, x0                    /*fn*/
    mov    x11, x1                    /*arg*/
    mov    x12, x2                    /*stack*/

    /* Do the system call.  */
    mov    x0, x2                     /* stack  */
    mov    x8, #SYSCALL_INDEX_PFORK
    svc    0x0

    cmp    x0, #0
    beq    thread_start
    ret

thread_start:
    mov    x29, 0

    /* Pick the function arg and execute.  */
    mov    x0, x11
    blr    x10

    /* We are done, pass the return value through x0.  */
    mov    x8, #SYSCALL_INDEX_EXIT
    svc    0x0