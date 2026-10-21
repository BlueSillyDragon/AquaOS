/**
Snow Operating System
Copyright (c) UtsumiFuyuki 2025, 2026
 
File: hal/hal.cpp

Description:
This file is the main source file of
Hardware Abstraction Layer module of Yuki

Author:
UtsumiFuyuki
October 28th 2025
**/

#include <cstdint>
#include <limine.h>
#include <flanterm.h>
#include <flanterm_backends/fb.h>
#include <hal/hal.hpp>
#include <hal/serial.hpp>
#include <hal/amd64/cpuid.hpp>
#include <hal/amd64/fred.hpp>
#include <ke/string.hpp>
#include <ke/log.hpp>
#include <ke/spinlock.hpp>
#include <mm/mm.hpp>

// Limine Stuff

// Limine Base Revision, set to 6, the most recent revision
namespace {
    __attribute__((used, section(".limine_requests")))
    volatile uint64_t limineBaseRevision[] = LIMINE_BASE_REVISION(6);
}

// The Limine requests. Basically tells the bootloader what we want from it :p

namespace {
    __attribute__((used, section(".limine_requests")))
    volatile limine_bootloader_info_request limineBootInfoRequest = {
        .id = LIMINE_BOOTLOADER_INFO_REQUEST_ID,
        .revision = 0,
        .response = nullptr
    };

    __attribute__((used, section(".limine_requests")))
    volatile limine_framebuffer_request limineFramebufferRequest = {
        .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
        .revision = 0,
        .response = nullptr
    };

    volatile limine_memmap_request limineMemoryMapRequest = {
        .id = LIMINE_MEMMAP_REQUEST_ID,
        .revision = 0,
        .response = nullptr
    };

    volatile limine_hhdm_request limineHhdmRequest = {
        .id = LIMINE_HHDM_REQUEST_ID,
        .revision = 0,
        .response = nullptr
    };

    volatile limine_mp_request limineMpRequest = {
        .id = LIMINE_MP_REQUEST_ID,
        .revision = 0,
        .response = nullptr
    };

    volatile limine_rsdp_request limineRsdpRequest = {
        .id = LIMINE_RSDP_REQUEST_ID,
        .revision = 0,
        .response = nullptr
    };

    volatile limine_executable_file_request limineExecutableRequest = {
        .id = LIMINE_EXECUTABLE_FILE_REQUEST_ID,
        .revision = 0,
        .response = nullptr
    };

    volatile limine_executable_address_request limineExecutableAddressRequest = {
        .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST_ID,
        .revision = 0,
        .response = nullptr
    };

    volatile limine_module_request limineModuleRequest = {
        .id = LIMINE_MODULE_REQUEST_ID,
        .revision = 0,
        .response = nullptr,

        .internal_module_count = 0,
        .internal_modules = nullptr
    };
}

namespace {
    __attribute__((used, section(".limine_requests_start")))
    volatile uint64_t LimineRequestsStartMarker[] = LIMINE_REQUESTS_START_MARKER;

    __attribute__((used, section(".limine_requests_end")))
    volatile uint64_t LimineRequestsEndMarker[] = LIMINE_REQUESTS_END_MARKER;
}

struct flanterm_context* flantermContext;

uint32_t terminalForeground = 0xFFFFFF;
uint32_t terminalBackground = 0x23272E;

extern "C" void ReloadSegments();
extern "C" void loadTss(uint32_t);
extern "C" void *isrStubTable[];

TSS tss;

GDT gdt {0, KERNEL_CS, KERNEL_DS, USER_DS, USER_CS};
DTR gdtr;

IDT_ENTRY idt[256];
DTR idtr;

bool fbExists{};
limine_framebuffer *framebuffer;

// The following stubs are required by the Itanium C++ ABI (the one we use,
// regardless of the "Itanium" nomenclature).
// Like the memory functions above, these stubs can be moved to a different .cpp file,
// but should not be removed, unless you know what you are doing.
extern "C" {
    int __cxa_atexit(void (*)(void *), void *, void *) { return 0; }
    void __cxa_pure_virtual() { hal::haltCpu(); }
    void *__dso_handle;
}

void halIdtSetDescriptor(uint8_t vector, void * isr, uint8_t flags) {
    IDT_ENTRY* descriptor = &idt[vector];

    descriptor->isrLow              = (uint64_t)isr & 0xFFFF;
    descriptor->segmentSelector     = 0x08;
    descriptor->ist                 = 0;
    descriptor->attributes          = flags;
    descriptor->isrMid              = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isrHigh             = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved            = 0;
}

extern "C" bool halInterruptsEnabled()
{
    uint64_t enabled{};
    __asm__ volatile (
                    "pushfq;"
                    "pop %%rax;"
                    "shr $9, %%rax;"
                    "and $1, %%rax;"
                    "mov %%rax, %0" : "=c"(enabled));
    return enabled;
}

// TODO: remove usage of ke::log()

void hal::initialize()
{
    // TODO: make this bootloader agnostic
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED(limineBaseRevision) == false) {
        hal::haltCpu();
    }

    hal::initializeSerial(COM1);

    // Ensure we got a framebuffer.
    if (limineFramebufferRequest.response == nullptr
     || limineFramebufferRequest.response->framebuffer_count < 1) {
        hal::writeStringToSerial(COM1, "No framebuffer could be found!");
        fbExists = false;
    }
    else
        fbExists = true;

    // Fetch the first framebuffer.
    framebuffer = limineFramebufferRequest.response->framebuffers[0];

    flantermContext = flanterm_fb_init(
        NULL,
        NULL,
        reinterpret_cast<uint32_t *>(framebuffer->address), framebuffer->width, framebuffer->height, framebuffer->pitch,
        framebuffer->red_mask_size, framebuffer->red_mask_shift,
        framebuffer->green_mask_size, framebuffer->green_mask_shift,
        framebuffer->blue_mask_size, framebuffer->blue_mask_shift,
        NULL,
        NULL, NULL,
        &terminalBackground, &terminalForeground,
        NULL, NULL,
        NULL, 0, 0, 1,
        0, 0,
        0,
        0
    );

    hal::writeStringToSerial(COM1, "HAL has initialized!\r\n");
}

