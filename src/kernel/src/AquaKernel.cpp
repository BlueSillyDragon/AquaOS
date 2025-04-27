#include "../../boot/efi/src/inc/aquaboot.h"

extern "C" void kernel_main (void)
{
    asm volatile("mov $0xffff, %rax");
    for(;;);
}