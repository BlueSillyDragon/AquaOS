#pragma once
#include <efi.h>

typedef struct {
    EFI_PHYSICAL_ADDRESS base;
    UINTN size;
    UINT32 horizontalRes;
    UINT32 verticalRes;
    UINT32 pixelsPerScanline;
    UINT32 pitch;
} AQUABOOT_FRAMEBUFFER;

AQUABOOT_FRAMEBUFFER initGop(EFI_SYSTEM_TABLE* ST);
void plotPixels (int x, int y, uint32_t pixel, AQUABOOT_FRAMEBUFFER fb);
void changeBackgroundColor(AQUABOOT_FRAMEBUFFER fb, uint32_t bgColor);