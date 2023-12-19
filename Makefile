CC = x86_64-w64-mingw32-gcc
SRC_DIR = src
BUILD_DIR = build
GNU_EFI = gnu-efi-3.0.17

.PHONY: all disk_image bootloader link clean always

disk_image: $(BUILD_DIR)/LWD-OS.iso

$(BUILD_DIR)/LWD-OS.iso: bootloader link
		dd if=/dev/zero of=$(BUILD_DIR)/LWD-OS.img bs=512 count=1440
		mformat -i $(BUILD_DIR)/LWD-OS.img -f 1440 ::
		mmd -i $(BUILD_DIR)/LWD-OS.img ::/EFI
		mmd -i $(BUILD_DIR)/LWD-OS.img ::/EFI/BOOT
		mmd -i $(BUILD_DIR)/LWD-OS.img ::/System
		mcopy -i $(BUILD_DIR)/LWD-OS.img $(SRC_DIR)/boot/efi/BOOTX64.EFI ::/EFI/BOOT
		xorriso -as mkisofs -R -f -e LWD-OS.img -no-emul-boot -o LWD-OS.iso
		qemu-system-x86_64 -L OVMF_dir/ -pflash OVMF.fd -cdrom $(BUILD_DIR)/LWD-OS.iso


bootloader: $(SRC_DIR)/LWDBOOT.o

$(SRC_DIR)/LWDBOOT.o:
		$(CC) -ffreestanding -I$(SRC_DIR)/boot/efi/$(GNU_EFI)/inc -I$(SRC_DIR)/boot/efi/$(GNU_EFI)/inc/x86_64 -I$(SRC_DIR)/boot/efi/$(GNU_EFI)/inc/protocol -c -o $(SRC_DIR)/boot/efi/LWDBOOT.o $(SRC_DIR)/boot/efi/LWDBOOT.c
		$(CC) -ffreestanding -I$(SRC_DIR)/boot/efi/$(GNU_EFI)/inc -I$(SRC_DIR)/boot/efi/$(GNU_EFI)/inc/x86_64 -I$(SRC_DIR)/boot/efi/$(GNU_EFI)/inc/protocol -c -o $(SRC_DIR)/boot/efi/data.o $(SRC_DIR)/boot/efi/$(GNU_EFI)/lib/data.c

link:
	$(CC) -nostdlib -Wl,-dll -shared -Wl,--subsystem,10 -e efi_main -o $(SRC_DIR)/boot/efi/BOOTX64.EFI $(SRC_DIR)/boot/efi/LWDBOOT.o $(SRC_DIR)/boot/efi/data.o

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/*
	rm -rf $(SRC_DIR)/boot/efi/LWDBOOT.o
	rm -rf $(SRC_DIR)/boot/efi/data.o
	rm -rf $(SRC_DIR)/boot/efi/BOOTX64.EFI