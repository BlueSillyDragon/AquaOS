set(SOURCES "src/AquaKernel.cpp" "src/kprint.cpp" "src/sys/gdt.cpp" "src/exception.cpp" "src/sys/idt.asm" "src/gdt.asm")

add_executable(aquakernel.elf ${SOURCES})

target_include_directories(
    aquakernel.elf
    PRIVATE freestnd-c-hdrs/x86_64/include
    PRIVATE freestnd-cxx-hdrs/x86_64/include
    private ../boot/efi/src/inc/boot_protocol
)

target_compile_options(aquakernel.elf
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wall>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-g>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-target x86_64-unknown-none>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-O2>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-nostdinc>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-ffreestanding>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-stack-protector>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-stack-check>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-PIC>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-ffunction-sections>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fdata-sections>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-I ../src/kernel/src>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-m64>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-march=x86-64>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mno-80387>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mno-mmx>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mno-sse>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mno-sse2>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mno-red-zone>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mcmodel=kernel>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-MMD>
PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-MP>)

target_link_options(aquakernel.elf
PRIVATE -Wl,-m,elf_x86_64
PRIVATE -Wl,--build-id=none
PRIVATE -nostdlib
PRIVATE -static
PRIVATE -z max-page-size=0x1000
PRIVATE -Wl,--gc-sections
PRIVATE -T ../src/kernel/linker.ld)