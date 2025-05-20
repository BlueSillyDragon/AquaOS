#include <cstdint>
#include <inc/print.hpp>

char *panic_art = "   ____________    _______________________________\n"
"  /            \\   |                              |\n"
" /              \\  / Whoops! Kernel died...       |\n"
" |    `|    `|  | /_______________________________|\n"
" |     |     |  |\n"
" |         ^    |\n"
" |              |\n"
" |              |\n"
" \\              |\n"
"  \\            /\n"
"   \\          /\n"
"   /        _/\n"
"  /       _/\n"
" /      _/\n"
"/     _/\n"
"\\    /\n"
" \\__/";

__attribute__((noreturn))
extern "C" void interrupt_handler(void);
extern "C" void interrupt_handler() {
    kprintf("\nKERNEL PANIC!\nA Fatal Error occured and the kernel can no longer continue!\n");
    kprintf(panic_art);
    __asm__ volatile ("cli; hlt");
}