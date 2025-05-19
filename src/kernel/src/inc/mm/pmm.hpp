#pragma once

#include <aquaboot.h>
#include <cstdint>

class Pmm
{
    public:
        Pmm();
        void initPmm(aquaboot_memory_descriptor *memory_map, std::uint64_t entries, std::uint64_t desc_size, uint64_t hhdm);
        uint64_t *palloc();
    private:
        std::uint64_t *head;    // Head of the linked list (first free page)
};