/*
 * Without this file, clang would compile the individual .c and .S files and then
 * link them as if this was a user space program.
 *
 * However, this is obviously not a user space program and the kernel needs to
 * specify certain absolute addresses to adhere to the convention expected by the
 * hardware.
 */
 SECTIONS
{
    /* Because we are enabling virtual memory, we must set the base address to 0xFFFF. This means that whenever we execute kernel code, the translation will be looked up in the kernel's paging tables */
    . = 0xFFFF000000000000;

    .text.boot : { *(.text.boot) }
    .text :  { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }

    . = ALIGN(0x8);
    bss_begin = .;
    .bss : { *(.bss*) } 
    bss_end = .;

    . = ALIGN(0x1000);
    .cpuakernelstack = 0xFFFF000000401000; /* TODO will need to revisit these shortly... */
    __paging_start = 0xFFFF000000401000;

    /* Must reserve three consecutive pages for the kernel's paging tables */
    . = ALIGN(0x1000);
    kernel_page_table_start = .;
    .data.kernel_page_tables : { . += (4096 * 3);}
}