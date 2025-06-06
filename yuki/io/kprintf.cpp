#include <cstdarg>
#include <inc/io/terminal.hpp>
#include <inc/io/kprintf.hpp>

extern "C" Terminal kernTerminal;

#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_SMALL_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0

// Compile nanoprintf in this translation unit.
#define NANOPRINTF_IMPLEMENTATION
#include <inc/io/nanoprintf.hpp>

void kprintf(char *string, ...)
{
    char buf[256];

    std::va_list arg;
    va_start(arg, string);

    npf_snprintf(buf, sizeof(buf), string, arg);
    kernTerminal.termPrint(buf);
    
    va_end(arg);
}