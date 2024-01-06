#include "bootinc/AquaBoot.h"

extern "C" EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    (void)ImageHandle;

    EFI_STATUS Status;
    UINTN MapKey;
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"AquaOS Bootloader has loaded successfully! Copyright (c) BlueSillyDragon 2023-2024\r\n");
    printf_("AquaOS Bootloader %u.%u\r\n",
    BootloaderMajorVersion, 
    BootloaderMinorVersion);

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Setting Watchdog timer...\r\n");

    // Disable the watchdog timer, so the bootloader doesn't time out
    Status = SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

    if (EFI_ERROR(Status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ERROR TRYING TO SET WATCHDOG TIMER\r\n");
        printf_("ERROR TRYING TO SET WATCHDOG TIMER\r\n");
    }

    else {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Watchdog timer set!\r\n");
    }

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Fetching current time...\r\n");

    EFI_TIME Time;
    Status = SystemTable->RuntimeServices->GetTime(&Time, NULL);
    if (EFI_ERROR(Status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Could not obtain date/time, Boot Time will be unavailable\r\n");
    }

    else {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Time was obtained succesfully!\r\n");
        printf_("Year:%u Month:%u Day:%u Hour:%u Minute:%u Second:%u Nanosecond:%u\r\n",
        Time.Year ,
        Time.Month, 
        Time.Day,
        Time.Hour,
        Time.Minute,
        Time.Second,
        Time.Nanosecond);
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"[NOTE]: Time may be inaccurate.\r\n");
    }
    

    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Attempting to locate GOP...\r\n");
    Status = SystemTable->BootServices->LocateProtocol(&gopGuid, NULL, (void**)&gop);
    if (EFI_ERROR(Status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"COULD NOT LOCATE GOP!!!\r\n");
    }

    else {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"GOP Located successfully!\r\n");
    }
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Obtaining current video mode...\r\n");
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN SizeOfInfo, numModes, nativeMode;
    Status = gop->QueryMode(gop, gop->Mode==NULL?0:gop->Mode->Mode, &SizeOfInfo, &info);
    if (Status == EFI_NOT_STARTED) {
        Status = gop->SetMode(gop, 0);
    }
    
    if (EFI_ERROR(Status)) {
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

    Status = gop->SetMode(gop, 21);
    if(EFI_ERROR(Status)) {
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

    Status = gop->Blt(gop, pixel, EfiBltVideoFill, sourceX, sourceY, destX, destY, width, height, NULL);

    if (EFI_ERROR(Status)) {
        printf_("ERROR TRYING TO BLOCK TRANSFER TO SCREEN!!!\r\n");
    }

    uint32_t pixie;
    int x;
    int y;

    printf_("pixie stores: %x\r\n", pixie);

    displayLogo(x, y, 540, 315, pixie, gop);

    //Get a memory map
    
    printf_("Obtaining the memorymap...\r\n");

    UINTN MemoryMapSize;
    EFI_MEMORY_DESCRIPTOR *MemoryMap;
    UINTN Mapkey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;

    //Get the size of the memory map
    MemoryMapSize = 0;
    MemoryMap = NULL;

    Status = SystemTable->BootServices->GetMemoryMap(
    &MemoryMapSize, 
    MemoryMap, 
    &MapKey, 
    &DescriptorSize, 
    &DescriptorVersion);

    if (Status == EFI_BUFFER_TOO_SMALL) {
        printf_("Needed MemoryMapSize:%d\r\n", MemoryMapSize);
    }

    else {
        printf_("ERROR TRYING TO RETRIEVE MEMORY MAP SIZE!!! BOOTLOADER CANNOT CONTINUE\r\n");
        pixie = 0xffff00;
        x=0;
        y=0;
        for (int i = 0; i < 100; i++) {
            plotPixels(x, y, pixie, gop);
            y++;
        }
        for (;;);
    }

    Status = SystemTable->BootServices->AllocatePool(EfiLoaderData, (MemoryMapSize + (2 * DescriptorSize)), (void**)&MemoryMap);

    if (EFI_ERROR(Status)) {
        printf_("ERROR TRYING TO ALLOCATE MEMORY FOR MEMORY MAP!!! BOOTLOADER CAN NOT PROCEED\r\n");
        pixie = 0xff00fe;
        x=1;
        y=1;
        for (int i = 0; i < 100; i++) {
            plotPixels(x, y, pixie, gop);
            y++;
        }
        for (;;);
    }

    printf_("MemoryMap:%x\r\n", &MemoryMap);

    MemoryMapSize += (2 * DescriptorSize);

    Status = SystemTable->BootServices->GetMemoryMap(
    &MemoryMapSize,
    MemoryMap, 
    &MapKey, 
    &DescriptorSize, 
    &DescriptorVersion);

    if (EFI_ERROR(Status)) {
        printf_("ERROR WHILE TRYING TO OBTAIN MEMORY MAP, BOOTLOADER CAN NOT PROCEED!!!\r\n");
        pixie = 0x11fe92;
        x=2;
        y=2;
        for (int i = 0; i < 100; i++) {
            plotPixels(x, y, pixie, gop);
            y++;
        }

        switch (Status)
        {
        case EFI_SUCCESS:
            printf_("EFI_SUCCESS\r\n");
            break;
        case EFI_BUFFER_TOO_SMALL:
            printf_("EFI_BUFFER_TOO_SMALL\r\n");
            break;
        case EFI_INVALID_PARAMETER:
            printf_("EFI_INVALID_PARAMETER\r\n");
            break;
        
        default:
            break;
        }

        SystemTable->BootServices->FreePool(MemoryMap);

        for(;;);
    }

    printf_("MemoryMapSize:%10u DescriptorSize:%10u DescriptorVersion:%10u\r\n",
    MemoryMapSize,
    DescriptorSize,
    DescriptorVersion);

    //Get RSDP
    UINT8 RSDPFound = 0;
    UINTN RSDPIndex = 0;

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

    printf_("Number of entires in the Configuration Table:%u\r\n", SystemTable->NumberOfTableEntries);

    for (UINTN i = 0; i < SystemTable->NumberOfTableEntries; i++) {
        printf_("Table: %8u GUID: %08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x\r\n", 
        i,
        SystemTable->ConfigurationTable[i].VendorGuid.Data1,
        SystemTable->ConfigurationTable[i].VendorGuid.Data2,
        SystemTable->ConfigurationTable[i].VendorGuid.Data3,
        SystemTable->ConfigurationTable[i].VendorGuid.Data4[0],
        SystemTable->ConfigurationTable[i].VendorGuid.Data4[1],
        SystemTable->ConfigurationTable[i].VendorGuid.Data4[2],
        SystemTable->ConfigurationTable[i].VendorGuid.Data4[3],
        SystemTable->ConfigurationTable[i].VendorGuid.Data4[4],
        SystemTable->ConfigurationTable[i].VendorGuid.Data4[5],
        SystemTable->ConfigurationTable[i].VendorGuid.Data4[6],
        SystemTable->ConfigurationTable[i].VendorGuid.Data4[7]);
    }

    //No RSDP 2.0, search for RSDP 1.0

    // Swap out Compare function, and manually compare the ConfigTable with the AcpiTable

    /*for (UINTN i = 0; i < SystemTable->NumberOfTableEntries; i++) {
        if (compare(&SystemTable->ConfigurationTable[i].VendorGuid, &Acpi20Table, 16)) {
            printf_("RSDP 2.0 found!");
            RSDPFound = 2;
            RSDPIndex = i;
        }
    }

    if (!RSDPFound) {
        for (UINTN i = 0; i < SystemTable->NumberOfTableEntries; i++) {
            if (compare(&SystemTable->ConfigurationTable[i].VendorGuid, &AcpiTable, 16)) {
                printf_("RSDP 1.0 found!");
                RSDPFound = 1;
                RSDPIndex = i;
            }
        }
    }

    if (!RSDPFound) {
        printf_("RSDP could not be found! System does not support it, Bootloader cannot proceed!");
        pixie = 0xff381e;
        x=3;
        y=3;
        for (int i = 0; i < 100; i++) {
            plotPixels(x, y, pixie, gop);
            y++;
        }
        for (;;);
    }*/

    // Setup Virtual Memory so we can load the Kernel

    for(;;);

}