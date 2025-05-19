#include <cstdint>
#include <inc/terminal.hpp>

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

extern "C" Terminal kern_terminal;

__attribute__((noreturn))
extern "C" void interrupt_handler(void);
extern "C" void interrupt_handler() {
    kern_terminal.term_print("\nKERNEL PANIC!\nA Fatal Error occured and the kernel can no longer continue!\n");
    kern_terminal.term_print(panic_art);
    __asm__ volatile ("cli; hlt");
}