SPINLOCK lock{};

void cpuTest() {
    bool ints = ke::spinlockAcquire(&lock);
    ke::print("This is a CPU test! ^w^");
    ke::print("Allocating a page...\r\n");
    uintptr_t page = mm::allocatePage();
    ke::print("Allocated page at 0x%llX\r\n", page);
    ke::spinlockRelease(&lock, ints);
    return;
}

void cpuStart(limine_mp_info *MpInfo)
{
    bool intsEnabled = ke::spinlockAcquire(&lock);

    __asm__ volatile ("lgdt %0" :: "m"(gdtr));
    ReloadSegments();
    __asm__ volatile ("lidt %0" :: "m"(idtr));

    ke::print("CPU startup complete!\r\n");
    ke::spinlockRelease(&lock, intsEnabled);

    cpuTest();
    hal::haltCpu();
}

void hal::printString(const char *String)
{
    if (fbExists)
        flanterm_write(flantermContext, String, strlen(String));
    hal::writeStringToSerial(COM1, String);
}

void hal::haltCpu()
{
    for (;;)
    {
        #if defined (__x86_64__)
            __asm__ volatile ("hlt");
        #endif
    }
}

void hal::initCpu()
{
    // Setup the GDT
    uint32_t tssLimit = sizeof(TSS);
    uint64_t tssBase = reinterpret_cast<uint64_t>(&tss);

    gdt.tssDescriptor.limit1 = static_cast<uint16_t>(tssLimit & 0xFFFF);
    gdt.tssDescriptor.base1 = static_cast<uint16_t>(tssBase & 0xFFFF);
    gdt.tssDescriptor.base2 = static_cast<uint16_t>((tssBase >> 16) & 0xFFFF);
    gdt.tssDescriptor.accessByte = 0x89;
    gdt.tssDescriptor.flags = (tssLimit >> 16) & 0xF;
    gdt.tssDescriptor.base3 = static_cast<uint8_t>(tssBase >> 24);
    gdt.tssDescriptor.base4 = static_cast<uint32_t>(tssBase >> 32);

    gdtr.base = reinterpret_cast<uintptr_t>(&gdt);
    gdtr.limit = (sizeof(gdt) - 1);

    __asm__ volatile ("lgdt %0" :: "m"(gdtr));
    ReloadSegments();
    loadTss(0x28);

    CPUID_REGISTERS cpuid = hal::x64::getCpuid(7, 1);
    uint8_t fredBit = (cpuid.rax >> 17) & 0x1;

    if (fredBit == 0) {
        ke::log(__FILE__, "CPU does not support FRED!\r\n");
        // Setup the IDT
        idtr.base = (uintptr_t)&idt;
        idtr.limit = (uint16_t)sizeof(IDT_ENTRY) * 256 - 1;

        // Exceptions
        for(int i = 0; i < 32; i++)
        {
            halIdtSetDescriptor(i, isrStubTable[i], 0x8e);
        }

        // Timer Int
        halIdtSetDescriptor(47, isrStubTable[32], 0x8e);
        halIdtSetDescriptor(48, isrStubTable[33], 0x8e);

        __asm__ volatile ("lidt %0" :: "m"(idtr));
    }
    else {
        ke::log(__FILE__, "CPU supports FRED!\r\n");
        hal::x64::initializeFred();
    }

    ke::print("CPU Initialized!\r\n");
}

// TODO: abstract this
void hal::setRsp0(uintptr_t rsp0) {
    tss.rsp0 = rsp0;
}

void hal::initSmp()
{
    // TODO: Maybe manual AP init? I don't know if I'll have Snowboot start-up the other cores yet.

    limine_mp_response *mpResponse = limineMpRequest.response;
    if (mpResponse->cpu_count == 1)
    {
        ke::print("Running on a Uniprocesser System!\r\n");
        return;
    }

    ke::print("Running with %llu processors\r\n", mpResponse->cpu_count);

    void (*cpuStartAddress)(limine_mp_info *MpInfo) = cpuStart;

    for (uint64_t i = 1; i < mpResponse->cpu_count; i++)
    {
        mpResponse->cpus[i]->goto_address = cpuStartAddress;
    }
}

void hal::setupSyscall() {

}

const char *hal::blVersion()
{
    return limineBootInfoRequest.response->version;
}

uint64_t hal::retrieveHhdmOffset()
{
    return limineHhdmRequest.response->offset;
}

limine_memmap_response *hal::retrieveMemoryMap()
{
    limine_memmap_response *memoryMap = limineMemoryMapRequest.response;
    return memoryMap;
}

uintptr_t hal::retrieveRsdpPhysicalAddress()
{
    return reinterpret_cast<uintptr_t>(limineRsdpRequest.response->address) - hal::retrieveHhdmOffset();
}

limine_file *hal::retrieveYukiImage() {
    return limineExecutableRequest.response->executable_file;
}

uintptr_t hal::yukiPhysicalAddress() {
    return limineExecutableAddressRequest.response->physical_base;
}

uintptr_t hal::yukiVirtualAddress() {
    return limineExecutableAddressRequest.response->virtual_base;
}

limine_module_response *hal::retrieveModules() {
    return limineModuleRequest.response;
}