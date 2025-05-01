#include <aquaboot.h>
#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include <inc/krpint.hpp>
#include "inc/krnl_colors.hpp"
#include "inc/sys/gdt.hpp"
#include "inc/sys/idt.hpp"

#define KERNEL_MAJOR 0
#define KERNEL_MINOR 1
#define KERNEL_PATCH 0

std::uint64_t HHDM;

extern int cursor_x;
extern int cursor_y;

extern std::uint32_t terminal_foreground;
extern std::uint32_t terminal_background;

extern aquaboot_framebuffer *framebuffer;

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

void disable_interrupts()
{
    asm volatile(" cli ");
}

void enable_interrupts()
{
    asm volatile(" sti ");
}

static bool vectors[256];
extern "C" void *isr_stub_table[];

void idt_set_desc(std::uint8_t vector, void *isr, std::uint8_t flags, idt_entry_t *idt)
{
    idt_entry_t *descriptor = &idt[vector];

    descriptor->isr_low = ((std::uint64_t)isr & 0xFFFF);
    descriptor->kernel_cs = 0x08;
    descriptor->ist = 0;
    descriptor->attributes = flags;
    descriptor->isr_mid        = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high       = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved = 0;
}

void init_idt(idtr_t idtr, idt_entry_t *idt)
{
    idtr.offset = (std::uintptr_t)&idt[0];
    idtr.size = (std::uint16_t)sizeof(idt_entry_t) * 256 - 1;

    for(std::uint8_t vector = 0; vector < 32; vector++)
    {
        idt_set_desc(vector, isr_stub_table[vector], 0x8e, idt);
        vectors[vector] = true;
    }
    kprintf("IDTR Offset: 0x%x\n", idtr.offset);
    __asm__ volatile ("lidt %0" : : "m"(idtr));
    __asm__ volatile ("sti");
}

extern "C" void divErr(void);

extern "C" void kernel_main (aquaboot_info *boot_info)
{
    boot_info->framebuffer->base = (boot_info->hhdm + boot_info->framebuffer->base);    // Make sure we have the correct address
    framebuffer = boot_info->framebuffer;
    std::uint64_t stuff = boot_info->framebuffer->base;
    asm volatile("mov %0, %%rax" :: "a"(stuff));
    
    cursor_x = 0;
    cursor_y = 0;
    terminal_foreground = KRNL_WHITE;
    terminal_background = KRNL_BLACK;

    display_logo();

    kprintf("\n\nBooted by AquaBoot Version %d.%d.%d\n",
                                                                boot_info->aquaboot_major,
                                                                boot_info->aquaboot_minor,
                                                                boot_info->aquaboot_patch);
    kprintf("\nAquaKernel Version %d.%d.%d\n", KERNEL_MAJOR, KERNEL_MINOR, KERNEL_PATCH);

    init_gdt(boot_info);

    putchar('[', terminal_foreground, terminal_background);
    cursor_x++;
    terminal_foreground = KRNL_GREEN;
    kprintf(" OK ");
    terminal_foreground = KRNL_WHITE;
    putchar(']', terminal_foreground, terminal_background);
    cursor_x += 2;
    kprintf("GDT Initialized!\n");

    __attribute__((aligned(0x10))) static idt_entry_t idt[256];
    static idtr_t idtr;

    init_idt(idtr, idt);

    //divErr(); Triple Faults and doesn't jump to the exception handler

    hlt();
}