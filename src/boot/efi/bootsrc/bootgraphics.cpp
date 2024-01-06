#include <efi.h>
#include <stdint.h>

#include "icon/logo.h"
#include "bootinc/bootgraphics.h"

    void plotPixels (int x, int y, uint32_t pixel, EFI_GRAPHICS_OUTPUT_PROTOCOL* gop) {
        uint32_t pixelbytes = (uint32_t)4;
        uint32_t pitch = pixelbytes * gop->Mode->Info->PixelsPerScanLine;
        *((uint32_t*) (gop->Mode->FrameBufferBase + pitch * y + 4 * x)) = pixel;
    }

    void displayLogo(int x, int y, int offX, int offY, uint32_t pixel, EFI_GRAPHICS_OUTPUT_PROTOCOL* gop) {

        for (int i = 0; i < 810000; i++) {

            if (x >= AQUAOS_LOGO_WIDTH) {
                x = 0;
                y++;
            }

            if (y == AQUAOS_LOGO_HEIGHT) {
                break;
            }

            pixel = aquaos_logo[i];
            plotPixels((x + offX), (y + offY), pixel, gop);
            x++;
        }
    }