#include <snowboot.h>
#include <cstdint>
#include <inc/terminal.hpp>
#include <inc/mm/pmm.hpp>

extern "C" Terminal kernTerminal;

uint64_t head;

uint64_t hhdm_offset;      // We need this to actually access the page, so that we can retrieve the next page in the free list

char *memTypeToString(std::uint32_t memType)
{
    switch (memType)
    {
        case SNOWOS_RESERVED:
            return "Reserved";
        case SNOWOS_RUNTIME_SERVICES:
            return "UEFI Runtime Services";
        case SNOWOS_ACPI_RECLAIM:
            return "ACPI Reclaimable Memory";
        case SNOWOS_BOOT_RECLAIM:
            return "Bootloader Reclaimable Memory";
        case SNOWOS_FREE_MEMORY:
            return "Free Memory";
        default:
            return "Unknown Memory Type";
    }
}

void initPmm(snowboot_memory_descriptor *memoryMap, std::uint64_t entries, std::uint64_t desc_size, uint64_t hhdm)
{
    kernTerminal.kinfo(PMM, "Initialzing PMM...\n");

    uint64_t nop = 0;
    uint64_t next = 0;
    hhdm_offset = hhdm;
    snowboot_memory_descriptor *desc;

    for(uint64_t i = 0; i < entries; i++)
    {
        static bool setEnd = false;

        desc = (snowboot_memory_descriptor *)((uint8_t *)memoryMap + (i * desc_size));

        if (desc->type == SNOWOS_FREE_MEMORY)
        {
            nop += desc->numOfPages;

            // Loop through all pages in memory area, and link them together
            for (uint64_t j = 0; j < desc->numOfPages; j++)
            {
                if (setEnd)
                {
                    *reinterpret_cast<uint64_t *>(hhdm + next) = desc->physicalStart;
                    setEnd = false;
                }

                next = (desc->physicalStart + j * 0x1000);

                *reinterpret_cast<uint64_t *>(hhdm + next) = (next + 0x1000);

                static bool headSet = true;
                if (headSet)
                {
                    head = next;
                    headSet = false;
                }
            }
            setEnd = true;
        }
    }
    end:
    *reinterpret_cast<uint64_t *>(hhdm + next) = 0xcafebabe;
    if (((nop * 4) / 1024) < 400)   // Not all of RAM is usable, give some account for this
    {
        kernTerminal.kerror("Please run SnowOS with atleast 512MB of RAM!\n");
        __asm__ volatile (" hlt ");
    }

    kernTerminal.kinfo(PMM, "SnowOS has ");
    kernTerminal.termPrint("%dGB of memory available\n", ((nop * 4) / 1024) / 1024);
    kernTerminal.kinfo(PMM, "Usable Pages: ");
    kernTerminal.termPrint("%d\n", nop);
}

uint64_t pmmAlloc()
{
    uint64_t page = head;
    head = *reinterpret_cast<uint64_t *>(hhdm_offset + head);

    kernTerminal.termPrint("Memory allocated at 0x%x, Head now points to 0x%x\n", page, head);

    return page;
}

void pmmFree(uint64_t page)
{
    
}