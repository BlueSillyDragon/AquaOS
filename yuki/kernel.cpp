#include <cstdint>
#include <cstddef>
#include <limine.h>
#include <inc/klibc/string.hpp>
#include <inc/io/terminal.hpp>
#include <inc/io/serial.hpp>
#include <inc/io/krnl_colors.hpp>
#include <inc/io/logo.hpp>
#include <inc/sys/gdt.hpp>
#include <inc/sys/idt.hpp>
#include <inc/mm/pmm.hpp>
#include <inc/mm/vmm.hpp>

#define KERNEL_MAJOR 0
#define KERNEL_MINOR 1
#define KERNEL_PATCH 0  

// Set the base revision to 3, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

namespace {

__attribute__((used, section(".limine_requests")))
volatile LIMINE_BASE_REVISION(3);

}

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

namespace {

__attribute__((used, section(".limine_requests")))
volatile limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
    .response = nullptr
};

__attribute__((used, section(".limine_requests")))
volatile limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
    .response = nullptr
};

__attribute__((used, section(".limine_requests")))
volatile limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0,
    .response = nullptr
};

}

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .cpp file, as seen fit.

namespace {

__attribute__((used, section(".limine_requests_start")))
volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
volatile LIMINE_REQUESTS_END_MARKER;

}

// Halt and catch fire function.
namespace {

void hcf() {
    for (;;) {
        asm ("hlt");
    }
}

}

// The following stubs are required by the Itanium C++ ABI (the one we use,
// regardless of the "Itanium" nomenclature).
// Like the memory functions above, these stubs can be moved to a different .cpp file,
// but should not be removed, unless you know what you are doing.
extern "C" {
    int __cxa_atexit(void (*)(void *), void *, void *) { return 0; }
    void __cxa_pure_virtual() { hcf(); }
    void *__dso_handle;
}

// Extern declarations for global constructors array.
extern void (*__init_array[])();
extern void (*__init_array_end[])();

Terminal kernTerminal;

uint64_t hhdm;

extern "C" void kernelMain()
{
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    // Call global constructors.
    for (std::size_t i = 0; &__init_array[i] != __init_array_end; i++) {
        __init_array[i]();
    }

    if (hhdm_request.response == nullptr)
    {
        hcf();
    }

    hhdm = hhdm_request.response->offset;

    if (serialInit() == 1)
    {
        hcf();
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == nullptr
     || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    kernTerminal.termInit(framebuffer, KRNL_WHITE, KRNL_DARK_GREY);
    kernTerminal.clearScreen();

    kernTerminal.termPrint(kernel_logo);
    kernTerminal.termPrint("\n\tYuki Version %d.%d.%d\n\n", KERNEL_MAJOR, KERNEL_MINOR, KERNEL_PATCH);

    initGdt();
    initIdt();

    // Ensure we have a Memory Map
    if (memmap_request.response == nullptr)
    {
        hcf();
    }

    limine_memmap_response *memoryMap = memmap_request.response;

    initPmm(memoryMap, hhdm);

    std::uint64_t test = pmmAlloc();

    // We're done, just hang...
    hcf();
}