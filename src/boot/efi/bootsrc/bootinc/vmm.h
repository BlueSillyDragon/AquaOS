#pragma once

#include <cstdint>
#include <cstddef>

class Vmm {
    public:
        void setupPaging();
        void mapAddress(uintptr_t physaddr, uintptr_t virtaddr, uint64_t flags);
        void setupPageTables();
        size_t addressToIndex(uintptr_t address, uint8_t level);
    private:
        
};