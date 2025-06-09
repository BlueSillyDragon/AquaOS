#include <cstdint>
#include <limine.h>
#include <inc/io/terminal.hpp>
#include <inc/klib/string.hpp>
#include <inc/io/serial.hpp>
#include <inc/mm/pmm.hpp>
#include <inc/mm/vmm.hpp>
#include <stdint.h>

#define VIRT_MASK 0x0000ffffffffffff
#define PT_ADDR_MASK 0x0000fffffffff000

#define PT_IS_PRESENT(pt) (pt & (1 << 0))

#define PML4_ID(virt) (((virt) >> 39) & 0x1FF)
#define PDPT_ID(virt) (((virt) >> 30) & 0x1FF)
#define PD_ID(virt) (((virt) >> 21) & 0x1FF)
#define PT_ID(virt) (((virt) >> 12) & 0x1FF)

extern "C" Terminal kernTerminal;

pagemap_t pagemap;

uint64_t hhdmOffset;

uint64_t kernelVirt = 0xffffffff80000000;

uint64_t getLowerLevel(uint64_t *currentLevel, uint64_t entry)
{
    uint64_t nextLevel;
    if(PT_IS_PRESENT(currentLevel[entry]))
    {
        nextLevel = (currentLevel[entry] & PT_ADDR_MASK);
    }

    else
    {
        nextLevel = pmmAlloc();
        KLib::memset(reinterpret_cast<uint64_t *>(nextLevel + hhdmOffset), 0x0, 0x1000);
        currentLevel[entry] = (nextLevel | (1 << 0));
    }
    return nextLevel;
}

uint64_t virtToPhys(uint64_t virtualAddr)
{
    uint64_t pml4Idx = PML4_ID(virtualAddr);
    uint64_t pml3Idx = PDPT_ID(virtualAddr);
    uint64_t pml2Idx = PD_ID(virtualAddr);
    uint64_t pml1Idx = PT_ID(virtualAddr);

    uint64_t *pml4 = reinterpret_cast<uint64_t *>(pagemap.topLevel + hhdmOffset);
    uint64_t *pml3 = reinterpret_cast<uint64_t *>(getLowerLevel(pml4, pml4Idx) + hhdmOffset);
    uint64_t *pml2 = reinterpret_cast<uint64_t *>(getLowerLevel(pml3, pml3Idx) + hhdmOffset);
    uint64_t *pml1 = reinterpret_cast<uint64_t *>(getLowerLevel(pml2, pml2Idx) + hhdmOffset);

    return (pml1[pml1Idx] & PT_ADDR_MASK);
}

void initVmm(limine_memmap_response *memoryMap, std::uint64_t hhdm)
{
    kernTerminal.kinfo(VMM, "Initializing VMM...\n");
    hhdmOffset = hhdm;
    pagemap.topLevel = pmmAlloc();
    KLib::memset(reinterpret_cast<uint64_t *>(pagemap.topLevel + hhdm), 0x0, 0x1000);

    uint64_t oldCr3 = 0;

    __asm__ volatile ("mov %%cr3, %%rax; mov %%rax, %0" : "=a"(oldCr3));

    for (uint64_t i = 0; i < 512; i++)
    {
        if (PT_IS_PRESENT(reinterpret_cast<uint64_t *>(oldCr3 + hhdm)[i]))
        {
            reinterpret_cast<uint64_t *>(pagemap.topLevel + hhdm)[i] = reinterpret_cast<uint64_t *>(oldCr3 + hhdm)[i];
        }
    }

    __asm__ volatile ("mov %0, %%rax; mov %%rax, %%cr3" :: "a"(pagemap.topLevel));

    kernTerminal.kinfo(VMM, "VMM Initialized!\n");
}

void mapPage(uint64_t virtualAddr, uint64_t physicalAddr, uint8_t flags)
{
    if (virtualAddr % 0x1000 != 0 || physicalAddr % 0x1000 != 0)
    {
        kernTerminal.kerror("Attempted to map a virtual address or physical address that was not aligned to 4KB!\n");
        __asm__ volatile (" hlt ");
    }

    uint64_t pml4Idx = PML4_ID(virtualAddr);
    uint64_t pml3Idx = PDPT_ID(virtualAddr);
    uint64_t pml2Idx = PD_ID(virtualAddr);
    uint64_t pml1Idx = PT_ID(virtualAddr);

    uint64_t *pml4 = reinterpret_cast<uint64_t *>(pagemap.topLevel + hhdmOffset);
    uint64_t *pml3 = reinterpret_cast<uint64_t *>(getLowerLevel(pml4, pml4Idx) + hhdmOffset);
    uint64_t *pml2 = reinterpret_cast<uint64_t *>(getLowerLevel(pml3, pml3Idx) + hhdmOffset);
    uint64_t *pml1 = reinterpret_cast<uint64_t *>(getLowerLevel(pml2, pml2Idx) + hhdmOffset);

    pml1[pml1Idx] = (physicalAddr | flags);
    kernTerminal.termPrint("Page now mapped: 0x%x\n", (physicalAddr | flags));
}

void mapPages(uint64_t virtualStart, uint64_t physicalStart, uint8_t flags, uint64_t count)
{
    if (virtualStart % 0x1000 != 0 || physicalStart % 0x1000 != 0 || count % 0x1000 != 0)
    {
        kernTerminal.kerror("Attempted to map multiple virtual addresses or physical addresses or count that was not aligned to 4KB!\n");
        __asm__ volatile (" hlt ");
    }

    for (uint64_t i = 0; i < count; ) {
        mapPage(virtualStart + i, physicalStart + i, flags);
        i += 0x1000;
    }
}