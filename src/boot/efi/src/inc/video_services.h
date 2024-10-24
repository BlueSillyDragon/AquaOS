#pragma once
#include <efi.h>

#define AQUABOOT_BG 0x2B60DE

typedef struct {
    EFI_PHYSICAL_ADDRESS base;
    UINTN size;
    UINT32 horizontalRes;
    UINT32 verticalRes;
    UINT32 pixelsPerScanline;
    UINT32 pitch;
} aquaboot_framebuffer;

void init_video_services(aquaboot_framebuffer *framebuffer);
void plotPixels (int x, int y, uint32_t pixel);
void changeBackgroundColor(uint32_t bgColor);
void display_logo();