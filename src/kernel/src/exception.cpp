#include <aquaboot.h>
#include <inc/krpint.hpp>

__attribute__((noreturn))
extern "C" void exception_handler() {
    kprintf("KERNEL PANIC!\n");
    asm volatile ("cli; hlt"); // Completely hangs the computer
}