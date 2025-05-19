#include <efi.h>
#include <stdint.h>
#include "efi/efidef.h"
#include "efi/efierr.h"
#include "efi/x86_64/efibind.h"
#include "inc/boot_protocol/aquaboot.h"
#include "inc/globals.h"
#include "inc/log.h"
#include "inc/print.h"
#include "inc/memory_services.h"
#include "inc/virtual_memory.h"

uint64_t num_of_entries;

uint64_t mapk;

UINTN descriptor_size = 0;

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memcpy(void *dest, const void *src, size_t n) {
    asm volatile(
        "rep movsb"
        : "=D"(dest), "=S"(src), "=c"(n)
        : "D"(dest), "S"(src), "c"(n)
        : "memory"
    );
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

AQUABOOT_MEM_TYPE uefi_type_to_native(EFI_MEMORY_TYPE type)
{
    switch (type)
    {
        case EfiReservedMemoryType:
        case EfiUnusableMemory:
        case EfiACPIMemoryNVS:
        case EfiMemoryMappedIO:
        case EfiMemoryMappedIOPortSpace:
        case EfiUnacceptedMemoryType:
        case EfiPalCode:
        case EfiPersistentMemory:
        case EfiMaxMemoryType:
            return AQUAOS_RESERVED;
            break;
        case EfiLoaderCode:
        case EfiLoaderData:
        case EfiBootServicesCode:
        case EfiBootServicesData:
        case EfiConventionalMemory:
            return AQUAOS_FREE_MEMORY;
            break;
        case EfiRuntimeServicesCode:
        case EfiRuntimeServicesData:
            return AQUAOS_RUNTIME_SERVICES;
            break;
        case EfiACPIReclaimMemory:
            return AQUAOS_ACPI_RECLAIM;
            break;
    }
}

EFI_MEMORY_DESCRIPTOR* get_memory_map(pagemap_t pagemap)
{
    EFI_STATUS status;

    UINTN memory_map_size = 0;
    EFI_MEMORY_DESCRIPTOR *memory_map = NULL;
    UINTN map_key = 0;
    UINT32 descriptor_version = 0;

    // Get the size of the amount of memory we need to Allocate memory for the MemoryMap
    status = sysT->BootServices->GetMemoryMap(&memory_map_size, memory_map, &map_key, &descriptor_size, &descriptor_version);

    if (status == EFI_BUFFER_TOO_SMALL)     // If status is not EFI_BUFFER_TOO_SMALL something has gone wrong
    {
        bdebug(INFO, "Buffer too small, correct size has been returned in memory_map_size\r\n");
    }

    // Allocate memory for MemoryMap
    void *buffer;
    status = sysT->BootServices->AllocatePool(EfiReservedMemoryType, (memory_map_size + (2 * descriptor_size)), &buffer);
    if (status != EFI_SUCCESS)
    {
        bdebug(ERROR, "Could not allocate memory for MemoryMap!\r\n");
    }

    memory_map = buffer;

    // Ensure that the Memory Map is identity mapped
    map_pages(pagemap, ((uint64_t)memory_map & ~0xfff), ((uint64_t)memory_map & ~0xfff), 0x3, 0x4000);

    // Get the MemoryMap
    status = sysT->BootServices->GetMemoryMap(&memory_map_size, memory_map, &map_key, &descriptor_size, &descriptor_version);
    if (status != EFI_SUCCESS)
    {
        bdebug(ERROR, "Something went wrong when retrieving MemoryMap!\r\n");
        bdebug(INFO, "MemoryMapSize: %d | MemoryMap Address: 0x%x | Desc Size: %d\r\n", memory_map_size, memory_map, descriptor_size);
    }

    else
    {
        bdebug(INFO, "MemoryMap was retrieved!\r\n");
        bdebug(INFO, "MemoryMapSize: %d | MemoryMap Address: 0x%x | Desc Size: %d\r\n", memory_map_size, memory_map, descriptor_size);
    }

    num_of_entries = memory_map_size / descriptor_size;
    mapk = map_key;

    return memory_map;
}

uint64_t get_entry_count()
{
    return num_of_entries;
}

uint64_t get_map_key()
{
    return mapk;
}

uint64_t get_desc_size()
{
    return descriptor_size;
}

void set_memory_types(EFI_MEMORY_DESCRIPTOR *memory_map)
{
    aquaboot_memory_descriptor *desc;
    for (int i = 0; i < num_of_entries; i++)
    {
        desc = (EFI_MEMORY_DESCRIPTOR *)((uint8_t *)memory_map + (i * get_desc_size()));
        desc->type = uefi_type_to_native(desc->type);
    }
}

void uefi_allocate_pool(UINTN size, void **buffer)
{
    EFI_STATUS status;
    status = sysT->BootServices->AllocatePool(EfiLoaderData, size, buffer);

    if(status != EFI_SUCCESS)
    {
        bdebug(ERROR, "Failed to allocate %d bytes of memory!\r\n", size);
    } else bdebug(INFO, "Allocated %d bytes! Starting address: 0x%x\r\n", size, buffer);
}

void uefi_allocate_pages(UINTN pages, uint64_t *memory, EFI_MEMORY_TYPE mem_type)
{
    EFI_STATUS status;
    status = sysT->BootServices->AllocatePages(AllocateAnyPages, mem_type, pages, memory);

    if(status != EFI_SUCCESS)
    {
        bdebug(ERROR, "Failed to allocate %d pages!\r\n", pages);
        switch (status)
        {
            case EFI_OUT_OF_RESOURCES:
                bdebug(ERROR, "Out of resources!\r\n");
                break;
            case EFI_INVALID_PARAMETER:
                bdebug(ERROR, "Invalid Parameter!\r\n");
                break;
            case EFI_NOT_FOUND:
                bdebug(ERROR, "Page not found!\r\n");
                break;
        }
    }
}