#include <cstdarg>
#include <inc/terminal.hpp>
#include <inc/kprintf.hpp>

extern "C" Terminal kernTerminal;

void kprintf(char *string, ...) {
    std::va_list arg;
    va_start(arg, string);
    kernTerminal.termPrint(string, arg);
    va_end(arg);
}