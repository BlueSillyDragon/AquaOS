dd if=/dev/zero of=SnowOS.img bs=1048576 count=256
sgdisk SnowOS.img -o --new=1:2048:131072 --typecode=1:ef00 --new=2:133120:522239 --typecode=2:8300 -w
sudo losetup --offset 1048576 --sizelimit 67108864 /dev/loop0 SnowOS.img
sudo mkfs.fat -F 32 /dev/loop0
./limine/limine bios-install SnowOS.img
sudo mount /dev/loop0 /mnt
sudo mkdir /mnt/EFI
sudo mkdir /mnt/EFI/BOOT
sudo cp limine/BOOTX64.EFI /mnt/EFI/BOOT/
sudo cp yuki/limine.conf /mnt/EFI/BOOT/
sudo cp limine/limine-bios.sys /mnt/EFI/BOOT/
sudo cp build/yuki/yuki /mnt/
sudo umount /mnt
sudo losetup -d /dev/loop0
sudo losetup --offset 68157440 --sizelimit 267386368 /dev/loop0 SnowOS.img
sudo mkfs.ext2 -b 4096 -L "SnowOS" /dev/loop0
sudo losetup -d /dev/loop0
echo "Finished Generating Image!"
