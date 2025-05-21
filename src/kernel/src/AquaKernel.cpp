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
#include <inc/mm/vmm.hpp>

#define KERNEL_MAJOR 0
#define KERNEL_MINOR 1
#define KERNEL_PATCH 0  

void hlt()
{
    asm volatile(" hlt ");
}

Terminal kern_terminal;

extern "C" void kernel_main (aquaboot_info *boot_info)
{
    kern_terminal.term_init(boot_info->framebuffer, KRNL_WHITE, KRNL_BLACK, boot_info->hhdm);

    kern_terminal.term_print(kernel_logo);

    kern_terminal.term_print("\n\n\tBooted by AquaBoot Version %d.%d.%d\n", boot_info->aquaboot_major, boot_info->aquaboot_minor, boot_info->aquaboot_patch);
    kern_terminal.term_print("\n\tAquaKernel Version %d.%d.%d\n\n", KERNEL_MAJOR, KERNEL_MINOR, KERNEL_PATCH);

    init_gdt();
    init_idt();
    init_pmm(boot_info->memory_map, boot_info->mem_map_entries, boot_info->desc_size, boot_info->hhdm);
    init_vmm(boot_info->hhdm, boot_info->kernel_paddr);

    hlt();
}