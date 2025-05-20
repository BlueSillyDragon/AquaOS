#include <cstdint>
#include <cstdarg>
#include <inc/print.hpp>
#include <inc/krnl_font.hpp>
#include <inc/krnl_colors.hpp>

std::uint32_t foreground = KRNL_WHITE;
std::uint32_t background = KRNL_BLACK;
std::uint64_t cursor_x = 0;
std::uint64_t cursor_y = 0;
aquaboot_framebuffer framebuffer;

void term_init(aquaboot_framebuffer *fb, std::uint32_t fg, std::uint32_t bg, std::uint64_t hhdm)
{
    framebuffer.base = (hhdm + fb->base);
    framebuffer.pitch = fb->pitch;
    framebuffer.horizontalRes = fb->horizontalRes;
    framebuffer.verticalRes = fb->verticalRes;
    foreground = fg;
    background = bg;
}

void plot_pixels(std::uint64_t y, std::uint64_t x, uint32_t pixel)
{
    std::uint32_t *fb_ptr = reinterpret_cast<std::uint32_t *>(framebuffer.base);
    fb_ptr[x * (framebuffer.pitch / 4) + y] = pixel;
}

void putchar(char c)
{
    uint32_t pixel;
    
    for (int cy = 0; cy < 16; cy++) {
        for (int cx = 0; cx < 8; cx++) {
            pixel = (kernel_font[(c * 16) + cy] >> (7 - cx)) & 1 ? foreground : background;

            // Offset the cx and cy by x and y so that x and y are in characters instead of pixels
            plot_pixels((cx + (cursor_x * 8)), (cy + (cursor_y * 16)), pixel);
        }
    }
}

void kprintf(char *string, ...)
{
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
                putchar('%');
                cursor_x++;
                continue;
            }

            else if (string[i] == 'c')
            {
                char char_to_print = va_arg(argp, int);
                putchar(char_to_print);
                cursor_x++;
                continue;
            }

            else if (string[i] == 'd') {
                std::uint64_t int_to_print = va_arg(argp, uint64_t);
                std::uint64_t number[100];
                int j = 0;
                do{
                    number[j] = (int_to_print % 10);
                    int_to_print = (int_to_print - int_to_print % 10) / 10;
                    j++;
                } while (int_to_print > 0);

                j--;

                for (; j>=0; j--) {
                    putchar((number[j] + '0'));
                    cursor_x++;
                }

                continue;
            }
            
            else if (string[i] == 'x') {
                std::uint64_t int_to_print = va_arg(argp, uint64_t);
                std::uint64_t number[100];
                int j = 0;
                do {
                    number[j] = (int_to_print % 16);
                    int_to_print = (int_to_print - int_to_print % 16) / 16;
                    j++;
                } while (int_to_print > 0);

                j--;

                for (; j>=0; j--) {
                    if(number[j] > 0x9)
                    {
                        putchar((number[j] + ('0' + 7)));
                    }
                    else putchar((number[j] + '0'));
                    cursor_x++;
                }

                continue;
            }

            else if (string[i] == 's') {
                char *string_to_print = va_arg(argp, char*);

                while (*string_to_print != '\0')
                {
                    putchar(*string_to_print);
                    string_to_print++;
                    cursor_x++;
                }
                continue;
            }

            else {
                putchar('0');
                cursor_x++;
                continue;
            }

            i--;
        }

        putchar(string[i]);

        cursor_x++;
    }

    va_end(argp);
}

void kerror(char *string)
{
    kprintf("[");
    change_colors(KRNL_RED, KRNL_BLACK);
    kprintf(" Error ");
    change_colors(KRNL_WHITE, KRNL_BLACK);
    kprintf("] ");
    kprintf(string);
}

void ksuccess(char *string)
{
    kprintf("[");
    change_colors(KRNL_GREEN, KRNL_BLACK);
    kprintf(" OK ");
    change_colors(KRNL_WHITE, KRNL_BLACK);
    kprintf("] ");
    kprintf(string);
}

void kinfo(INFO_TYPE type, char *string)
{
    kprintf("[ ");
    switch (type)
    {
        case PMM:
            change_colors(KRNL_BLUE, KRNL_BLACK);
            kprintf("PMM");
            break;
        case VMM:
            change_colors(KRNL_PINK, KRNL_BLACK);
            kprintf("VMM");
            break;
        case SCHEDULER:
            change_colors(KRNL_GREEN, KRNL_BLACK);
            kprintf("SCHEDULER");
            break;
        default:
        kprintf(" ");
    }
    change_colors(KRNL_WHITE, KRNL_BLACK);
    kprintf(" ] ");
    kprintf(string);
}

void change_colors(std::uint32_t fg, std::uint32_t bg)
{
    foreground = fg;
    foreground = bg;
}