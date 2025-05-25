#pragma once

#include <snowboot.h>
#include <cstdint>

void initPmm(snowboot_memory_descriptor *memory_map, std::uint64_t entries, std::uint64_t desc_size, uint64_t hhdm);
uint64_t pmmAlloc();
void pmmFree(uint64_t *page);