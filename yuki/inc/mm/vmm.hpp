#pragma once

#include <cstdint>
#include <limine.h>

typedef struct 
{
    uint64_t levels;
    uint64_t topLevel;
} pagemap_t;

void initVmm(limine_memmap_response *memoryMap, std::uint64_t hhdm);
void mapPage(uint64_t virtualAddr, uint64_t physicalAddr, uint8_t flags);
void mapPages(uint64_t virtualStart, uint64_t physicalStart, uint8_t flags, uint64_t count);
void unmapPage(uint64_t virtualAddr);
void remapPage(uint64_t virtualAddr);