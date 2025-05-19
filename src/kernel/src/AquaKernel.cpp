#include <aquaboot.h>
#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include <inc/krnl_colors.hpp>
#include <inc/logo.hpp>
#include <inc/terminal.hpp>
#include <inc/sys/gdt.hpp>
#include <inc/sys/idt.hpp>
#include <inc/mm/pmm.hpp>

#define KERNEL_MAJOR 0
#define KERNEL_MINOR 1
#define KERNEL_PATCH 0

std::uint32_t boot_major;
std::uint32_t boot_minor;
std::uint32_t boot_patch;

Terminal kern_terminal;

aquaboot_memory_descriptor *memory_map;

extern "C" {

    void *memcpy(void *__restrict dest, const void *__restrict src, std::size_t n) {
        std::uint8_t *__restrict pdest = static_cast<std::uint8_t *__restrict>(dest);
        const std::uint8_t *__restrict psrc = static_cast<const std::uint8_t *__restrict>(src);
    
        for (std::size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    
        return dest;
    }
    
    void *memset(void *s, int c, std::size_t n) {
        std::uint8_t *p = static_cast<std::uint8_t *>(s);
    
        for (std::size_t i = 0; i < n; i++) {
            p[i] = static_cast<uint8_t>(c);
        }
    
        return s;
    }
    
    void *memmove(void *dest, const void *src, std::size_t n) {
        std::uint8_t *pdest = static_cast<std::uint8_t *>(dest);
        const std::uint8_t *psrc = static_cast<const std::uint8_t *>(src);
    
        if (src > dest) {
            for (std::size_t i = 0; i < n; i++) {
                pdest[i] = psrc[i];
            }
        } else if (src < dest) {
            for (std::size_t i = n; i > 0; i--) {
                pdest[i-1] = psrc[i-1];
            }
        }
    
        return dest;
    }
    
    int memcmp(const void *s1, const void *s2, std::size_t n) {
        const std::uint8_t *p1 = static_cast<const std::uint8_t *>(s1);
        const std::uint8_t *p2 = static_cast<const std::uint8_t *>(s2);
    
        for (std::size_t i = 0; i < n; i++) {
            if (p1[i] != p2[i]) {
                return p1[i] < p2[i] ? -1 : 1;
            }
        }
    
        return 0;
    }
}    

void hlt()
{
    asm volatile(" hlt ");
}

Gdt gdt;

Idt idt;

Pmm pmm;

extern "C" void divInt(void);

extern "C" void kernel_main (aquaboot_info *boot_info)
{
    boot_major = boot_info->aquaboot_major;
    boot_minor = boot_info->aquaboot_minor;
    boot_patch = boot_info->aquaboot_patch;

    memory_map = boot_info->memory_map;

    kern_terminal.term_init(boot_info->framebuffer, KRNL_WHITE, KRNL_BLACK, boot_info->hhdm);

    kern_terminal.term_print(kernel_logo);

    kern_terminal.term_print("\n\n\tBooted by AquaBoot Version %d.%d.%d\n", boot_major, boot_minor, boot_patch);
    kern_terminal.term_print("\n\tAquaKernel Version %d.%d.%d\n\n", KERNEL_MAJOR, KERNEL_MINOR, KERNEL_PATCH);

    gdt.GDTDescs.null_segment = 0x0;
    gdt.GDTDescs.kernel_code = KERNEL_CS;
    gdt.GDTDescs.kernel_data = KERNEL_DS;
    gdt.GDTDescs.user_code = USER_CS;
    gdt.GDTDescs.user_data = USER_DS;

    gdt.loadGdt();

    kern_terminal.ksuccess("GDT Initialized!\n");

    idt.initIdt();

    kern_terminal.ksuccess("IDT Initialized!\n");

    std::uint64_t nop = 0;

    aquaboot_memory_descriptor *desc;

    for(uint64_t i = 0; i < boot_info->mem_map_entries; i++)
    {
        desc = (aquaboot_memory_descriptor *)((uint8_t *)memory_map + (i * boot_info->desc_size));
        if (desc->type == AQUAOS_FREE_MEMORY)
        {
            nop += desc->num_of_pages;
        }
    }

    if (((nop * 4) / 1024) < 400)   // Not all of RAM is usable, give some account for this
    {
        kern_terminal.kerror("Please run AquaOS with atleast 512MB of RAM!\n");
        hlt();
    }

    kern_terminal.term_print("AquaOS has %dGB of memory available\n", ((nop * 4) / 1024) / 1024);

    pmm.initPmm(memory_map, boot_info->mem_map_entries, boot_info->desc_size, boot_info->hhdm);

    uint64_t *test = pmm.palloc();

    *test = 64;

    kern_terminal.term_print("uint64_t allocated at 0x%x, holds the value %d\n", test, *test);

    hlt();
}