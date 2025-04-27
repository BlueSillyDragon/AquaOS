#pragma once

#include <stdint.h>
EFI_MEMORY_DESCRIPTOR* get_memory_map();

// Allocates size amount of bytes and returns the memory allocated in buffer
void uefi_allocate_pool(UINTN size, void **buffer);
void uefi_allocate_pages(UINTN pages, uint64_t *memory);