#include "syscall/syscall.h"
#include "kprintf.h"
#include "paging/alloc.h"
#include "get-el.h"
#include "process/fork.h"

void printf_impl(char *fmt) {
    kprintf(fmt);
}

void *mmap_impl() {
    kprintf("In mmap_impl\n");
    kprintf("Current EL should be 1, currentEl=%d\n", getExceptionLevel());
    return alloc_page();
}
void exit_impl() {
    kprintf("TODO\n");
}

int pfork_impl(void *stack) {
    kprintf("In pfork_impl()\n");
    //kprintf("in pfork syscall handler with function=%p, arg=%p, stack=%p\n", function, arg, stack);
    return fork(0, 0, 0, stack);
}


void * const sys_call_table[] = {
    printf_impl,
    mmap_impl,
    exit_impl,
    pfork_impl
};