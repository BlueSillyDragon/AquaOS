#include <aquaboot.h>
#include <cstdint>
#include <cstdarg>
#include "inc/krnl_colors.hpp"
#include "inc/krnl_font.hpp"
#include "inc/logo.hpp"

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

static void plotPixels (int y, int x, uint32_t pixel, aquaboot_framebuffer *fb) {
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
            plotPixels((cx + (cursor_x * 8)), (cy + (cursor_y * 16)), pixel, fb);
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
                while(int_to_print > 0) {
                    number[j] = (int_to_print % 10);
                    int_to_print = (int_to_print - int_to_print % 10) / 10;
                    j++;
                }

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

extern "C" void kernel_main (aquaboot_info *boot_info)
{
    boot_info->framebuffer->base = (boot_info->hhdm + boot_info->framebuffer->base);    // Make sure we have the correct address
    std::uint64_t stuff = boot_info->framebuffer->base;
    asm volatile("mov %0, %%rax" :: "a"(stuff));
    
    cursor_x = 0;
    cursor_y = 0;
    terminal_foreground = KRNL_WHITE;
    terminal_background = KRNL_BLACK;
    int c = 0;

    display_logo(boot_info->framebuffer);
    hlt();
}