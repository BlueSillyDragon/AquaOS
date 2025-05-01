#include <aquaboot.h>
#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include "inc/krnl_colors.hpp"
#include "inc/krnl_font.hpp"
#include "inc/logo.hpp"
#include "inc/gdt.hpp"

#define KERNEL_MAJOR 0
#define KERNEL_MINOR 1
#define KERNEL_PATCH 0

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

static void plot_pixels (int y, int x, uint32_t pixel, aquaboot_framebuffer *fb) {
    std::uint64_t stuff = fb->base;

    std::uint32_t *fb_ptr = (uint32_t *)stuff;
    fb_ptr[x * (fb->pitch / 4) + y] = pixel;
}

int cursor_x;
int cursor_y;

std::uint32_t terminal_foreground;
std::uint32_t terminal_background;

void putchar (unsigned short int c, uint32_t fg, uint32_t bg, aquaboot_framebuffer *fb) {
    uint32_t pixel;
    
    for (int cy = 0; cy < 16; cy++) {
        for (int cx = 0; cx < 8; cx++) {
            pixel = (kernel_font[(c * 16) + cy] >> (7 - cx)) & 1 ? fg : bg;

            // Offset the cx and cy by x and y so that x and y are in characters instead of pixels
            plot_pixels((cx + (cursor_x * 8)), (cy + (cursor_y * 16)), pixel, fb);
        }
    }

}

void print(char* string, aquaboot_framebuffer *fb, ...) {

    std::va_list argp;
    va_start(argp, string);

    for (int i = 0;;i++) {

        // Check if the character is NULL, if it is, we've hit the end of the string
        if (string[i] == 0x00) {
            break;
        }

        // Check if it's '\n' if it is. move the cursor down.
        if (string[i] == 0x0A) {
            cursor_y++;
            cursor_x = 0;
            continue;
        }

        // Check if it's '\t' if it is, move the cursor foward by 6 spaces
        if (string[i] == 0x09) {
            cursor_x += 6;
            continue;
        }

        // Check the variable arguments
        if (string[i] == '%') {

            i++;

            if (string[i] == '%') {
                putchar('%', terminal_foreground, terminal_background, fb);
                cursor_x++;
                continue;
            }

            else if (string[i] == 'c')
            {
                char char_to_print = va_arg(argp, int);
                putchar(char_to_print, terminal_foreground, terminal_background, fb);
                cursor_x++;
                continue;
            }

            else if (string[i] == 'd') {
                int int_to_print = va_arg(argp, int);
                int number[100];
                int j = 0;
                do{
                    number[j] = (int_to_print % 10);
                    int_to_print = (int_to_print - int_to_print % 10) / 10;
                    j++;
                } while (int_to_print > 0);

                j--;

                for (; j>=0; j--) {
                    putchar((number[j] + '0'), terminal_foreground, terminal_background, fb);
                    cursor_x++;
                }

                continue;
            }      

            else {
                putchar('0', terminal_foreground, terminal_background, fb);
                cursor_x++;
                continue;
            }

            i--;
        }

        putchar(string[i], terminal_foreground, terminal_background, fb);

        cursor_x++;
    }

    va_end(argp);

}

void display_logo(aquaboot_framebuffer *fb) {
    print(kernel_logo, fb);
}

void kerror(char * string, aquaboot_framebuffer *fb)
{
    putchar('[', terminal_foreground, terminal_background, fb);
    cursor_x++;
    terminal_foreground = KRNL_RED;
    print("Error", fb);
    terminal_foreground = KRNL_WHITE;
    putchar(']', terminal_foreground, terminal_background, fb);
    cursor_x += 2;
    print(string, fb);
}

extern "C" void setGDT(std::uint16_t limit, std::uint64_t base);
extern "C" void reloadSegments(void);

extern "C" void kernel_main (aquaboot_info *boot_info)
{
    boot_info->framebuffer->base = (boot_info->hhdm + boot_info->framebuffer->base);    // Make sure we have the correct address
    std::uint64_t stuff = boot_info->framebuffer->base;
    asm volatile("mov %0, %%rax" :: "a"(stuff));
    
    cursor_x = 0;
    cursor_y = 0;
    terminal_foreground = KRNL_WHITE;
    terminal_background = KRNL_BLACK;

    display_logo(boot_info->framebuffer);

    print("\n\nBooted by AquaBoot Version %d.%d.%d\n", boot_info->framebuffer,
                                                                boot_info->aquaboot_major,
                                                                boot_info->aquaboot_minor,
                                                                boot_info->aquaboot_patch);
    print("\nAquaKernel Version %d.%d.%d\n", boot_info->framebuffer, KERNEL_MAJOR, KERNEL_MINOR, KERNEL_PATCH);

    GDTR gdtr;
    GDT gdt;

    gdt.null_seg = NULL_SEG;
    gdt.kernel_code = KERNEL_CODE_SEG;
    gdt.kernel_data = KERNEL_DATA_SEG;
    gdt.user_code = USER_CODE_SEG;
    gdt.user_data = USER_DATA_SEG;

    gdtr.offset = (boot_info->hhdm + (std::uint64_t)&gdt);
    gdtr.size = sizeof(gdt);
    asm volatile("mov %0, %%ax" :: "a"(gdtr.size));

    disable_interrupts();

    setGDT(gdtr.size, gdtr.offset);
    reloadSegments();

    putchar('[', terminal_foreground, terminal_background, boot_info->framebuffer);
    cursor_x++;
    terminal_foreground = KRNL_GREEN;
    print(" OK ", boot_info->framebuffer);
    terminal_foreground = KRNL_WHITE;
    putchar(']', terminal_foreground, terminal_background, boot_info->framebuffer);
    cursor_x += 2;
    print("GDT Initialized!", boot_info->framebuffer);

    hlt();
}