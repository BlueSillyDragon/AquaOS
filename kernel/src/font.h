#define PSF1_FONT_MAGIC 0x0436

#include <stdint.h>

typedef struct {
    uint16_t magic;
    uint8_t fontMode;
    uint8_t characterSize;
} psf1_header;


#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t headersize;
    uint32_t flags;
    uint32_t numglyph;
    uint32_t bytesperglyph;
    uint32_t height;
    uint32_t width;
} psf_font;