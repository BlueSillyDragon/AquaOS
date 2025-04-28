#include "../../boot/efi/src/inc/aquaboot.h"

extern "C" void kernel_main (aquaboot_info *boot_info)
{
    asm ("mov %0, %%eax" :: "a"(0x2fff));
    asm (" hlt ");
}