OUTPUT_FORMAT(elf64-x86-64)

ENTRY(kernel_main)

PHDRS
{
    text PT_LOAD;
    rodata PT_LOAD;
    data PT_LOAD;
}

SECTIONS
{
    . = 0xffffffff80000000;

    .text : ALIGN(CONSTANT(MAXPAGESIZE)){
        *(.text .text.*)
    } :text

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    /* C++ is a language that allows for global constructors. In order to obtain the */
    /* address of the ".init_array" section we need to define a symbol for it. */
    .init_array : {
        __init_array = .;
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP(*(.init_array .ctors))
        __init_array_end = .;
    } :rodata

    /* Move to the next memory page for .data */
    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .data : {
        *(.data .data.*)
    } :data

    /* NOTE: .bss needs to be the last thing mapped to :data, otherwise lots of */
    /* unnecessary zeros will be written to the binary. */
    /* If you need, for example, .init_array and .fini_array, those should be placed */
    /* above this. */
    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :data

    /* Discard .note.* and .eh_frame* since they may cause issues on some hosts. */
    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
    }
}