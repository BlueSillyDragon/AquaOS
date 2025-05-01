set(SOURCES
"src/AquaBoot.c"
"src/video_services.c"
"src/disk_services.c"
"src/print.c" "src/filesystem.c"
"src/log.c"
"src/memory_services.c"
"src/virtual_memory.c"
"src/acpi.c"
"src/elf.c"
"src/paging.asm")
add_executable(AquaBoot ${SOURCES})

target_compile_options(AquaBoot
PRIVATE $<$<COMPILE_LANGUAGE:C>:-g>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-target x86_64-unknown-none>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-pipe>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-Wall>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-Wextra>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-nostdinc>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fno-stack-protector>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fno-stack-check>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fshort-wchar>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fno-lto>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fPIE>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-I ../src/boot/efi/limine-efi/inc>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-I ../src/boot/efi/src>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-isystem ../src/boot/efi/freestanding-headers>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-m64>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-march=x86-64>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-80387>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-mmx>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-sse>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-sse2>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-red-zone>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-MMD>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-MP>)

target_link_options(AquaBoot
PRIVATE -fuse-ld=lld
PRIVATE -Wl,-m,elf_x86_64
PRIVATE -nostdlib
PRIVATE -pie
PRIVATE -z text
PRIVATE -Wl,-z,max-page-size=0x1000
PRIVATE -Wl,-gc-sections
PRIVATE -Wl,-T,../src/boot/efi/limine-efi/src/elf_x86_64_efi.lds
PRIVATE ../src/boot/efi/limine-efi/src/crt0-efi-x86_64.S.o
PRIVATE ../src/boot/efi/limine-efi/src/reloc_x86_64.c.o)

add_custom_target(AquaBoot.efi ALL
COMMAND llvm-objcopy -O binary AquaBoot AquaBoot.efi
COMMAND dd if=/dev/zero of=AquaBoot.efi bs=4096 count=0 seek=$$(( ($$(wc -c < AquaBoot.efi) + 4095) / 4096 )) 2>/dev/null
DEPENDS AquaBoot)

add_custom_target(AquaOS.img ALL
COMMAND echo "Generating image file..."
COMMAND dd if=/dev/zero of=AquaOS.img bs=1048576 count=128
COMMAND mkfs.fat -F 32 -n "AquaOS" AquaOS.img
COMMAND mmd -i AquaOS.img ::/EFI
COMMAND mmd -i AquaOS.img ::/EFI/BOOT
COMMAND mcopy -i AquaOS.img AquaBoot.efi ::/EFI/BOOT/BOOTX64.EFI
COMMAND echo "Finished Build!"
DEPENDS AquaBoot.efi)