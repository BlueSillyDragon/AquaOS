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

class Gdt
{   
    public:
        Gdt();      // This will set the segments of the GDT, as well as the Base and Limit of the GDTR
        void loadGdt();
        gdtr_t GDTR;
        gdt_t GDTDescs;
};