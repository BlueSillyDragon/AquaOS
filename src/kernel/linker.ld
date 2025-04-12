OUTPUT_FORMAT(elf64-x86-64)

ENTRY(kernel_main)

SECTIONS
{
     .text : ALIGN(CONSTANT(MAXPAGESIZE))
    {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(COMMON)
        *(.bss)
    }
}