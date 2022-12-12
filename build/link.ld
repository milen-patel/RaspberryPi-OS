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
    . = 0x80000;     /* Kernel load address for AArch64 */
    .text : { 
		KEEP(*(.text.boot))
		*(.text .text.* .gnu.linkonce.t*) 
	}
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
    PROVIDE(_data = .);
    .data : { *(.data .data.* .gnu.linkonce.d*) }
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
    _end = .;

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
__bss_size = (__bss_end - __bss_start)>>3;
