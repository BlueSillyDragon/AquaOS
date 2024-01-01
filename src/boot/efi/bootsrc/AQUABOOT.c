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
    printf_("Hello! %d \r\n", 10);
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Attempting to locate GOP...\r\n");
    Status = (SystemTable->BootServices->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
    if (EFI_ERROR(Status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"COULD NOT LOCATE GOP!!!\r\n");
    }

    else {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"GOP Located successfully!\r\n");
    }
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Obtaining current video mode...\r\n");
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN SizeOfInfo, numModes, nativeMode;
    Status = (gop->QueryMode, 4, gop, gop->Mode==NULL?0:gop->Mode->Mode, &SizeOfInfo, &info);
    if (Status == EFI_NOT_STARTED) {
        Status = (gop->SetMode, 2, gop, 0);
    }
    
    if (EFI_ERROR(Status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to get native mode!!!\r\n");
    }

    else {
        printf_("Working!\r\n");
        nativeMode = gop->Mode->Mode;
        numModes = gop->Mode->MaxMode;
        printf_("numberOfModes: %u, nativeMode: %u\r\n", numModes, nativeMode);
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Video mode obtained!\r\n");
    }

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Retrieving avaliable video modes...\r\n");

    for (UINTN i = 0; i < numModes; i++) {
        gop->QueryMode(&gop, i, &SizeOfInfo, &info);
        printf_("mode %03d width %d height %d format %x%s\r\n",
        i,
        info->HorizontalResolution,
        info->VerticalResolution,
        info->PixelFormat,
        i == nativeMode ? "(current)" : ""
        );
    }

    Status = (gop->SetMode, 2, gop, 0);
    if(EFI_ERROR(Status)) {
        printf_("Unable to set mode %03d\r\n", 3);
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

    pixel->Red = (UINT8)4;
    pixel->Green = (UINT8)25;
    pixel->Blue = (UINT8)184;

    printf_("pixel values: r:%x g:%x b:%x\r\n", pixel->Red, pixel->Green, pixel->Blue);

    for (;;);
}
