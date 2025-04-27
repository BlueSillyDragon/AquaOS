#pragma once

#include <stdint.h>

typedef struct
{

} aquaboot_mem_info;

EFI_MEMORY_DESCRIPTOR* get_memory_map(uint64_t mapk);

// Allocates size amount of bytes and returns the memory allocated in buffer
void uefi_allocate_pool(UINTN size, void **buffer);
void uefi_allocate_pages(UINTN pages, uint64_t *memory);