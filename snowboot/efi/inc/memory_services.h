#pragma once

#include <stddef.h>
#include <stdint.h>
#include "boot_protocol/snowboot.h"
#include "virtual_memory.h"

void *memset(void *s, int c, size_t n);

void *memcpy(void *dest, const void *src, size_t n);

int memcmp(const void *s1, const void *s2, size_t n);

SNOWBOOT_MEM_TYPE uefiTypeToNative(EFI_MEMORY_TYPE type);

EFI_MEMORY_DESCRIPTOR* getMemoryMap(pagemap_t pagemap);

void setMemoryTypes(EFI_MEMORY_DESCRIPTOR *memoryMap);

// Returns the number of entries in the Memory Map
uint64_t getEntryCount();

uint64_t getMapKey();

uint64_t getDescSize();

// Allocates size amount of bytes and returns the memory allocated in buffer
void uefiAllocatePool(UINTN size, void **buffer);
void uefiAllocatePages(UINTN pages, uint64_t *memory, EFI_MEMORY_TYPE memType);