#include <cstdint>
#include <inc/print.hpp>
#include <inc/mem.hpp>
#include <inc/mm/pmm.hpp>
#include <inc/mm/vmm.hpp>
#include <stdint.h>

#define PML4_ID(virt) (((virt) >> 39) & 0x1FF)
#define PDPT_ID(virt) (((virt) >> 30) & 0x1FF)
#define PD_ID(virt) (((virt) >> 21) & 0x1FF)
#define PT_ID(virt) (((virt) >> 12) & 0x1FF)

extern "C" Terminal kern_terminal;

pagemap_t pagemap;

uint64_t hhdm_offs;

uint64_t kernel_virt = 0xffffffff80000000;

uint64_t *get_lower_level(uint64_t *current_level, uint64_t entry)
{
    uint64_t *next_level;
    if (current_level[entry] & (1 << 0))    // Perform AND on the first bit to see if the entry is present
    {
        next_level = reinterpret_cast<std::uint64_t *>(current_level[entry] & 0x0000fffffffff000);
        next_level = reinterpret_cast<std::uint64_t *>((std::uint64_t)next_level + hhdm_offs);
    }
    else
    {
        // Create a new entry
        next_level = pmm_alloc();
        next_level = reinterpret_cast<std::uint64_t *>((std::uint64_t)next_level + hhdm_offs);
        memset(next_level, 0, 0x1000);
        current_level[entry] = ((reinterpret_cast<uint64_t>(next_level) - hhdm_offs) | (1 << 0));
    }
    return next_level;
}

uint64_t virt_to_phys(uint64_t virt_address)
{
    uint64_t pml4_idx = PML4_ID((uint64_t)virt_address);
    uint64_t pml3_idx = PDPT_ID((uint64_t)virt_address);
    uint64_t pml2_idx = PD_ID((uint64_t)virt_address);
    uint64_t pml1_idx = PT_ID((uint64_t)virt_address);
    
    uint64_t *pml4, *pml3, *pml2, *pml1;
    pml4 = (uint64_t *)pagemap.top_level;

    pml3 = get_lower_level(pml4, pml4_idx);

    pml2 = get_lower_level(pml3, pml3_idx);

    pml1 = get_lower_level(pml2, pml2_idx);

    return (pml1[pml1_idx] & 0x0000fffffffff000);
}

void init_vmm(std::uint64_t hhdm, std::uint64_t kernel_paddr)
{
    kern_terminal.kinfo(VMM, "Initializing VMM...\n");
    hhdm_offs = hhdm;
    pagemap.levels = 4;
    pagemap.top_level = pmm_alloc();
    pagemap.top_level = reinterpret_cast<std::uint64_t *>((std::uint64_t)pagemap.top_level + hhdm_offs);
    map_pages(hhdm, 0x0, 0x3, 0x100000000);  // Make sure to keep the HHDM mapped
    kern_terminal.kinfo(VMM, "Finished HHDM map! Remapping kernel...\n");
    map_pages(kernel_virt, kernel_paddr, 0x3, 0x10000);
    kern_terminal.kinfo(VMM, "Finished remapping kernel! ");
    kern_terminal.term_print("Kernel physical start 0x%x mapped to virtual start 0x%x\n", virt_to_phys(kernel_virt), kernel_virt);
}

void map_page(uint64_t virtual_addr, uint64_t physical_addr, uint8_t flags)
{
    if (virtual_addr % 0x1000 != 0 || physical_addr % 0x1000 != 0)
    {
        kern_terminal.kerror("Attempt was made to map a virtual address or physical address not aligned to a 4KB boundary\n");
        kern_terminal.term_print("Virtual Address 0x%x | Physical Address 0x%x\nHalting system...", virtual_addr, physical_addr);
        __asm__ volatile (" hlt ");
    }

    uint64_t pml4_idx = PML4_ID(virtual_addr);
    uint64_t pml3_idx = PDPT_ID(virtual_addr);
    uint64_t pml2_idx = PD_ID(virtual_addr);
    uint64_t pml1_idx = PT_ID(virtual_addr);

    uint64_t *pml4, *pml3, *pml2, *pml1;

    pml4 = pagemap.top_level;

    pml3 = get_lower_level(pml4, pml4_idx);
    pml2 = get_lower_level(pml3, pml3_idx);
    pml1 = get_lower_level(pml2, pml2_idx);

    if (flags & ((uint64_t)1 << 12)) {
        flags &= ~((uint64_t)1 << 12);
        flags |= ((uint64_t)1 << 7);
    }

    pml1[pml1_idx] = (uint64_t)(physical_addr | flags);
}

void map_pages(uint64_t virtual_start, uint64_t physical_start, uint8_t flags, uint64_t count)
{
    if (virtual_start % 0x1000 != 0 || physical_start % 0x1000 != 0 || count % 0x1000 != 0) {
        kern_terminal.kerror("Virtual Start, Physical Start, or Count not aligned to 4KB!\n");
        kern_terminal.term_print("Virtual Address 0x%x | Physical Address 0x%x | Count 0x%x\nHalting system...", virtual_start, physical_start, count);
    }

    for (uint64_t i = 0; i < count; ) {
        map_page(virtual_start + i, physical_start + i, flags);
        i += 0x1000;
    }
}