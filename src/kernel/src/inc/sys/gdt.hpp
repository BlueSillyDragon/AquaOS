#pragma once

#include <cstdint>

#define KERNEL_CS 0x00af9b000000ffff
#define KERNEL_DS 0x00af93000000ffff

#define USER_CS 0x00affb000000ffff
#define USER_DS 0x00aff3000000ffff

typedef struct
{
    std::uint16_t limit;
    std::uint64_t base;
} gdtr_t;

typedef struct
{
    std::uint64_t null_segment;
    std::uint64_t kernel_code;
    std::uint64_t kernel_data;
    std::uint64_t user_code;
    std::uint64_t user_data;
} gdt_t;

void init_gdt();