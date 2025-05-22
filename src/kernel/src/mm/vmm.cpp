#include <cstdint>
#include <inc/print.hpp>
#include <inc/mem.hpp>
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

extern "C" Terminal kern_terminal;

pagemap_t pagemap;

uint64_t hhdm_offs;

uint64_t kernel_virt = 0xffffffff80000000;

uint64_t get_lower_level(uint64_t *current_level, uint64_t entry)
{
    uint64_t next_level;
    if(PT_IS_PRESENT(current_level[entry]))
    {
        next_level = (current_level[entry] & PT_ADDR_MASK);
    }

    else
    {
        next_level = pmm_alloc();
        memset(reinterpret_cast<uint64_t *>(next_level + hhdm_offs), 0x0, 0x1000);
        current_level[entry] = (next_level | (1 << 0));
    }
    return next_level;
}

uint64_t virt_to_phys(uint64_t virtual_addr)
{
    uint64_t pml4_idx = PML4_ID(virtual_addr);
    uint64_t pml3_idx = PDPT_ID(virtual_addr);
    uint64_t pml2_idx = PD_ID(virtual_addr);
    uint64_t pml1_idx = PT_ID(virtual_addr);

    uint64_t *pml4 = reinterpret_cast<uint64_t *>(pagemap.top_level + hhdm_offs);
    uint64_t *pml3 = reinterpret_cast<uint64_t *>(get_lower_level(pml4, pml4_idx) + hhdm_offs);
    uint64_t *pml2 = reinterpret_cast<uint64_t *>(get_lower_level(pml3, pml3_idx) + hhdm_offs);
    uint64_t *pml1 = reinterpret_cast<uint64_t *>(get_lower_level(pml2, pml2_idx) + hhdm_offs);

    return (pml1[pml1_idx] & PT_ADDR_MASK);
}

void init_vmm(std::uint64_t hhdm, std::uint64_t kernel_paddr)
{
    kern_terminal.kinfo(VMM, "Initializing VMM...\n");
    hhdm_offs = hhdm;
    pagemap.top_level = pmm_alloc();
    memset(reinterpret_cast<uint64_t *>(pagemap.top_level + hhdm), 0x0, 0x1000);
    
    map_pages(hhdm, 0x0, 0x3, 0x100000000);
    map_pages(0x00000000b0000000, 0x00000000b0000000, 0x3, 0x10000000);
    map_pages(kernel_virt, kernel_paddr, 0x3, 0x10000);
    map_pages(pagemap.top_level, pagemap.top_level, 0x3, 0x10000000);

    __asm__ volatile ("mov %0, %%rax; mov %%rax, %%cr3" :: "a"(pagemap.top_level));
}

void map_page(uint64_t virtual_addr, uint64_t physical_addr, uint8_t flags)
{
    if (virtual_addr % 0x1000 != 0 || physical_addr % 0x1000 != 0)
    {
        kern_terminal.kerror("Attempted to map a virtual address or physical address that was not aligned to 4KB!\n");
        __asm__ volatile (" hlt ");
    }

    uint64_t pml4_idx = PML4_ID(virtual_addr);
    uint64_t pml3_idx = PDPT_ID(virtual_addr);
    uint64_t pml2_idx = PD_ID(virtual_addr);
    uint64_t pml1_idx = PT_ID(virtual_addr);

    uint64_t *pml4 = reinterpret_cast<uint64_t *>(pagemap.top_level + hhdm_offs);
    uint64_t *pml3 = reinterpret_cast<uint64_t *>(get_lower_level(pml4, pml4_idx) + hhdm_offs);
    uint64_t *pml2 = reinterpret_cast<uint64_t *>(get_lower_level(pml3, pml3_idx) + hhdm_offs);
    uint64_t *pml1 = reinterpret_cast<uint64_t *>(get_lower_level(pml2, pml2_idx) + hhdm_offs);

    pml1[pml1_idx] = (physical_addr | flags);
}

void map_pages(uint64_t virtual_start, uint64_t physical_start, uint8_t flags, uint64_t count)
{
    if (virtual_start % 0x1000 != 0 || physical_start % 0x1000 != 0 || count % 0x1000 != 0)
    {
        kern_terminal.kerror("Attempted to map multiple virtual addresses or physical addresses or count that was not aligned to 4KB!\n");
        __asm__ volatile (" hlt ");
    }

    for (uint64_t i = 0; i < count; ) {
        map_page(virtual_start + i, physical_start + i, flags);
        i += 0x1000;
    }
}