# SnowOS

SnowOS is a microkernel-based hobby operating system built from scratch in C++ targeted at the x86 architecture.

## How to Build and Run SnowOS

### Step 1: Clone repository

If you don't already have the source code, you'll have to clone it using the following commands:
```
$ git clone https://github.com/BlueSillyDragon/SnowOS.git
$ cd SnowOS
```

### Step 2: Install Dependencies

You'll need to install the needed tools to be able to build SnowOS if you don't have them installed already.
```
$ sudo apt install cmake clang ninja-build lld llvm
```
Also install qemu and ovmf for running SnowOS, you can skip this if you're going to use a different VM though (eg. VirtualBox)
> [!WARNING]
> I can't guarantee that SnowOS will run on VirtualBox, but feel free to try

### Step 3: Run get-deps and build Limine-Efi

SnowOS uses the Limine-Efi library for it's bootloader, hence, you will need to build it.
First go to the src dir of the efi bootloader
```
$ cd src/boot/efi
```
Then run get-deps
```
$ ./get-deps
```
Now limine-efi should've been cloned into the current directory.
Next up is building limine-efi, this is simple enough.
```
$ cd limine-efi/src
$ make -f limine-efi.mk
```
Now you're free to return to the main directory.
```
$ cd ../../../../../
```

### Step 4: Run CMake

First, you'll need to create a build dir, call it whatever you want, I'll call mine build here
```
$ mkdir build
```
cd into there, and then run the command for CMake below
```
$ cd build
$ cmake .. -GNinja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
```

### Step 5: Run Ninja

You should still be in the build directory, so all you have to do us run Ninja.
```
$ ninja
```
Now you'll probably see something like this:
```
$ FAILED: src/boot/efi/CMakeFiles/snowboot.efi /home/user/Desktop/SnowOS/build/src/boot/efi/CMakeFiles/snowboot.efi 
$ cd /home/user/Desktop/SnowOS/build/src/boot/efi && llvm-objcopy -O binary snowboot snowboot.efi && dd if=/dev/zero of=snowboot.efi bs=4096 count=0 seek=$ ( ( ( $ ( wc -c < snowboot.efi ) + 4095 ) / 4096 ) ) 2>/dev/null
$ /bin/sh: 1: Syntax error: "(" unexpected
$ ninja: build stopped: subcommand failed.
```
Don't panic, for some reason the spacing on the last command is messed up, you can just fix it manually.
First open build.ninja in whatever code editor you use. Then go to line 172.
Go near the end and delete everything after "seek=" and replace it with this "$$(( ($$(wc -c < snowboot.efi) + 4095) / 4096)) 2>/dev/null" should look like this after you're done.
```
...seek=$$(( ($$(wc -c < snowboot.efi) + 4095) / 4096)) 2>/dev/null
```

Now save and run ninja again, if everything goes well, you should see something like this.
```
$ Generating image file...
$ 128+0 records in
$ 128+0 records out
$ 134217728 bytes (134 MB, 128 MiB) copied, 0.0290467 s, 4.6 GB/s
$ mkfs.fat: Warning: lowercase labels might not work properly on some systems
$ mkfs.fat 4.2 (2021-01-31)
$ Finished Build!
```

### Step 6: Run SnowOS

Now all you have to do is run SnowOS under QEMU (or your VM of choice, just remember that you need to configure the VM settings to use UEFI).
Cd to the directory the .img file is in (if you're not in the build directory already for some reason, cd there first)
```
$ cd src/boot/efi
```
Now all you gotta do is run qemu with the command below
```
$ qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -serial stdio -hda SnowOS.img
```
And presto! You should be in SnowOS now! you should be seeing something like this.
```
SnowBoot version 0.1.0
Initializing Serial Services...
Initializing Disk Services...
Initializing FileSystem Services...
the SnowOS Kernel must be located on an Ext2 Partition!
```
Congratulations! You have succesfully built and ran SnowOS!

However, you would probably like to be able to boot into the kernel. SnowBoot can't load the kernel off a FAT FS yet (gotta add that). This is simple enough.

### Step 7: Booting into kernel

You can use anything that can be partitioned, like an .img, I'll be using a USB in my example.

First, partition the device to have an ESP (EFI System Partition) and an Ext2 Partition. Now all you have to do is copy the bootloader file and kernel file
to the proper locations. SnowBoot should be copied to EFI/BOOT/* and rename SnowBoot.efi to BOOTX64.EFI
> [!WARNING]
> If you're trying to put this on real hardware, **PLEASE** make sure that you copy it to a different location.
> I would recommend something like SnowOS/snowboot.efi, and then add that to your boot options.

Then copy Yuki (SnowOS' kernel) to the Ext2 partition at /Snow64/System/* make sure it's called yuki.elf

Specifically, I have these commands in a .sh file to make my life easier (feel free to use it if you want)
```
sudo losetup --offset 1048576 --sizelimit 46934528 /dev/loop0 /path/to/device
sudo mount /dev/loop0 /mnt
sudo cp /path/to/bootloader /mnt/EFI/BOOT/BOOTX64.EFI
sudo umount /mnt
sudo losetup -d /dev/loop0
sudo losetup /dev/loop0 /path/to/device/part2
sudo mount /dev/loop0 /mnt
sudo rm /mnt/Snow64/System/yuki.elf
sudo cp /path/to/kernel /mnt/Snow64/System
sudo umount /mnt
sudo losetup -d /dev/loop0
```
Now when you run SnowOS, you should be booted into the kernel.

## Technologies
limine-efi by mintsuki as a UEFI library, uses tinyubsan by rdmsr, and SnowBoot is the custom bootloader.

## License
SnowOS is under an GPLv3 License.

BlueSillyDragon (c) 2023, 2025
