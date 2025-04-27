#pragma once
#include <efi.h>

#define AQUABOOT_BG 0x2B60DE

void init_video_services(aquaboot_framebuffer *framebuffer);
void plotPixels (int x, int y, uint32_t pixel);
void changeBackgroundColor(uint32_t bgColor);
void display_logo();