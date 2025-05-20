#include <aquaboot.h>
#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include <inc/krnl_colors.hpp>
#include <inc/logo.hpp>
#include <inc/print.hpp>
#include <inc/sys/gdt.hpp>
#include <inc/sys/idt.hpp>
#include <inc/mm/pmm.hpp>

#define KERNEL_MAJOR 0
#define KERNEL_MINOR 1
#define KERNEL_PATCH 0  

void hlt()
{
    asm volatile(" hlt ");
}

extern "C" void kernel_main (aquaboot_info *boot_info)
{
    term_init(boot_info->framebuffer, KRNL_WHITE, KRNL_BLACK, boot_info->hhdm);

    kprintf(kernel_logo);

    kprintf("\n\n\tBooted by AquaBoot Version %d.%d.%d\n", boot_info->aquaboot_major, boot_info->aquaboot_minor, boot_info->aquaboot_patch);
    kprintf("\n\tAquaKernel Version %d.%d.%d\n\n", KERNEL_MAJOR, KERNEL_MINOR, KERNEL_PATCH);

    init_gdt();
    init_idt();
    init_pmm(boot_info->memory_map, boot_info->mem_map_entries, boot_info->desc_size, boot_info->hhdm);

    hlt();
}