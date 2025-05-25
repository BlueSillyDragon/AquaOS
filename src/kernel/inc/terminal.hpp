#pragma once

#include <snowboot.h>
#include <cstdint>
#include <cstdarg>

enum INFO_TYPE {PMM, VMM, SCHEDULER};

class Terminal
{
    public:
        Terminal();
        void termInit(snowboot_framebuffer *framebuffer, std::uint32_t foreground, std::uint32_t background, std::uint64_t hhdm);
        void termPrint(char *string, ...);
        void kerror(char *string);
        void ksuccess(char *string);
        void kinfo(INFO_TYPE type, char *string);   // Labels the string for different parts of the kernel (eg. [PMM])
        void changeColors(std::uint32_t foreground, std::uint32_t background);
        void clearScreen();
    private:
        std::uint32_t terminalForeground;
        std::uint32_t terminalBackground;
        std::uint64_t cursorX;
        std::uint64_t cursorY;
        snowboot_framebuffer terminalFb;
        void plotPixels(std::uint64_t y, std::uint64_t x, uint32_t pixel);
        void termPutchar(unsigned short int c);
};