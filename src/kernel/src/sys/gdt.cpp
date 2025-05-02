#include <cstdint>
#include <inc/sys/gdt.hpp>
#include <stdint.h>

extern "C" void loadGdtAsm(std::uint16_t limit, std::uint64_t base);
extern "C" void reloadSegs(void);

Gdt::Gdt()
{

}

void Gdt::loadGdt()
{
    GDTR.base = reinterpret_cast<uint64_t>(&GDTDescs);
    GDTR.limit = (sizeof(GDTDescs) - 1);
    loadGdtAsm(GDTR.limit, GDTR.base);
    reloadSegs();
}