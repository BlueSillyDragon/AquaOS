#include <efi.h>
#include <stddef.h>
#include <stdint.h>
#include "inc/log.h"
#include "inc/memory_services.h"
#include "inc/virtual_memory.h"

#define VIRT_MASK 0x0000ffffffffffff

#define PT_TABLE_FLAGS   (PT_VALID | PT_WRITE | PT_USER)
#define PT_VALID ((uint64_t)1 << 0)
#define PT_WRITE ((uint64_t)1 << 1)
#define PT_USER ((uint64_t)1 << 2)
#define PT_LARGE ((uint64_t)1 << 7)     // We're using 4KiB pages, so this should always be 0
#define PT_NX ((uint64_t)1 << 63)
#define PT_ADDR_MASK ((uint64_t)0x0000fffffffff000)

#define PT_IS_TABLE(x) (((x) & (PT_VALID | PT_LARGE)) == PT_VALID)
#define PT_IS_LARGE(x) (((x) & (PT_VALID | PT_FLAG_LARGE)) == (PT_VALID | PT_LARGE))
#define PT_TO_VMM_FLAGS(x) ((x) & (PT_FLAG_WRITE | PT_FLAG_NX))

#define pte_new(addr, flags)    ((uint64_t)(addr) | (flags))
#define pte_addr(pte) ((pte) & PT_ADDR_MASK)

#define PAGE_SIZE ((uint64_t)0x1000)

#define PML4_ID(virt) (((virt) >> 39) & 0x1FF)
#define PDPT_ID(virt) (((virt) >> 30) & 0x1FF)
#define PD_ID(virt) (((virt) >> 21) & 0x1FF)
#define PT_ID(virt) (((virt) >> 12) & 0x1FF)

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

pagemap_t new_pagemap()
{
    pagemap_t pagemap;
    pagemap.levels = 4;
    uefi_allocate_pages(1, &pagemap.top_level);
    memset((uint64_t *)pagemap.top_level, 0x0, 0x1000);

    return pagemap;
}

uint64_t *get_lower_level(uint64_t *current_level, uint64_t entry, uint64_t level)
{
    uint64_t *next_level;
    if(PT_IS_TABLE(current_level[entry]))
    {
        next_level = (uint64_t *)pte_addr(current_level[entry]);
    }
    else
    {
        bdebug(INFO, "Entry not found, creating new entry...\r\n");
        uint64_t next_lvl_addr;
        uefi_allocate_pages(1, &next_lvl_addr);
        memset((uint64_t *)next_lvl_addr, 0x0, 0x1000);
        next_level = (uint64_t *)next_lvl_addr;
        bdebug(INFO, "Level %d allocated at 0x%x\r\n", level, next_level);
        current_level[entry] = pte_new((size_t)next_level, PT_TABLE_FLAGS);
        if(PT_IS_TABLE(current_level[entry])) bdebug(INFO, "Entry created!\r\n");
        else bdebug(ERROR, "Sad face =(\r\n");
    }
    return (uint64_t *)next_level;
}

void map_page(pagemap_t pagemap, uint64_t virt_address, uint64_t phys_address, uint64_t flags)
{
    if (virt_address % 0x1000 != 0 || phys_address % 0x1000 != 0)
    {
        bdebug(ERROR, "Physical Address or Virtual Address not aligned to 4KB!\r\n");
        for(;;);
    }
    uint64_t pml4_idx = PML4_ID((uint64_t)virt_address);
    uint64_t pml3_idx = PDPT_ID((uint64_t)virt_address);
    uint64_t pml2_idx = PD_ID((uint64_t)virt_address);
    uint64_t pml1_idx = PT_ID((uint64_t)virt_address);
    
    uint64_t *pml4, *pml3, *pml2, *pml1;
    pml4 = (uint64_t *)pagemap.top_level;

    /*bdebug(INFO, "PML4 address 0x%x\r\n", pagemap.top_level);

    bdebug(INFO, "PML4 Index of Virtual Address 0x%x is: %d\r\n", virt_address, pml4_idx);
    bdebug(INFO, "PDPT Index of Virtual Address 0x%x is: %d\r\n", virt_address, pml3_idx);
    bdebug(INFO, "PD Index of Virtual Address 0x%x is: %d\r\n", virt_address, pml2_idx);
    bdebug(INFO, "PT Index of Virtual Address 0x%x is: %d\r\n", virt_address, pml1_idx);

    bdebug(INFO, "Mapping Physical Address 0x%x to Virtual Address 0x%x\r\n", phys_address, virt_address);*/

    pml3 = get_lower_level(pml4, pml4_idx, 3);
    pml2 = get_lower_level(pml3, pml3_idx, 2);
    pml1 = get_lower_level(pml2, pml2_idx, 1);

    if (flags & ((uint64_t)1 << 12)) {
        flags &= ~((uint64_t)1 << 12);
        flags |= ((uint64_t)1 << 7);
    }

    pml1[pml1_idx] = (uint64_t)(phys_address | flags);

    if(!PT_IS_TABLE(pml1[pml1_idx])) {bdebug(INFO, "Not mapping a 4KB page!\r\n");}
}

void map_pages(pagemap_t pagemap, uint64_t virt_address, uint64_t phys_address, uint64_t flags, uint64_t count) {
    if (virt_address % 0x1000 != 0 || phys_address % 0x1000 != 0 || count % 0x1000 != 0) {
        bdebug(ERROR, "Uh oh!\r\n");
    }

    for (uint64_t i = 0; i < count; ) {
        map_page(pagemap, virt_address + i, phys_address + i, flags);
        i += 0x1000;
    }
}

uint64_t virt_to_phys(pagemap_t pagemap, uint64_t virt_address)
{
    uint64_t pml4_idx = PML4_ID((uint64_t)virt_address);
    uint64_t pml3_idx = PDPT_ID((uint64_t)virt_address);
    uint64_t pml2_idx = PD_ID((uint64_t)virt_address);
    uint64_t pml1_idx = PT_ID((uint64_t)virt_address);

    bdebug(INFO, "PML4 Index of Virtual Address 0x%x is: %d\r\n", virt_address, pml4_idx);
    bdebug(INFO, "PDPT Index of Virtual Address 0x%x is: %d\r\n", virt_address, pml3_idx);
    bdebug(INFO, "PD Index of Virtual Address 0x%x is: %d\r\n", virt_address, pml2_idx);
    bdebug(INFO, "PT Index of Virtual Address 0x%x is: %d\r\n", virt_address, pml1_idx);
    
    uint64_t *pml4, *pml3, *pml2, *pml1;
    pml4 = (uint64_t *)pagemap.top_level;

    bdebug(INFO, "PML4 address 0x%x\r\n", pagemap.top_level);

    pml3 = get_lower_level(pml4, pml4_idx, 3);

    pml2 = get_lower_level(pml3, pml3_idx, 2);

    pml1 = get_lower_level(pml2, pml2_idx, 1);

    bdebug(INFO, "Virtual Address 0x%x is mapped to Physical Address 0x%x\r\n", virt_address, (pte_addr(pml1[pml1_idx])));

    return (pte_addr(pml1[pml1_idx]));
}