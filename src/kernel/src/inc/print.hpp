#pragma once

#include <aquaboot.h>
#include <cstdint>
#include <cstdarg>

enum INFO_TYPE {PMM, VMM, SCHEDULER};

class Terminal
{
    public:
        Terminal();
        void term_init(aquaboot_framebuffer *framebuffer, std::uint32_t foreground, std::uint32_t background, std::uint64_t hhdm);
        void term_print(char *string, ...);
        void kerror(char *string);
        void ksuccess(char *string);
        void kinfo(INFO_TYPE type, char *string);   // Labels the string for different parts of the kernel (eg. [PMM])
        void change_colors(std::uint32_t foreground, std::uint32_t background);
    private:
        std::uint32_t terminal_foreground;
        std::uint32_t terminal_background;
        std::uint64_t cursor_x;
        std::uint64_t cursor_y;
        aquaboot_framebuffer terminal_fb;
        void plot_pixels(std::uint64_t y, std::uint64_t x, uint32_t pixel);
        void term_putchar(unsigned short int c);
};