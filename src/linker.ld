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
    .text.boot : { *(.text.boot) }
    .text :  { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    . = ALIGN(0x8);
    bss_begin = .;
    .bss : { *(.bss*) } 
    bss_end = .;
}