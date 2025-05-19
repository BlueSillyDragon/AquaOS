#include <cstdint>
#include <cstdarg>
#include <inc/terminal.hpp>
#include <inc/krnl_font.hpp>
#include <inc/krnl_colors.hpp>

Terminal::Terminal()
{
    terminal_fb.base = 0;
    terminal_fb.pitch = 0;
    terminal_foreground = 0;
    terminal_background = 0;
}

void Terminal::term_init(aquaboot_framebuffer *framebuffer, std::uint32_t foreground, std::uint32_t background, std::uint64_t hhdm)
{
    terminal_fb.base = (hhdm + framebuffer->base);
    terminal_fb.pitch = framebuffer->pitch;
    terminal_fb.horizontalRes = framebuffer->horizontalRes;
    terminal_fb.verticalRes = framebuffer->verticalRes;
    terminal_foreground = foreground;
    terminal_background = background;
    cursor_x = 0;
    cursor_y = 0;
}

void Terminal::plot_pixels(std::uint64_t y, std::uint64_t x, uint32_t pixel)
{
    std::uint32_t *fb_ptr = reinterpret_cast<std::uint32_t *>(terminal_fb.base);
    fb_ptr[x * (terminal_fb.pitch / 4) + y] = pixel;
}

void Terminal::term_putchar(unsigned short int c)
{
    uint32_t pixel;
    
    for (int cy = 0; cy < 16; cy++) {
        for (int cx = 0; cx < 8; cx++) {
            pixel = (kernel_font[(c * 16) + cy] >> (7 - cx)) & 1 ? terminal_foreground : terminal_background;

            // Offset the cx and cy by x and y so that x and y are in characters instead of pixels
            plot_pixels((cx + (cursor_x * 8)), (cy + (cursor_y * 16)), pixel);
        }
    }
}

void Terminal::term_print(char *string, ...)
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
                term_putchar('%');
                cursor_x++;
                continue;
            }

            else if (string[i] == 'c')
            {
                char char_to_print = va_arg(argp, int);
                term_putchar(char_to_print);
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
                    term_putchar((number[j] + '0'));
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
                        term_putchar((number[j] + ('0' + 7)));
                    }
                    else term_putchar((number[j] + '0'));
                    cursor_x++;
                }

                continue;
            }

            else if (string[i] == 's') {
                char *string_to_print = va_arg(argp, char*);

                while (*string_to_print != '\0')
                {
                    term_putchar(*string_to_print);
                    string_to_print++;
                    cursor_x++;
                }
                continue;
            }

            else {
                term_putchar('0');
                cursor_x++;
                continue;
            }

            i--;
        }

        term_putchar(string[i]);

        cursor_x++;
    }

    va_end(argp);
}

void Terminal::kerror(char *string)
{
    term_print("[");
    change_colors(KRNL_RED, KRNL_BLACK);
    term_print(" Error ");
    change_colors(KRNL_WHITE, KRNL_BLACK);
    term_print("] ");
    term_print(string);
}

void Terminal::ksuccess(char *string)
{
    term_print("[");
    change_colors(KRNL_GREEN, KRNL_BLACK);
    term_print(" OK ");
    change_colors(KRNL_WHITE, KRNL_BLACK);
    term_print("] ");
    term_print(string);
}

void Terminal::kinfo(INFO_TYPE type, char *string)
{
    term_print("[ ");
    switch (type)
    {
        case PMM:
            change_colors(KRNL_BLUE, KRNL_BLACK);
            term_print("PMM");
            break;
        case VMM:
            change_colors(KRNL_PINK, KRNL_BLACK);
            term_print("VMM");
            break;
        case SCHEDULER:
            change_colors(KRNL_GREEN, KRNL_BLACK);
            term_print("SCHEDULER");
            break;
        default:
        term_print(" ");
    }
    change_colors(KRNL_WHITE, KRNL_BLACK);
    term_print(" ] ");
    term_print(string);
}

void Terminal::change_colors(std::uint32_t foreground, std::uint32_t background)
{
    terminal_foreground = foreground;
    terminal_background = background;
}