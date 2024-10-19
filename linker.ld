ENTRY(_start)

SECTIONS
{
    . = 0x80000;

    .text : {
        *(.text.boot)    /* Bootloader code */
        *(.text)         /* Kernel code */
    }

    .data : {
        *(.data)
    }

    .bss : {
        __bss_start = .;
        *(.bss)
        *(COMMON)
        __bss_end = .;
    }

    _end = .;
}