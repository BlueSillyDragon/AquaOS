#pragma once

#include <cstdint>

typedef struct 
{
    uint64_t levels;
    uint64_t top_level;
} pagemap_t;

void init_vmm(std::uint64_t hhdm, std::uint64_t kernel_paddr);
void map_page(uint64_t virtual_addr, uint64_t physical_addr, uint8_t flags);
void map_pages(uint64_t virtual_start, uint64_t physical_start, uint8_t flags, uint64_t count);
void unmap_page(uint64_t virtual_addr);
void remap_page(uint64_t virtual_addr);