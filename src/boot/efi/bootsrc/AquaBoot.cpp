#include "bootinc/AquaBoot.h"

extern "C" EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS status;

    // Clear the screen
    status = SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Could not clear the screen, there may be system text above\r\n");
    }

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"AquaOS Bootloader has loaded successfully! Copyright (c) BlueSillyDragon 2023-2024\r\n");
    printf_("AquaOS Bootloader %u.%u\r\n",
    BootloaderMajorVersion, 
    BootloaderMinorVersion);

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Setting Watchdog timer...\r\n");

    // Disable the watchdog timer, so the bootloader doesn't time out
    status = SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ERROR TRYING TO SET WATCHDOG TIMER\r\n");
        printf_("ERROR TRYING TO SET WATCHDOG TIMER\r\n");
    }

    else {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Watchdog timer set!\r\n");
    }

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Fetching current time...\r\n");

    EFI_TIME time;
    status = SystemTable->RuntimeServices->GetTime(&time, NULL);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Could not obtain date/time, Boot time will be unavailable\r\n");
    }

    else {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"time was obtained succesfully!\r\n");
        printf_("Year:%u Month:%u Day:%u Hour:%u Minute:%u Second:%u Nanosecond:%u\r\n",
        time.Year ,
        time.Month, 
        time.Day,
        time.Hour,
        time.Minute,
        time.Second,
        time.Nanosecond);
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"[NOTE]: time may be inaccurate.\r\n");
    }
    

    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Attempting to locate GOP...\r\n");
    status = SystemTable->BootServices->LocateProtocol(&gopGuid, NULL, (void**)&gop);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"COULD NOT LOCATE GOP!!!\r\n");
    }

    else {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"GOP Located successfully!\r\n");
    }
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Obtaining current video mode...\r\n");
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN SizeOfInfo, numModes, nativeMode;
    status = gop->QueryMode(gop, gop->Mode==NULL?0:gop->Mode->Mode, &SizeOfInfo, &info);
    if (status == EFI_NOT_STARTED) {
        status = gop->SetMode(gop, 0);
    }
    
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to get native mode!!!\r\n");
    }

    else {
        nativeMode = gop->Mode->Mode;
        numModes = gop->Mode->MaxMode;
        printf_("numberOfModes: %u, nativeMode: %u\r\n", numModes, nativeMode);
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Video mode obtained!\r\n");
    }

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Retrieving avaliable video modes...\r\n");

    for (UINTN i = 0; i < numModes; i++) {
        gop->QueryMode(gop, i, &SizeOfInfo, &info);
        printf_("mode %03d width %d height %d format %x%s\r\n",
        i,
        info->HorizontalResolution,
        info->VerticalResolution,
        info->PixelFormat,
        i == nativeMode ? "(current)" : ""
        );
    }

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Video modes retrieved!\r\n");

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Setting video mode...\r\n");

    status = gop->SetMode(gop, 21);
    if(EFI_ERROR(status)) {
        printf_("Unable to set mode %03d\r\n", 21);
    } 
    
    else {
    // get framebuffer
        printf_("Framebuffer address %x size %d, width %d height %d pixelsperline %d\r\n",
        gop->Mode->FrameBufferBase,
        gop->Mode->FrameBufferSize,
        gop->Mode->Info->HorizontalResolution,
        gop->Mode->Info->VerticalResolution,
        gop->Mode->Info->PixelsPerScanLine
    );
  }

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *pixel;

    pixel->Red = (UINT8)43;
    pixel->Green = (UINT8)96;
    pixel->Blue = (UINT8)222;

    printf_("pixel values: r:%x g:%x b:%x\r\n", pixel->Red, pixel->Green, pixel->Blue);
    
    UINTN sourceX = (UINTN)0;
    UINTN sourceY = (UINTN)0;
    UINTN destX = (UINTN)0;
    UINTN destY = (UINTN)0;
    UINTN width = (UINTN)1680;
    UINTN height = (UINTN)1050;

    status = gop->Blt(gop, pixel, EfiBltVideoFill, sourceX, sourceY, destX, destY, width, height, NULL);

    if (EFI_ERROR(status)) {
        printf_("ERROR TRYING TO BLOCK TRANSFER TO SCREEN!!!\r\n");
    }

    uint32_t pixie;
    int x;
    int y;

    printf_("pixie stores: %x\r\n", pixie);

    displayLogo(x, y, 540, 315, pixie, gop);

    //Get a memory map
    
    printf_("Obtaining the memorymap...\r\n");

    UINTN memoryMapSize;
    EFI_MEMORY_DESCRIPTOR *memoryMap;
    UINTN mapKey;
    UINTN descriptorSize;
    UINT32 descriptorVersion;

    //Get the size of the memory map
    memoryMapSize = 0;
    memoryMap = NULL;

    status = SystemTable->BootServices->GetMemoryMap(
    &memoryMapSize, 
    memoryMap, 
    &mapKey, 
    &descriptorSize, 
    &descriptorVersion);

    if (status == EFI_BUFFER_TOO_SMALL) {
        printf_("Needed memoryMapSize:%ld\r\n", memoryMapSize);
    }

    else {
        printf_("ERROR TRYING TO RETRIEVE MEMORY MAP SIZE!!! BOOTLOADER CANNOT CONTINUE\r\n");

            pixel->Red = (UINT8)255;
            pixel->Green = (UINT8)0;
            pixel->Blue = (UINT8)43;

            screenOfDeath(pixel, status, gop);

        for (;;);
    }

    status = SystemTable->BootServices->AllocatePool(EfiLoaderData, (memoryMapSize + (2 * descriptorSize)), (void**)&memoryMap);

    if (EFI_ERROR(status)) {
        printf_("ERROR TRYING TO ALLOCATE MEMORY FOR MEMORY MAP!!! BOOTLOADER CAN NOT PROCEED\r\n");

            pixel->Red = (UINT8)10;
            pixel->Green = (UINT8)232;
            pixel->Blue = (UINT8)192;

            screenOfDeath(pixel, status, gop);

        for (;;);
    }

    printf_("memoryMap:%x\r\n", &memoryMap);

    memoryMapSize += (2 * descriptorSize);

    status = SystemTable->BootServices->GetMemoryMap(
    &memoryMapSize,
    memoryMap, 
    &mapKey, 
    &descriptorSize, 
    &descriptorVersion);

    if (EFI_ERROR(status)) {
        printf_("ERROR WHILE TRYING TO OBTAIN MEMORY MAP, BOOTLOADER CAN NOT PROCEED!!!\r\n");
        
            pixel->Red = (UINT8)200;
            pixel->Green = (UINT8)132;
            pixel->Blue = (UINT8)43;

            screenOfDeath(pixel, status, gop);

        SystemTable->BootServices->FreePool(memoryMap);

        for(;;);
    }

    printf_("memoryMapSize:%10lu descriptorSize:%10lu descriptorVersion:%10u\r\n",
    memoryMapSize,
    descriptorSize,
    descriptorVersion);

    //Get RSDP
    UINT8 rsdpFound = 0;
    UINTN rsdpIndex = 0;

    AcpiTable.Data1 = (UINT32)0xeb9d2d30;
    AcpiTable.Data2 = (UINT16)0x2d88;
    AcpiTable.Data3 = (UINT16)0x11d3;
    AcpiTable.Data4[0] = (UINT8)0x9a;
    AcpiTable.Data4[1] = (UINT8)0x16;
    AcpiTable.Data4[2] = (UINT8)0x00;
    AcpiTable.Data4[3] = (UINT8)0x90;
    AcpiTable.Data4[4] = (UINT8)0x27;
    AcpiTable.Data4[5] = (UINT8)0x3f;
    AcpiTable.Data4[6] = (UINT8)0xc1;
    AcpiTable.Data4[7] = (UINT8)0x4d;

    Acpi20Table.Data1 = (UINT32)0x8868e871;
    Acpi20Table.Data2 = (UINT16)0xe4f1;
    Acpi20Table.Data3 = (UINT16)0x11d3;
    Acpi20Table.Data4[0] = (UINT8)0xbc;
    Acpi20Table.Data4[1] = (UINT8)0x22;
    Acpi20Table.Data4[2] = (UINT8)0x00;
    Acpi20Table.Data4[3] = (UINT8)0x80;
    Acpi20Table.Data4[4] = (UINT8)0xc7;
    Acpi20Table.Data4[5] = (UINT8)0x3c;
    Acpi20Table.Data4[6] = (UINT8)0x88;
    Acpi20Table.Data4[7] = (UINT8)0x81;

    printf_("Number of entires in the Configuration Table:%lu\r\n", SystemTable->NumberOfTableEntries);

    // Swap out Compare function, and manually compare the ConfigTable with the AcpiTable

    for (UINTN i = 0; i < SystemTable->NumberOfTableEntries; i++) {
        if (compareRSDP(&SystemTable->ConfigurationTable[i].VendorGuid, &Acpi20Table)) {
            printf_("RSDP 2.0 found!\r\n");
            rsdpFound = 2;
            rsdpIndex = i;
        }
        printf_("X\r\n");
    }

    //No RSDP 2.0, search for RSDP 1.0

    if (!rsdpFound) {
        for (UINTN i = 0; i < SystemTable->NumberOfTableEntries; i++) {
            if (compareRSDP(&SystemTable->ConfigurationTable[i].VendorGuid, &Acpi20Table)) {
                printf_("RSDP 1.0 found!\r\n");
                rsdpFound = 1;
                rsdpIndex = i;
            }
        }
    }

    if (!rsdpFound) {
        printf_("RSDP could not be found! System does not support it, Bootloader cannot proceed!");
            pixel->Red = (UINT8)255;
            pixel->Green = (UINT8)0;
            pixel->Blue = (UINT8)100;

            screenOfDeath(pixel, status, gop);
        for (;;);
    }

    uint64_t pml4[512];

    uint64_t pdpt[512];

    uintptr_t pmlAddr = (uintptr_t)&pml4;

    uint64_t rbx_boot;

    printf_("pmlAddr holds: 0x%x\r\n", pmlAddr);

    asm volatile("push %%rbx; mov %0, %%rbx; pop %%rbx;" :: "r"(pmlAddr) : "memory");

    asm volatile("mov %%rbx, %0;" :: "r"(rbx_boot));

    printf_("The register 'rbx' stores: 0x%x\r\n", rbx_boot);

    // PML4 bits
    

    // Set bits of the PML4
    printf_("1st PML4: 0x%lx\r\n", pml4[0]);

    // Exit Boot Services
    printf_("mapKey: %lu\r\n", mapKey);

    status = SystemTable->BootServices->ExitBootServices(ImageHandle, mapKey);

    if (EFI_ERROR(status)) {

        pixel->Red = (UINT8)255;
        pixel->Green = (UINT8)255;
        pixel->Blue = (UINT8)255;

        screenOfDeath(pixel, status, gop);
    }

    statusError(status);

    for(;;);

}

void statusError(EFI_STATUS s) {
    if (EFI_ERROR(s)) {
        printf_("An Error Occurred!\r\n");
        printf_("Reason: ");
        switch (s)
        {
        case EFI_OUT_OF_RESOURCES:
            printf_("Out of resources!\r\n");
            break;
        case EFI_INVALID_PARAMETER:
            printf_("Invalid parameter!\r\n");
            break;
        case EFI_NOT_FOUND:
            printf_("Pages could not be found!\r\n");
            break;
        default:
            break;
        }
    }
}

bool compareRSDP(EFI_GUID *vendGuid, EFI_GUID *rsdp) {
    int i;
    bool same = false;
    if (vendGuid->Data1 != rsdp->Data1) {
        return same;
    }

    if (vendGuid->Data2 != rsdp->Data2) {
        return same;
    }

    if (vendGuid->Data3 != rsdp->Data3) {
        return same;
    }

    for (i = 0; i < 8; i++) {
        same = (vendGuid->Data4[i] == rsdp->Data4[i]);

        if (!same) {
            break;
        }
    }

    return same;
}