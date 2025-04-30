#include "inc/boot_protocol/aquaboot.h"
#include "inc/globals.h"
#include "inc/print.h"
#include "inc/video_services.h"
#include "inc/logo.h"

EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

aquaboot_framebuffer *fb;

aquaboot_framebuffer *init_video_services() {
    EFI_STATUS sta;
    sta = sysT->BootServices->LocateProtocol(&gopGuid, NULL, (void**)&gop);

    if (EFI_ERROR(sta)) {
        print(u"Could not locate GOP, you may be using a pre-UEFI machine, try using a BIOS build of AquaBoot instead.\r\n");
        asm volatile ( "hlt" );
    }

    UINTN sizeOfInfo, numModes, nativeMode;

    sta = gop->QueryMode(gop, gop->Mode == NULL ? 0:gop->Mode->Mode, &sizeOfInfo, &info);

    if (sta == EFI_NOT_STARTED) {
        gop->SetMode(gop, 0);
    }

    if (EFI_ERROR(sta)) {
        print(u"Unable to retrieve native mode!\n\r");
        asm volatile ( "hlt" );
    }

    else {
        nativeMode = gop->Mode->Mode;
        numModes = gop->Mode->MaxMode;
        print(u"Video mode retrieved!\n\r");
    }

    // Set video mode and get frame buffer

    sta = gop->SetMode(gop, 10);

    if (EFI_ERROR(sta)) {
        print(u"Could not set video mode!");
        asm volatile ( "hlt" );
    }

    fb->base = gop->Mode->FrameBufferBase;
    fb->size = gop->Mode->FrameBufferSize;
    fb->horizontalRes = gop->Mode->Info->HorizontalResolution;
    fb->verticalRes = gop->Mode->Info->VerticalResolution;
    fb->pixelsPerScanline = gop->Mode->Info->PixelsPerScanLine;
    fb->pitch = (fb->pixelsPerScanline * 4);

    return fb;
}

void plotPixels (aquaboot_framebuffer *framebuffer, int x, int y, uint32_t pixel) {
    volatile uint32_t *fb_ptr = (uint32_t *)framebuffer->base;

    fb_ptr[x * (fb->pitch / 4) + y] = pixel;
}

void changeBackgroundColor(aquaboot_framebuffer *framebuffer, uint32_t bgColor) {

    for (int i = 0; i < fb->verticalRes; i++) {
        for (int j = 0; j < fb->horizontalRes; j++) {
            plotPixels(framebuffer,i, j, bgColor);
        }
    }

}

void display_logo(aquaboot_framebuffer *framebuffer)
{
    int k = 0;

    for (int i = 0; i < AQUAOS_LOGO_HEIGHT; i++) {
        for (int j = 0; j < AQUAOS_LOGO_WIDTH; j++) {

            if (aquaos_logo[k] == 0x000000) {
                plotPixels(framebuffer, i + (framebuffer->verticalRes / 4), j + (framebuffer->horizontalRes / 4), AQUABOOT_BG);
            }

            else {
                plotPixels(framebuffer, i + (framebuffer->verticalRes / 4), j + (framebuffer->horizontalRes / 4), aquaos_logo[k]);
            }
            k++;
        }
    }
}