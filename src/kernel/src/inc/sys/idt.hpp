#include <cstdint>

typedef struct
{
    std::uint16_t size;
    std::uint64_t offset;
} __attribute__((packed)) idtr_t;

typedef struct
{
    std::uint16_t isr_low;
    std::uint16_t kernel_cs;
    std::uint8_t ist;   // Bits 2:0 hold the IST, rest should be set to zero
    std::uint8_t attributes;
    std::uint16_t isr_mid;
    std::uint32_t isr_high;
    std::uint32_t reserved;
} __attribute__((packed)) idt_entry_t;