set (aquaboot_sources 
                    "bootsrc/AquaBoot.cpp" 
                    "bootsrc/bootgraphics.cpp" 
                    "bootsrc/loader.cpp" 
                    "bootsrc/pmm.cpp" 
                    "bootsrc/print.cpp" 
                    "bootsrc/vmm.cpp")

message("AquaBoot source files: ${aquaboot_sources}")

set(linker_script "${CMAKE_SOURCE_DIR}/src/boot/efi/limine-efi/gnuefi/elf_x86_64_efi.lds")

add_executable(AquaBoot.elf ${aquaboot_sources})

set_target_properties(AquaBoot.elf PROPERTIES LINK_DEPENDS "${linker_script}")
set_target_properties(AquaBoot.elf PROPERTIES CXX_STANDARD 20)
set_target_properties(AquaBoot.elf PROPERTIES CXX_STANDARD_REQUIRED 20)

target_include_directories(AquaBoot.elf PUBLIC ${CMAKE_SOURCE_DIR}/src/boot/efi/limine-efi/inc)
target_include_directories(AquaBoot.elf PUBLIC ${CMAKE_SOURCE_DIR}/src/boot/efi/limine-efi/inc/x86_64)
target_include_directories(AquaBoot.elf PUBLIC ${CMAKE_SOURCE_DIR}/src/boot/efi/bootsrc)

target_compile_options(AquaBoot.elf 
                                PUBLIC "-Wall"
                                PUBLIC "-Wextra"
                                PUBLIC "-O2"
                                PUBLIC "-ffreestanding" 
                                PUBLIC "-fno-stack-check" 
                                PUBLIC "-fno-stack-protector" 
                                PUBLIC "-fno-lto"
                                PUBLIC "-fno-rtti" 
                                PUBLIC "-fPIE")

target_link_options(AquaBoot.elf                              
                                PUBLIC "-nostdlib" 
                                PUBLIC "-static" 
                                PUBLIC "-pie"
                                PUBLIC "-Wl,--no-dynamic-linker"
                                PUBLIC "-T" PUBLIC "${linker_script}"
                                PUBLIC "${CMAKE_SOURCE_DIR}/src/boot/efi/limine-efi/gnuefi/crt0-efi-x86_64.S.o"
                                PUBLIC "${CMAKE_SOURCE_DIR}/src/boot/efi/limine-efi/gnuefi/reloc_x86_64.c.o")

add_custom_target(generate_efi ALL 
                                COMMAND x86_64-elf-objcopy -O binary AquaBoot.elf AquaBoot.EFI
                                DEPENDS AquaBoot.elf)

add_custom_target(generate_iso ALL
                                COMMAND echo "Generating iso..."
                                COMMAND dd if=/dev/zero of=AquaOS.img bs=1048576 count=128
                                COMMAND mkfs.fat -F 32 -n "AquaOS" AquaOS.img
                                COMMAND mmd -i AquaOS.img ::/EFI
                                COMMAND mmd -i AquaOS.img ::/EFI/BOOT
                                COMMAND mmd -i AquaOS.img ::/System
                                COMMAND mmd -i AquaOS.img ::/System/Aqua64
                                COMMAND mmd -i AquaOS.img ::/System/Users
                                COMMAND mcopy -i AquaOS.img AquaBoot.EFI ::/EFI/BOOT/BOOTX64.EFI
                                COMMAND xorriso -as mkisofs -R -f -e AquaOS.img -no-emul-boot -o AquaOS.iso
                                COMMAND echo "Finished Build!"
                                DEPENDS generate_efi)