#include <aquaboot.h>

void hlt()
{
    asm volatile(" hlt ");
}

extern "C" void kernel_main (aquaboot_info *boot_info)
{
    asm volatile ("mov %0, %%eax" :: "a"(0xffff8000));
    hlt();
}