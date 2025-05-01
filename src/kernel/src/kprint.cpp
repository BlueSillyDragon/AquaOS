#include <aquaboot.h>
#include <cstdint>
#include <cstdarg>
#include <inc/krnl_colors.hpp>
#include <inc/krnl_font.hpp>
#include <inc/logo.hpp>
#include <inc/krpint.hpp>

int cursor_x;
int cursor_y;

std::uint32_t terminal_foreground;
std::uint32_t terminal_background;

aquaboot_framebuffer *framebuffer;

static void plot_pixels (int y, int x, uint32_t pixel) {
    std::uint64_t stuff = framebuffer->base;

    std::uint32_t *fb_ptr = (uint32_t *)stuff;
    fb_ptr[x * (framebuffer->pitch / 4) + y] = pixel;
}

void putchar (unsigned short int c, uint32_t fg, uint32_t bg) {
    uint32_t pixel;
    
    for (int cy = 0; cy < 16; cy++) {
        for (int cx = 0; cx < 8; cx++) {
            pixel = (kernel_font[(c * 16) + cy] >> (7 - cx)) & 1 ? fg : bg;

            // Offset the cx and cy by x and y so that x and y are in characters instead of pixels
            plot_pixels((cx + (cursor_x * 8)), (cy + (cursor_y * 16)), pixel);
        }
    }

}

void kprintf(char* string, ...) {

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
                putchar('%', terminal_foreground, terminal_background);
                cursor_x++;
                continue;
            }

            else if (string[i] == 'c')
            {
                char char_to_kprintf = va_arg(argp, int);
                putchar(char_to_kprintf, terminal_foreground, terminal_background);
                cursor_x++;
                continue;
            }

            else if (string[i] == 'd') {
                int int_to_kprintf = va_arg(argp, int);
                int number[100];
                int j = 0;
                do{
                    number[j] = (int_to_kprintf % 10);
                    int_to_kprintf = (int_to_kprintf - int_to_kprintf % 10) / 10;
                    j++;
                } while (int_to_kprintf > 0);

                j--;

                for (; j>=0; j--) {
                    putchar((number[j] + '0'), terminal_foreground, terminal_background);
                    cursor_x++;
                }

                continue;
            }
            
            else if (string[i] == 'x') {
                std::uint64_t int_to_kprintf = va_arg(argp, int);
                std::uint64_t number[100];
                int j = 0;
                do{
                    number[j] = (int_to_kprintf % 16);
                    int_to_kprintf = (int_to_kprintf - int_to_kprintf % 16) / 16;
                    j++;
                } while (int_to_kprintf > 0);

                j--;

                for (; j>=0; j--) {
                    if(number[j] > 0x9)
                    {
                        putchar((number[j] + ('0' + 7)), terminal_foreground, terminal_background);
                    }
                    else putchar((number[j] + '0'), terminal_foreground, terminal_background);
                    cursor_x++;
                }

                continue;
            }

            else {
                putchar('0', terminal_foreground, terminal_background);
                cursor_x++;
                continue;
            }

            i--;
        }

        putchar(string[i], terminal_foreground, terminal_background);

        cursor_x++;
    }

    va_end(argp);

}

void display_logo() {
    kprintf(kernel_logo);
}

void kerror(char * string)
{
    putchar('[', terminal_foreground, terminal_background);
    cursor_x++;
    terminal_foreground = KRNL_RED;
    kprintf("Error");
    terminal_foreground = KRNL_WHITE;
    putchar(']', terminal_foreground, terminal_background);
    cursor_x += 2;
    kprintf(string);
}