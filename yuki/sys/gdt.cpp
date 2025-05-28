#include <cstdint>
#include <inc/terminal.hpp>
#include <inc/sys/gdt.hpp>

extern "C" void loadGdtAsm(std::uint16_t limit, std::uint64_t base);
extern "C" void reloadSegs(void);

extern Terminal kernTerminal;

gdtr_t gdtr;
gdt_t gdt;

void initGdt()
{
    // Set the segments
    gdt.null_segment = 0x0;
    gdt.kernel_code = KERNEL_CS;
    gdt.kernel_data = KERNEL_DS;
    gdt.user_code = USER_CS;
    gdt.user_data = USER_DS;

    // Set the GDTR
    gdtr.base = reinterpret_cast<std::uint64_t>(&gdt);
    gdtr.limit = (sizeof(gdt) - 1);

    // Load the GDT and reload segment registers
    loadGdtAsm(gdtr.limit, gdtr.base);
    reloadSegs();

    kernTerminal.ksuccess("GDT Initialized!\n");
}