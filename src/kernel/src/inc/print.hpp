#pragma once

#include <aquaboot.h>
#include <cstdint>
#include <cstdarg>

enum INFO_TYPE {PMM, VMM, SCHEDULER};

void term_init(aquaboot_framebuffer *framebuffer, std::uint32_t foreground, std::uint32_t background, std::uint64_t hhdm);
void putchar(char c);
void kprintf(char *string, ...);
void kerror(char *string);
void ksuccess(char *string);
void kinfo(INFO_TYPE type, char *string);   // Labels the string for different parts of the kernel (eg. [PMM])
void change_colors(std::uint32_t fg, std::uint32_t bg);