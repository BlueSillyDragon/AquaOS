//
// AquaBoot Video Services
//

#include "inc/video_services.h"

EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

AQUABOOT_FRAMEBUFFER initGop(EFI_SYSTEM_TABLE* ST) {
    EFI_STATUS status;
    status = ST->BootServices->LocateProtocol(&gopGuid, NULL, (void**)&gop);

    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut,
        L"Could not locate GOP, you may be using a pre-UEFI machine, try using a BIOS build of AquaBoot instead.");
        asm volatile ( "hlt" );
    }

    UINTN sizeOfInfo, numModes, nativeMode;

    status = gop->QueryMode(gop, gop->Mode==NULL?0:gop->Mode->Mode, &sizeOfInfo, &info);

    if (status == EFI_NOT_STARTED) {
        gop->SetMode(gop, 0);
    }

    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"Unable to retrieve native mode!\n\r");
        asm volatile ( "hlt" );
    }

    else {
        nativeMode = gop->Mode->Mode;
        numModes = gop->Mode->MaxMode;
        ST->ConOut->OutputString(ST->ConOut, L"Video mode retrieved!\n\r");
    }

    // Set video mode and get frame buffer

    status = gop->SetMode(gop, 10);

    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"Could not set video mode!");
        asm volatile ( "hlt" );
    }

    AQUABOOT_FRAMEBUFFER fb;

    fb.base = gop->Mode->FrameBufferBase;
    fb.size = gop->Mode->FrameBufferSize;
    fb.horizontalRes = gop->Mode->Info->HorizontalResolution;
    fb.verticalRes = gop->Mode->Info->VerticalResolution;
    fb.pixelsPerScanline = gop->Mode->Info->PixelsPerScanLine;
    fb.pitch = (fb.pixelsPerScanline * 4);

    return fb;

}

void plotPixels (int x, int y, uint32_t pixel, AQUABOOT_FRAMEBUFFER fb) {
    volatile uint32_t *fb_ptr = (uint32_t *)fb.base;

    fb_ptr[x * (fb.pitch / 4) + y] = pixel;
}

void changeBackgroundColor(AQUABOOT_FRAMEBUFFER fb, uint32_t bgColor) {

    for (int i = 0; i < fb.verticalRes; i++) {
        for (int j = 0; j < fb.horizontalRes; j++) {
            plotPixels(i, j, bgColor, fb);
        }
    }

}