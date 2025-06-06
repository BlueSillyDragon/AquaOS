#include <limine.h>
#include <cstdint>
#include <inc/io/terminal.hpp>
#include <inc/mm/pmm.hpp>

extern "C" Terminal kernTerminal;

uint64_t head;

uint64_t hhdm_offset;      // We need this to actually access the page, so that we can retrieve the next page in the free list

char *memTypeToString(std::uint64_t memType)
{
    switch (memType)
    {
        case LIMINE_MEMMAP_RESERVED:
            return "Reserved";
        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
            return "ACPI Reclaimable Memory";
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            return "Bootloader Reclaimable Memory";
        case LIMINE_MEMMAP_USABLE:
            return "Free Memory";
        case LIMINE_MEMMAP_KERNEL_AND_MODULES:
            return "Kernel and Modules";
        default:
            return "Unknown Memory Type";
    }
}

void initPmm(limine_memmap_response *memoryMap, uint64_t hhdm)
{
    kernTerminal.kinfo(PMM, "Initialzing PMM...\n");

    uint64_t nop = 0;
    uint64_t next = 0;
    hhdm_offset = hhdm;

    for(uint64_t i = 0; i < memoryMap->entry_count; i++)
    {
        static bool setEnd = false;

        if (memoryMap->entries[i]->type == LIMINE_MEMMAP_USABLE)
        {
            nop += (memoryMap->entries[i]->length / 0x1000);

            // Loop through all pages in memory area, and link them together
            for (uint64_t j = 0; j < (memoryMap->entries[i]->length / 0x1000); j++)
            {
                if (setEnd)
                {
                    *reinterpret_cast<uint64_t *>(hhdm + next) = memoryMap->entries[i]->base;
                    setEnd = false;
                }

                next = (memoryMap->entries[i]->base + j * 0x1000);

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

    //kernTerminal.termPrint("Memory allocated at 0x%x, Head now points to 0x%x\n", page, head);

    return page;
}

void pmmFree(uint64_t page)
{
    
}