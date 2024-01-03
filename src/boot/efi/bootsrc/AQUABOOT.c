#include <efi.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    UINTN MapKey;
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"AquaOS Bootloader has loaded successfully!\r\n");

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

    for (int i = 0; i < numModes; i++) {
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
        for (;;);
    }

    Status = SystemTable->BootServices->AllocatePool(EfiLoaderData, (MemoryMapSize + (2 * DescriptorSize)), (void**)&MemoryMap);

    if (EFI_ERROR(Status)) {
        printf_("ERROR TRYING TO ALLOCATE MEMORY FOR MEMORY MAP!!! BOOTLOADER CAN NOT PROCEED\r\n");
        for (;;);
    }

    printf_("MemoryMap:%x\r\n", &MemoryMap);

    Status = SystemTable->BootServices->GetMemoryMap(
    &MemoryMapSize,
    MemoryMap, 
    &MapKey, 
    &DescriptorSize, 
    &DescriptorVersion);

    if (EFI_ERROR(Status)) {
        printf_("ERROR WHILE TRYING TO OBTAIN MEMORY MAP, BOOTLOADER CAN NOT PROCEED!!!\r\n");
        pixie = 0x11fe92;
        x=0;
        y-0;
        for (int i = 0; i < 100; i++) {
            PlotPixels(x, y, pixie, gop);
            y++;
        }
        for(;;);
    }

    for(;;);

}
