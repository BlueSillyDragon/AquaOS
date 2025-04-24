OUTPUT_FORMAT(elf64-x86-64)

ENTRY(kernel_main)

SECTIONS
{
    . = 0xffffffff80000000;

     .text : ALIGN(CONSTANT(MAXPAGESIZE))
    {
        *(.text)
    }

    .rodata : ALIGN(CONSTANT(MAXPAGESIZE))
    {
        *(.rodata)
    }

    .data : ALIGN(CONSTANT(MAXPAGESIZE))
    {
        *(.data)
    }

    .bss : ALIGN(CONSTANT(MAXPAGESIZE))
    {
        *(COMMON)
        *(.bss)
    }
}