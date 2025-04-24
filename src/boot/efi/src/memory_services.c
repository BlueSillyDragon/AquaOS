#include <efi.h>
#include <stdint.h>
#include "efi/efidef.h"
#include "efi/efierr.h"
#include "efi/x86_64/efibind.h"
#include "inc/globals.h"
#include "inc/log.h"
#include "inc/memory_services.h"

EFI_MEMORY_DESCRIPTOR* get_memory_map()
{
    EFI_STATUS status;

    UINTN memory_map_size = 0;
    EFI_MEMORY_DESCRIPTOR *memory_map;
    UINTN *map_key;
    UINTN *descriptor_size;
    UINT32 *descriptor_version;

    // Get the size of the amount of memory we need to Allocate memory for the MemoryMap
    status = sysT->BootServices->GetMemoryMap(&memory_map_size, memory_map, map_key, descriptor_size, descriptor_version);

    if (status == EFI_BUFFER_TOO_SMALL)     // If status is not EFI_BUFFER_TOO_SMALL something has gone wrong
    {
        bdebug(INFO, "Buffer too small, correct size has been returned in memory_map_size\r\n");
    }

    // Allocate memory for MemoryMap
    status = sysT->BootServices->AllocatePool(EfiLoaderData, memory_map_size, &memory_map);
    if (status != EFI_SUCCESS)
    {
        bdebug(ERROR, "Could not allocate memory for MemoryMap!\r\n");
    }

    // Get the MemoryMap
    status = sysT->BootServices->GetMemoryMap(&memory_map_size, memory_map, map_key, descriptor_size, descriptor_version);
    if (status != EFI_SUCCESS)
    {
        bdebug(ERROR, "Something went wrong when retrieving MemoryMap!\r\n");
        bdebug(INFO, "MemoryMapSize: %d | MemoryMap Address: 0x%x\r\n", memory_map_size, memory_map);
    }

    else
    {
        bdebug(INFO, "MemoryMap was retrieved!\r\n");
        bdebug(INFO, "MemoryMapSize: %d | MemoryMap Address: 0x%x\r\n", memory_map_size, memory_map);
    }
    return memory_map;
}

void uefi_allocate_pool(UINTN size, void **buffer)
{
    EFI_STATUS status;
    status = sysT->BootServices->AllocatePool(EfiLoaderData, size, buffer);

    if (status == EFI_SUCCESS)
    {
        bdebug(INFO, "Successfully allocated %d bytes of memory!\r\n", size);
    }

    else
    {
        bdebug(ERROR, "Failed to allocate memory!\r\n");
    }
}