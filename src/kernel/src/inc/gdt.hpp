#pragma once

#include <cstdint>
#include <stdint.h>

#define NULL_SEG 0x0
#define KERNEL_CODE_SEG 0x00af9b000000ffff
#define KERNEL_DATA_SEG 0x00af93000000ffff
#define USER_CODE_SEG 0x00af9b000000ffff
#define USER_DATA_SEG 0x00af93000000ffff

typedef struct
{
    std::uint16_t size;
    std::uint64_t offset;
} GDTR;

typedef struct
{
    uint64_t null_seg;
    uint64_t kernel_code;
    uint64_t kernel_data;
    uint64_t user_code;
    uint64_t user_data;
} GDT;