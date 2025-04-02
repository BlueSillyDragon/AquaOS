# AquaOS
![aquaos-high-resolution-logo](https://github.com/user-attachments/assets/5438a573-6d78-4a0d-bb0a-c9aa12c1e15f)

AquaOS is a hobby operating system built from scratch for x86, written in C++.

## How to Build and Run AquaOS

> [!NOTE]
> AquaOS currently only has a basic bootloader done
> So no kernel yet, sorry

> [!WARNING]
> I have only built AquaOS on Linux, I don't know if it will build on Windows
> (What're doing trying to do OSDev stuff on Windows anyways?)

### Step 1: Clone repository

If you don't already have the source code, you'll have to clone it using the following commands:
```
$ git clone https://github.com/BlueSillyDragon/AquaOS.git
$ cd AquaOS
```

### Step 2: Install Dependencies

You'll need to install the needed tools to be able to build AquaOS if you don't have them installed already.
```
$ sudo apt install cmake clang ninja-build lld llvm
```
Also install qemu and ovmf for running AquaOS, you can skip this iof you're going to use a different VM though (eg. VirtualBox)
> [!WARNING]
> I can't guarantee that AquaOS will run on VirtualBox, but feel free to try

### Step 3: Run get-deps and build Limine-Efi

AquaOS uses the Limine-Efi library for it's bootloader, hence, you will need to build it.
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
$ FAILED: src/boot/efi/CMakeFiles/AquaBoot.efi /home/user/Desktop/AquaOS/build/src/boot/efi/CMakeFiles/AquaBoot.efi 
$ cd /home/user/Desktop/AquaOS/build/src/boot/efi && llvm-objcopy -O binary AquaBoot AquaBoot.efi && dd if=/dev/zero of=AquaBoot.efi bs=4096 count=0 seek=$ ( ( ( $ ( wc -c < AquaBoot.efi ) + 4095 ) / 4096 ) ) 2>/dev/null
$ /bin/sh: 1: Syntax error: "(" unexpected
$ ninja: build stopped: subcommand failed.
```
Don't panic, for some reason the spacing on the last command is messed up, you can just fix it manually.
First open build.ninja in whatever code editor you use. Then go to line 172.
Go near the end and delete everything after "seek=" and replace it with this "$$(( ($$(wc -c < AquaBoot.efi) + 4095) / 4096)) 2>/dev/null" should look like this after you're done.
```
COMMAND = cd /home/bluesillydragon/Desktop/AquaOS/build/src/boot/efi && llvm-objcopy -O binary AquaBoot AquaBoot.efi && dd if=/dev/zero of=AquaBoot.efi bs=4096 count=0 seek=$$(( ($$(wc -c < AquaBoot.efi) + 4095) / 4096)) 2>/dev/null
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

### Step 6: Run AquaOS

Now all you have to do is run AquaOS under QEMU (or your VM of choice, just remember that you need to configure the VM settings to use UEFI).
Cd to the directory the .img file is in (if you're not in the build directory already for some reason, cd there first)
```
$ cd src/boot/efi
```
Now all you gotta do is run qemu with the command below
```
$ qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -serial stdio -hda AquaOS.img
```
And presto! You should be in AquaOS now! you should be seeing something like this.
```
AquaBoot version 0.1.0
Initializing Serial Services...
Initializing Disk Services...
Initializing FileSystem Services...
the AquaOS Kernel must be located on an Ext2 Partition!
```
Congratulations! You have succesfully built and ran AquaOS!

## Technologies
limine-efi as a UEFI library, and AquaBoot is the custom bootloader.

## License
AquaOS is under an MIT License.

BlueSillyDragon (c) 2023, 2025
