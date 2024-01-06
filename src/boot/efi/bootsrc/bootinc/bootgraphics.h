#ifndef BOOTGRAPHICS_H
#define BOOTGRAPHICS_H

void displayLogo(int x, int y, int offX, int offY, uint32_t pixel, EFI_GRAPHICS_OUTPUT_PROTOCOL* gop);

void plotPixels (int x, int y, uint32_t pixel, EFI_GRAPHICS_OUTPUT_PROTOCOL* gop);

#endif