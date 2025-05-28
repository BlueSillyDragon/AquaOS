#include <cstdint>
#include <snowboot.h>
#include <cstdarg>
#include <inc/krnl_colors.hpp>
#include <inc/logo.hpp>
#include <inc/terminal.hpp>
#include <inc/sys/gdt.hpp>
#include <inc/sys/idt.hpp>
#include <inc/mm/pmm.hpp>
#include <inc/mm/vmm.hpp>

#define KERNEL_MAJOR 0
#define KERNEL_MINOR 1
#define KERNEL_PATCH 0  

void hlt()
{
    asm volatile(" hlt ");
}

Terminal kernTerminal;

extern "C" void kernelMain (snowboot_info *bootInfo)
{
    kernTerminal.termInit(bootInfo->framebuffer, KRNL_WHITE, KRNL_DARK_GREY, bootInfo->hhdm);
    kernTerminal.clearScreen();

    kernTerminal.termPrint(kernel_logo);

    kernTerminal.termPrint("\n\n\tBooted by SnowBoot Version %d.%d.%d\n", bootInfo->snowbootMajor, bootInfo->snowbootMinor, bootInfo->snowbootPatch);
    kernTerminal.termPrint("\n\tYuki Version %d.%d.%d\n\n", KERNEL_MAJOR, KERNEL_MINOR, KERNEL_PATCH);

    initGdt();
    initIdt();
    initPmm(bootInfo->memoryMap, bootInfo->memMapEntries, bootInfo->descSize, bootInfo->hhdm);

    uint64_t test = pmmAlloc();

    hlt();
}