# AquaOS
![aquaos-high-resolution-logo](https://github.com/user-attachments/assets/5438a573-6d78-4a0d-bb0a-c9aa12c1e15f)

AquaOS is a hobby operating system built from scratch for x86, written in C++.

## How to Build and Run AquaOS

> [!NOTE]
> This is branch uses Limine as the bootloader

> [!WARNING]
> I have only tried to AquaOS build on Linux.
> It is unknown if it will build on Windows.

### Step 1: Clone repository

If you don't already have the source code, you'll have to clone it using the following commands:
```
$ git clone https://github.com/BlueSillyDragon/AquaOS.git
$ cd AquaOS
```
### Step 2: Install Dependencies

you'll need to install the needed tools to be able to build
AquaOS.
```
$ sudo apt install make mtools g++ ld xorriso qemu-system-x86
```
You'll also have to run the `get-deps` file
```
$ cd kernel
$ ./get-deps
```
> [!NOTE]
> If you see something like `bash: ./get-deps: Permission denied` you might have to run chmod first.
> ```
> chmod +x ./get-deps
> ```
> Run the shell file again, and now it should work.

### Step 3: Build AquaOS

Just run make in the base directory
```
$ make
```

### Step 3: Run AquaOS

To run the iso file run
```
qemu-system-x86_64 -cdrom AquaOS.iso
```
And you should be booted into the kernel!

## Technologies
Uses eyalroz's printf lib, limine-efi as a UEFI library, and AquaBoot is the custom bootloader.

## License
AquaOS is under an MIT License, read the LICENSE file, or visit this site: https://opensource.org/license/MIT.
