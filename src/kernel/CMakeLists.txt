set(SOURCES "src/AquaKernel.cpp")

add_executable(aquakernel.elf ${SOURCES})

target_compile_options(aquakernel.elf
PRIVATE -Wall
PRIVATE -target x86_64-unknown-none
PRIVATE -Wextra
PRIVATE -nostdinc
PRIVATE -ffreestanding
PRIVATE -fno-stack-protector
PRIVATE -fno-stack-check
PRIVATE -fno-PIC
PRIVATE -ffunction-sections
PRIVATE -fdata-sections
PRIVATE -m64
PRIVATE -march=x86-64
PRIVATE -mno-80387
PRIVATE -mno-mmx
PRIVATE -mno-sse
PRIVATE -mno-sse2
PRIVATE -mno-red-zone
PRIVATE -mcmodel=kernel)

target_link_options(aquakernel.elf
PRIVATE -Wl,-m,elf_x86_64
PRIVATE -Wl,--build-id=none
PRIVATE -nostdlib
PRIVATE -static
PRIVATE -z max-page-size=0x1000
PRIVATE -Wl,--gc-sections
PRIVATE -T ../src/kernel/linker.ld)