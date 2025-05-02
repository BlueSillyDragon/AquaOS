#include <inc/terminal.hpp>

extern "C" Terminal kern_terminal;

__attribute__((noreturn))
extern "C" void interrupt_handler(void);
extern "C" void interrupt_handler() {
    __asm__ volatile ("cli; hlt");
}