// This header defines structures that AquaBoot will pass to the Kernel
#pragma once

#include <stdint.h>

// Memory Descriptor Types
typedef enum {
    SNOWOS_RESERVED,
    SNOWOS_RUNTIME_SERVICES,
    SNOWOS_ACPI_RECLAIM,
    SNOWOS_BOOT_RECLAIM,
    SNOWOS_FREE_MEMORY
} SNOWBOOT_MEM_TYPE;

typedef struct {
    uint64_t base;
    uint64_t size;
    uint32_t horizontalRes;
    uint32_t verticalRes;
    uint32_t pixelsPerScanline;
    uint32_t pitch;
} snowboot_framebuffer;

typedef struct
{
    uint32_t type;
    uint64_t physicalStart;
    uint64_t virtualStart;
    uint64_t numOfPages;
    uint64_t attribute;
} snowboot_memory_descriptor;

typedef struct
{
    uint32_t snowbootMajor;    // We'll pass the version of SnowBoot to Kernel for the sake of it
    uint32_t snowbootMinor;
    uint32_t snowbootPatch;

    uint64_t hhdm;  // Pass the Higher-Half Direct Map to the Kernel

    snowboot_framebuffer *framebuffer;  // Obviously we need to pass the framebuffer to the Kernel

    uint64_t memMapEntries;
    uint64_t descSize;
    snowboot_memory_descriptor *memoryMap;

    uint64_t kernelPaddr;  // Pass the physical address of the kernel so it can remap itself
} snowboot_info;