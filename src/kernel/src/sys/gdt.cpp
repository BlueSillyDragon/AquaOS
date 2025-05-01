#include "aquaboot.h"
#include <cstdint>
#include <inc/sys/gdt.hpp>

extern "C" void setGDT(std::uint16_t limit, std::uint64_t base);
extern "C" void reloadSegments(void);

void init_gdt(aquaboot_info *boot_info)
{
    gdtr_t gdtr;
    gdt_t gdt;

    gdt.null_seg = NULL_SEG;
    gdt.kernel_code = KERNEL_CODE_SEG;
    gdt.kernel_data = KERNEL_DATA_SEG;
    gdt.user_code = USER_CODE_SEG;
    gdt.user_data = USER_DATA_SEG;

    gdtr.offset = (boot_info->hhdm + (std::uint64_t)&gdt);
    gdtr.size = sizeof(gdt);

    setGDT(gdtr.size, gdtr.offset);
    reloadSegments();
}