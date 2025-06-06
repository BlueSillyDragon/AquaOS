#pragma once

#include <limine.h>
#include <cstdint>

void initPmm(limine_memmap_response *memoryMap, uint64_t hhdm);
uint64_t pmmAlloc();
void pmmFree(uint64_t page);