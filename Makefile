CC = gcc
CCFLAGS = -ffreestanding -c

LD = elf_x86_64_efi.lds
LDFLAGS = -nostdlib -Wl,-dll -shared -Wl, -T $(SRC_DIR)/boot/efi/$(GNU_EFI)/gnuefi/$(LD) -e efi_main

SRC_DIR = src
BUILD_DIR = build
GNU_EFI = limine-efi

.PHONY: all disk_image bootloader link clean always

disk_image: $(BUILD_DIR)/LWD-OS.iso

$(BUILD_DIR)/LWD-OS.iso: bootloader link
		dd if=/dev/zero of=$(BUILD_DIR)/LWD-OS.img bs=1K count=1440
		mformat -i $(BUILD_DIR)/LWD-OS.img -f 1440 ::
		mmd -i $(BUILD_DIR)/LWD-OS.img ::/EFI
		mmd -i $(BUILD_DIR)/LWD-OS.img ::/EFI/BOOT
		mmd -i $(BUILD_DIR)/LWD-OS.img ::/System
		mcopy -i $(BUILD_DIR)/LWD-OS.img $(SRC_DIR)/boot/efi/BOOTX64.EFI ::/EFI/BOOT
		xorriso -as mkisofs -R -f -e LWD-OS.img -no-emul-boot -o LWD-OS.iso
		qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -net none -cdrom $(BUILD_DIR)/LWD-OS.iso


bootloader: $(SRC_DIR)/LWDBOOT.o

$(SRC_DIR)/LWDBOOT.o:
		$(CC) $(CCFLAGS) -I$(SRC_DIR)/boot/efi/$(GNU_EFI)/inc -I$(SRC_DIR)/boot/efi/$(GNU_EFI)/inc/x86_64 -I$(SRC_DIR)/boot/efi/$(GNU_EFI)/inc/protocol -o $(SRC_DIR)/boot/efi/LWDBOOT.o $(SRC_DIR)/boot/efi/LWDBOOT.c

link:
	$(CC) $(LDFLAGS) -o $(SRC_DIR)/boot/efi/BOOTX64.EFI $(SRC_DIR)/boot/efi/LWDBOOT.o

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/*
	rm -rf $(SRC_DIR)/boot/efi/LWDBOOT.o
	rm -rf $(SRC_DIR)/boot/efi/BOOTX64.EFI
