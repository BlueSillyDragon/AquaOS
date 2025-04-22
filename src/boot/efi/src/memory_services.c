#include "inc/globals.h"
#include "inc/log.h"
#include "inc/memory_services.h"

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