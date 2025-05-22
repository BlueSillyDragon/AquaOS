#pragma once

#include <aquaboot.h>
#include <cstdint>

void init_pmm(aquaboot_memory_descriptor *memory_map, std::uint64_t entries, std::uint64_t desc_size, uint64_t hhdm);
uint64_t pmm_alloc();
void pmm_free(uint64_t *page);