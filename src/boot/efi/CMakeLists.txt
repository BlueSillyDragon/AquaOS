set(SOURCES "src/AquaBoot.c" "src/video_services.c")

add_executable(AquaBoot.elf ${SOURCES})

target_compile_options(AquaBoot.elf
PRIVATE -target x86_64-unknown-none
PRIVATE -O2
PRIVATE -pipe
PRIVATE -Wall
PRIVATE -Wextra
PRIVATE -std=gnu11
PRIVATE -nostdinc
PRIVATE -ffreestanding
PRIVATE -fno-stack-protector
PRIVATE -fno-stack-check
PRIVATE -fshort-wchar
PRIVATE -fno-lto
PRIVATE -fPIE
PRIVATE -ffunction-sections
PRIVATE -fdata-sections
PRIVATE -I ${CMAKE_SOURCE_DIR}/src/boot/efi/limine-efi/inc
PRIVATE -isystem ${CMAKE_SOURCE_DIR}/src/boot/efi/freestanding-headers
PRIVATE -m64
PRIVATE -march=x86-64
PRIVATE -mno-80387
PRIVATE -mno-mmx
PRIVATE -mno-sse
PRIVATE -mno-sse2
PRIVATE -mno-red-zone)

target_link_options(AquaBoot.elf
PRIVATE -nostdlib
PRIVATE -pie
PRIVATE -z text
PRIVATE -T ${CMAKE_SOURCE_DIR}/src/boot/efi/limine-efi/src/elf_x86_64_efi.lds
PRIVATE ${CMAKE_SOURCE_DIR}/src/boot/efi/limine-efi/src/crt0-efi-x86_64.S.o
PRIVATE ${CMAKE_SOURCE_DIR}/src/boot/efi/limine-efi/src/reloc_x86_64.c.o)

add_custom_target(AquaBoot.efi ALL
COMMAND llvm-objcopy -O binary AquaBoot.elf AquaBoot.efi
DEPENDS AquaBoot.elf)

add_custom_target(AquaOS.iso ALL
COMMAND echo "Generating iso..."
COMMAND dd if=/dev/zero of=AquaOS.img bs=1048576 count=128
COMMAND mkfs.fat -F 32 -n "AquaOS" AquaOS.img
COMMAND mmd -i AquaOS.img ::/EFI
COMMAND mmd -i AquaOS.img ::/EFI/BOOT
COMMAND mcopy -i AquaOS.img AquaBoot.efi ::/EFI/BOOT/BOOTX64.EFI
COMMAND xorriso -as mkisofs -R -f -e AquaOS.img -no-emul-boot -o AquaOS.iso
COMMAND echo "Finished Build!"
DEPENDS AquaBoot.efi)