#include "aquaboot.h"
#include "inc/print.hpp"
#include <cstdint>
#include <inc/print.hpp>
#include <inc/mm/pmm.hpp>

extern "C" Terminal kern_terminal;

uint64_t *head;

void init_pmm(aquaboot_memory_descriptor *memory_map, std::uint64_t entries, std::uint64_t desc_size, uint64_t hhdm)
{
    kern_terminal.kinfo(PMM, "Initialzing PMM...\n");

    uint64_t *next;
    uint64_t nop = 0;
    bool start = true;  // This is used for setting the head, so we only set it the first iteration
    bool free_end = false;  // Tells us if we've hit the end, in which case we need make sure we link the next
    aquaboot_memory_descriptor *desc;

    for(uint64_t i = 0; i < entries; i++)
    {
        desc = (aquaboot_memory_descriptor *)((uint8_t *)memory_map + (i * desc_size));
        if (desc->type == AQUAOS_FREE_MEMORY)
        {
            nop += desc->num_of_pages;

            // Loop through all pages in memory area, and link them together
            for (uint64_t j = 0; j < desc->num_of_pages; j++)
            {
                if (free_end)
                {
                    *next = (hhdm + desc->physical_start);
                    free_end = false;   // Be sure to set this back
                }
                next = reinterpret_cast<uint64_t *>(hhdm + desc->physical_start + (j * 0x1000));
                if (reinterpret_cast<std::uint64_t>(next) >= 0xffff8000fffff000)    // TODO: Figure out a better way of doing this lol
                {
                    goto link_end;
                }
                *next = reinterpret_cast<uint64_t>((uint8_t *)next + 0x1000);
                if(start)
                {
                    head = next;
                    start = false;
                }
            }
            free_end = true;
        }
    }
    *next = 0;
    link_end:
    if (((nop * 4) / 1024) < 400)   // Not all of RAM is usable, give some account for this
    {
        kern_terminal.kerror("Please run AquaOS with atleast 512MB of RAM!\n");
        __asm__ volatile (" hlt ");
    }

    kern_terminal.kinfo(PMM, "AquaOS has ");
    kern_terminal.term_print("%dGB of memory available\n", ((nop * 4) / 1024) / 1024);
    kern_terminal.kinfo(PMM, "Usable Pages: ");
    kern_terminal.term_print("%d\n", nop);
}

uint64_t *pmm_alloc()
{
    uint64_t *page;     // The page we're returning
    uint64_t *next;     // Used for getting the next page
    kern_terminal.kinfo(PMM, "Allocated a free page at ");
    kern_terminal.term_print("0x%x\n", head);

    // Remove page from free list
    next = head;
    page = head;
    head = reinterpret_cast<uint64_t *>(*next);

    return page;
}

void pmm_free(uint64_t *page)
{
    *page = reinterpret_cast<uint64_t>(head);   // The newly freed page now points to the old head
    head = page;    // Head now points to newly freed page
}