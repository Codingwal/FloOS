SECTIONS
{
    /* Stack grows downward from the kernel executable (0x1_0000 bytes space between kernel executable and _stack_top) */
    _stack_bottom = 0x60000;
    _stack_top = 0x70000;

    . = 0x80000;     /* Kernel load address for AArch64 */
    _start = .;

    _text_start = .;
    .text : { KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t*) }
    _text_end = .;

    /* Data sector starts on the next page */
    . += 1;         
    . = ALIGN(4096);

    _data_start = .;
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
    .data : { *(.data .data.* .gnu.linkonce.d*) }

    . = ALIGN(16);
    _bss_start = .;
    .bss (NOLOAD) : {
        *(.bss .bss.*)
        *(COMMON)
    }
    _bss_end = .;

    _data_end = .;

    _end = .;

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
_bss_size = (_bss_end - _bss_start);