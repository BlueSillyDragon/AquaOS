// This header defines structures that AquaBoot will pass to the Kernel
#pragma once

#include <stdint.h>

typedef struct {
    uint64_t base;
    uint64_t size;
    uint32_t horizontalRes;
    uint32_t verticalRes;
    uint32_t pixelsPerScanline;
    uint32_t pitch;
} aquaboot_framebuffer;

typedef struct
{
    uint32_t type;
    uint64_t physical_start;
    uint64_t virtual_start;
    uint64_t num_of_pages;
    uint64_t attribute;
} aquaboot_memory_descriptor;

typedef struct
{
    uint32_t aquaboot_major;    // We'll pass the version of AquaBoot to Kernel for the sake of it
    uint32_t aquaboot_minor;
    uint32_t aquaboot_patch;

    uint64_t hhdm;  // Pass the Higher-Half Direct Map to the Kernel

    aquaboot_framebuffer *framebuffer;  // Obviously we need to pass the framebuffer to the Kernel
} aquaboot_info;