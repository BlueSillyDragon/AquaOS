#include "inc/boot_protocol/snowboot.h"
#include "inc/globals.h"
#include "inc/print.h"
#include "inc/memory_services.h"
#include "inc/video_services.h"
#include <stdint.h>

EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

snowboot_framebuffer *initVideoServices() {
    EFI_STATUS sta;
    snowboot_framebuffer *fb;
    sta = sysT->BootServices->LocateProtocol(&gopGuid, NULL, (void**)&gop);

    if (EFI_ERROR(sta)) {
        print(u"Could not locate GOP, you may be using a pre-UEFI machine, try using a BIOS build of SnowBoot instead.\r\n");
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
        print(u"Could not set video mode! Defaulting to native...\r\n");
        sta = gop->SetMode(gop, 0);
        if (EFI_ERROR(sta)) {
            print(u"Failure to set video mode! Halting...\r\n");
            asm volatile ( "hlt" );
        }
    }

    uint64_t fb_ptr;
    uefiAllocatePages(1, &fb_ptr, EfiReservedMemoryType);
    fb = (uint64_t *)fb_ptr;

    fb->base = gop->Mode->FrameBufferBase;
    fb->size = gop->Mode->FrameBufferSize;
    fb->horizontalRes = gop->Mode->Info->HorizontalResolution;
    fb->verticalRes = gop->Mode->Info->VerticalResolution;
    fb->pixelsPerScanline = gop->Mode->Info->PixelsPerScanLine;
    fb->pitch = (fb->pixelsPerScanline * 4);

    return fb;
}

void plotPixels (snowboot_framebuffer *framebuffer, int x, int y, uint32_t pixel) {
    volatile uint32_t *fb_ptr = (uint32_t *)framebuffer->base;

    fb_ptr[x * (framebuffer->pitch / 4) + y] = pixel;
}

void changeBackgroundColor(snowboot_framebuffer *framebuffer, uint32_t bgColor) {

    for (int i = 0; i < framebuffer->verticalRes; i++) {
        for (int j = 0; j < framebuffer->horizontalRes; j++) {
            plotPixels(framebuffer,i, j, bgColor);
        }
    }

}