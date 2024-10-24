set(SOURCES "src/AquaBoot.c" "src/video_services.c" "src/disk_services.c" "src/print.c" "src/filesystem.c" "src/log.c")

add_executable(AquaBoot ${SOURCES})

target_compile_options(AquaBoot
PRIVATE -g
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
PRIVATE -I ../src/boot/efi/limine-efi/inc
PRIVATE -I ../src/boot/efi/src
PRIVATE -isystem ../src/boot/efi/freestanding-headers
PRIVATE -m64
PRIVATE -march=x86-64
PRIVATE -mno-80387
PRIVATE -mno-mmx
PRIVATE -mno-sse
PRIVATE -mno-sse2
PRIVATE -mno-red-zone
PRIVATE -MMD
PRIVATE -MP)

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