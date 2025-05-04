#include <cstdint>
#include <inc/terminal.hpp>

extern "C" Terminal kern_terminal;

__attribute__((noreturn))
extern "C" void interrupt_handler(void);
extern "C" void interrupt_handler() {
    kern_terminal.term_print("KERNEL PANIC!\nA Fatal Error occured and the kernel can no longer continue!\n");
    __asm__ volatile ("cli; hlt");
}