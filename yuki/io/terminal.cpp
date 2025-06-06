#include <cstdint>
#include <cstdarg>
#include <inc/io/terminal.hpp>
#include <inc/io/krnl_font.hpp>
#include <inc/io/krnl_colors.hpp>
#include <inc/io/serial.hpp>

Terminal::Terminal()
{
    terminalFb.address = 0;
    terminalFb.pitch = 0;
    terminalForeground = 0;
    terminalBackground = 0;
}

void Terminal::termInit(limine_framebuffer *framebuffer, std::uint32_t foreground, std::uint32_t background)
{
    terminalFb.address = framebuffer->address;
    terminalFb.pitch = framebuffer->pitch;
    terminalFb.width = framebuffer->width;
    terminalFb.height = framebuffer->height;
    terminalForeground = foreground;
    terminalBackground = background;
    cursorX = 0;
    cursorY = 0;
}

void Terminal::plotPixels(std::uint64_t y, std::uint64_t x, uint32_t pixel)
{
    std::uint32_t *fb_ptr = reinterpret_cast<std::uint32_t *>(terminalFb.address);
    fb_ptr[x * (terminalFb.pitch / 4) + y] = pixel;
}

void Terminal::termPutchar(unsigned short int c)
{
    uint32_t pixel;
    
    for (int cy = 0; cy < 16; cy++) {
        for (int cx = 0; cx < 8; cx++) {
            pixel = (kernel_font[(c * 16) + cy] >> (7 - cx)) & 1 ? terminalForeground : terminalBackground;

            // Offset the cx and cy by x and y so that x and y are in characters instead of pixels
            plotPixels((cx + (cursorX * 8)), (cy + (cursorY * 16)), pixel);
        }
    }
}

void Terminal::termPrint(char *string, ...)
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
            cursorY++;
            cursorX = 0;
            writeSerial('\n');
            continue;
        }

        // Check if it's '\t' if it is, move the cursor foward by 6 spaces
        if (string[i] == 0x09) {
            cursorX += 6;
            writeSerial('\t');
            continue;
        }

        // Check the variable arguments
        if (string[i] == '%') {

            i++;

            if (string[i] == '%') {
                termPutchar('%');
                writeSerial('%');
                cursorX++;
                continue;
            }

            else if (string[i] == 'c')
            {
                char char_to_print = va_arg(argp, int);
                termPutchar(char_to_print);
                writeSerial(char_to_print);
                cursorX++;
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
                    termPutchar((number[j] + '0'));
                    writeSerial((number[j] + '0'));
                    cursorX++;
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
                        termPutchar((number[j] + ('0' + 7)));
                        writeSerial((number[j] + ('0' + 7)));
                    }
                    else 
                    {
                        termPutchar((number[j] + '0'));
                        writeSerial((number[j] + '0'));
                    }
                    cursorX++;
                }

                continue;
            }

            else if (string[i] == 's') {
                char *string_to_print = va_arg(argp, char*);

                while (*string_to_print != '\0')
                {
                    termPutchar(*string_to_print);
                    writeSerial(*string_to_print);
                    string_to_print++;
                    cursorX++;
                }
                continue;
            }

            else {
                termPutchar('0');
                writeSerial('0');
                cursorX++;
                continue;
            }

            i--;
        }

        termPutchar(string[i]);
        writeSerial(string[i]);

        cursorX++;
    }

    va_end(argp);
}

void Terminal::kerror(char *string)
{
    termPrint("[");
    writeSerial('[');
    changeColors(KRNL_RED, KRNL_DARK_GREY);
    stringToSerial("\033[31;1m");
    termPrint(" Error ");
    stringToSerial("\033[0m");
    changeColors(KRNL_WHITE, KRNL_DARK_GREY);
    termPrint("] ");
    termPrint(string);
}

void Terminal::ksuccess(char *string)
{
    termPrint("[");
    changeColors(KRNL_GREEN, KRNL_DARK_GREY);
    stringToSerial("\033[32;1m");
    termPrint(" OK ");
    stringToSerial("\033[0m");
    changeColors(KRNL_WHITE, KRNL_DARK_GREY);
    termPrint("] ");
    termPrint(string);
}

void Terminal::kinfo(INFO_TYPE type, char *string)
{
    termPrint("[ ");
    switch (type)
    {
        case PMM:
            changeColors(KRNL_BLUE, KRNL_DARK_GREY);
            stringToSerial("\033[34;1m");
            termPrint("PMM");
            break;
        case VMM:
            changeColors(KRNL_PINK, KRNL_DARK_GREY);
            stringToSerial("\033[95;1m");
            termPrint("VMM");
            break;
        case SCHEDULER:
            changeColors(KRNL_GREEN, KRNL_DARK_GREY);
            stringToSerial("\033[32;1m");
            termPrint("SCHEDULER");
            break;
        default:
        termPrint(" ");
    }
    stringToSerial("\033[0m");
    changeColors(KRNL_WHITE, KRNL_DARK_GREY);
    termPrint(" ] ");
    termPrint(string);
}

void Terminal::changeColors(std::uint32_t foreground, std::uint32_t background)
{
    terminalForeground = foreground;
    terminalBackground = background;
}

void Terminal::clearScreen()
{
    for (uint64_t i = 0; i < terminalFb.width; i++)
    {
        for (uint64_t j = 0; j < terminalFb.height; j++)
        {
            plotPixels(i, j, terminalBackground);
        }
    }
}