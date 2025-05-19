#pragma once

#include <stddef.h>
#include <stdint.h>
#include "boot_protocol/aquaboot.h"
#include "efi/efidef.h"
#include "virtual_memory.h"

typedef struct
{

} aquaboot_mem_info;

void *memset(void *s, int c, size_t n);

void *memcpy(void *dest, const void *src, size_t n);

int memcmp(const void *s1, const void *s2, size_t n);

AQUABOOT_MEM_TYPE uefi_type_to_native(EFI_MEMORY_TYPE type);

EFI_MEMORY_DESCRIPTOR* get_memory_map(pagemap_t pagemap);

void set_memory_types(EFI_MEMORY_DESCRIPTOR *memory_map);

// Returns the number of entries in the Memory Map
uint64_t get_entry_count();

uint64_t get_map_key();

uint64_t get_desc_size();

// Allocates size amount of bytes and returns the memory allocated in buffer
void uefi_allocate_pool(UINTN size, void **buffer);
void uefi_allocate_pages(UINTN pages, uint64_t *memory, EFI_MEMORY_TYPE mem_type);