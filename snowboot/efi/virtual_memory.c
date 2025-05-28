#include <efi.h>
#include <stddef.h>
#include <stdint.h>
#include "efi/efidef.h"
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

#define PTE_NEW(addr, flags)    ((uint64_t)(addr) | (flags))
#define PTE_ADDR(pte) ((pte) & PT_ADDR_MASK)

#define PAGE_SIZE ((uint64_t)0x1000)

#define PML4_ID(virt) (((virt) >> 39) & 0x1FF)
#define PDPT_ID(virt) (((virt) >> 30) & 0x1FF)
#define PD_ID(virt) (((virt) >> 21) & 0x1FF)
#define PT_ID(virt) (((virt) >> 12) & 0x1FF)

pagemap_t newPagemap()
{
    pagemap_t pagemap;
    pagemap.levels = 4;
    uefiAllocatePages(1, &pagemap.topLevel, EfiReservedMemoryType);
    memset((uint64_t *)pagemap.topLevel, 0x0, 0x1000);

    return pagemap;
}

uint64_t *getLowerLevel(uint64_t *currentLevel, uint64_t entry)
{
    uint64_t *nextLevel;
    if(PT_IS_TABLE(currentLevel[entry]))
    {
        nextLevel = (uint64_t *)PTE_ADDR(currentLevel[entry]);
    }
    else
    {
        uint64_t nextLvlAddr;
        uefiAllocatePages(1, &nextLvlAddr, EfiReservedMemoryType);
        memset((uint64_t *)nextLvlAddr, 0x0, 0x1000);
        nextLevel = (uint64_t *)nextLvlAddr;
        currentLevel[entry] = PTE_NEW((size_t)nextLevel, PT_TABLE_FLAGS);
        if(!PT_IS_TABLE(currentLevel[entry])) bdebug(INFO, "Failed to create new page entry!\r\n");
    }
    return (uint64_t *)nextLevel;
}

void mapPage(pagemap_t pagemap, uint64_t virtAddress, uint64_t physAddress, uint64_t flags)
{
    if (virtAddress % 0x1000 != 0 || physAddress % 0x1000 != 0)
    {
        bdebug(ERROR, "Physical Address or Virtual Address not aligned to 4KB!\r\n");
        for(;;);
    }
    uint64_t pml4Idx = PML4_ID((uint64_t)virtAddress);
    uint64_t pml3Idx = PDPT_ID((uint64_t)virtAddress);
    uint64_t pml2Idx = PD_ID((uint64_t)virtAddress);
    uint64_t pml1Idx = PT_ID((uint64_t)virtAddress);
    
    uint64_t *pml4, *pml3, *pml2, *pml1;
    pml4 = (uint64_t *)pagemap.topLevel;

    pml3 = getLowerLevel(pml4, pml4Idx);
    pml2 = getLowerLevel(pml3, pml3Idx);
    pml1 = getLowerLevel(pml2, pml2Idx);

    if (flags & ((uint64_t)1 << 12)) {
        flags &= ~((uint64_t)1 << 12);
        flags |= ((uint64_t)1 << 7);
    }

    pml1[pml1Idx] = (uint64_t)(physAddress | flags);

    if(!PT_IS_TABLE(pml1[pml1Idx])) {bdebug(INFO, "Not mapping a 4KB page!\r\n");}
}

void mapPages(pagemap_t pagemap, uint64_t virtAddress, uint64_t phys_address, uint64_t flags, uint64_t count) {
    if (virtAddress % 0x1000 != 0 || phys_address % 0x1000 != 0 || count % 0x1000 != 0) {
        bdebug(ERROR, "Uh oh!\r\n");
    }

    for (uint64_t i = 0; i < count; ) {
        mapPage(pagemap, virtAddress + i, phys_address + i, flags);
        i += 0x1000;
    }
}

uint64_t virtToPhys(pagemap_t pagemap, uint64_t virtAddress)
{
    uint64_t pml4Idx = PML4_ID((uint64_t)virtAddress);
    uint64_t pml3Idx = PDPT_ID((uint64_t)virtAddress);
    uint64_t pml2Idx = PD_ID((uint64_t)virtAddress);
    uint64_t pml1Idx = PT_ID((uint64_t)virtAddress);

    bdebug(INFO, "PML4 Index of Virtual Address 0x%x is: %d\r\n", virtAddress, pml4Idx);
    bdebug(INFO, "PDPT Index of Virtual Address 0x%x is: %d\r\n", virtAddress, pml3Idx);
    bdebug(INFO, "PD Index of Virtual Address 0x%x is: %d\r\n", virtAddress, pml2Idx);
    bdebug(INFO, "PT Index of Virtual Address 0x%x is: %d\r\n", virtAddress, pml1Idx);
    
    uint64_t *pml4, *pml3, *pml2, *pml1;
    pml4 = (uint64_t *)pagemap.topLevel;

    bdebug(INFO, "PML4 address 0x%x\r\n", pagemap.topLevel);

    pml3 = getLowerLevel(pml4, pml4Idx);

    pml2 = getLowerLevel(pml3, pml3Idx);

    pml1 = getLowerLevel(pml2, pml2Idx);

    bdebug(INFO, "Virtual Address 0x%x is mapped to Physical Address 0x%x\r\n", virtAddress, (PTE_ADDR(pml1[pml1Idx])));

    return (PTE_ADDR(pml1[pml1Idx]));
}