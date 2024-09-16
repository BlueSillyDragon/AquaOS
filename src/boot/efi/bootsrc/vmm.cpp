#include "bootinc/vmm.h"

size_t Vmm::addressToIndex(uintptr_t address, uint8_t level) {
    return (address >> (9 * level + 12)) & 0x1ff;
}

void Vmm::mapAddress(uintptr_t physaddr, uintptr_t virtaddr, uint64_t flags) {
    
}