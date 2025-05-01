#pragma once

#include "aquaboot.h"
#include <cstdint>

#define NULL_SEG 0x0
#define KERNEL_CODE_SEG 0x00af9b000000ffff
#define KERNEL_DATA_SEG 0x00af93000000ffff
#define USER_CODE_SEG 0x00af9b000000ffff
#define USER_DATA_SEG 0x00af93000000ffff

typedef struct
{
    std::uint16_t size;
    std::uint64_t offset;
} gdtr_t;

typedef struct
{
    std::uint64_t null_seg;
    std::uint64_t kernel_code;
    std::uint64_t kernel_data;
    std::uint64_t user_code;
    std::uint64_t user_data;
} gdt_t;

void init_gdt(aquaboot_info *boot_info);