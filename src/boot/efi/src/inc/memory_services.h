#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct
{

} aquaboot_mem_info;

void *memset(void *s, int c, size_t n);

void *memcpy(void *dest, const void *src, size_t n);

int memcmp(const void *s1, const void *s2, size_t n);

EFI_MEMORY_DESCRIPTOR* get_memory_map(uint64_t mapk);

// Allocates size amount of bytes and returns the memory allocated in buffer
void uefi_allocate_pool(UINTN size, void **buffer);
void uefi_allocate_pages(UINTN pages, uint64_t *memory);