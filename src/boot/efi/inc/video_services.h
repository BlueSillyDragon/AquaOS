#pragma once
#include <efi.h>

#define AQUABOOT_BG 0x2B60DE

snowboot_framebuffer *initVideoServices();
void plotPixels (snowboot_framebuffer *framebuffer, int x, int y, uint32_t pixel);
void changeBackgroundColor(snowboot_framebuffer *framebuffer, uint32_t bgColor);