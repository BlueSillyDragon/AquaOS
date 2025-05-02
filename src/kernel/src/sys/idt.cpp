#include <cstdint>
#include <inc/sys/idt.hpp>
#include <stdint.h>

extern "C" void loadIdtAsm(std::uint16_t limit, std::uint64_t base);

extern void* isr_stub_table[];

Idt::Idt()
{

}

void Idt::idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &IDT[vector];

    descriptor->isr_low        = (uint64_t)isr & 0xFFFF;
    descriptor->seg_select     = 0x08;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->isr_mid        = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high       = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved       = 0;
}

void Idt::initIdt()
{
    IDTR.base = (uint64_t)&IDT;
    IDTR.limit = (uint16_t)sizeof(idt_entry_t) * 256 - 1;

    for(int i = 0; i < 32; i++)
    {
        idt_set_descriptor(i, isr_stub_table[i], 0x8e);
    }

    loadIdtAsm(IDTR.limit, IDTR.base);
}