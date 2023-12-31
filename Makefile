SRC_DIR = src
BUILD_DIR = build
BOOTEFI_DIR = $(SRC_DIR)/boot/efi

OS_NAME=AquaOS

.PHONY: all disk_image make_iso run bootloader clean always

make_iso: always disk_image
	mcopy -i $(BUILD_DIR)/$(OS_NAME).img $(BOOTEFI_DIR)/bin/AQUABOOT.EFI ::/EFI/BOOT/BOOTX64.EFI
	xorriso -as mkisofs -R -f -e $(OS_NAME).img -no-emul-boot -o $(BUILD_DIR)/$(OS_NAME).iso $(BUILD_DIR)

disk_image:
	dd if=/dev/zero of=$(BUILD_DIR)/$(OS_NAME).img bs=1K count=1440
	mformat -i $(BUILD_DIR)/$(OS_NAME).img -f 1440 ::
	mmd -i $(BUILD_DIR)/$(OS_NAME).img ::/EFI
	mmd -i $(BUILD_DIR)/$(OS_NAME).img ::/EFI/BOOT
	mmd -i $(BUILD_DIR)/$(OS_NAME).img ::/System

.PHONY: run

run:
	qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -net none -cdrom $(BUILD_DIR)/$(OS_NAME).iso

always:
	mkdir -p $(BUILD_DIR)

.PHONY: clean

clean:
	rm -rf $(BUILD_DIR)/*
	rm -rf iso
