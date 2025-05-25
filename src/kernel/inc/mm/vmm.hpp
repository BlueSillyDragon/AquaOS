#pragma once

#include <cstdint>

typedef struct 
{
    uint64_t levels;
    uint64_t topLevel;
} pagemap_t;

void initVmm(std::uint64_t hhdm, std::uint64_t kernelPaddr);
void mapPage(uint64_t virtualAddr, uint64_t physicalAddr, uint8_t flags);
void mapPages(uint64_t virtualStart, uint64_t physicalStart, uint8_t flags, uint64_t count);
void unmapPage(uint64_t virtualAddr);
void remapPage(uint64_t virtualAddr);