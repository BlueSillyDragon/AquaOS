#pragma once

extern "C" void putchar (unsigned short int c, uint32_t fg, uint32_t bg);
extern "C" void kprintf(char* string, ...);
extern "C" void display_logo();
extern "C" void kerror(char * string);