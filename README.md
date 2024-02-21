# OrigamiOS
![logo-color](https://github.com/BlueSillyDragon/OrigamiOS/assets/137140267/ffb32a89-6194-4d9e-b8a6-74e0e6ccd936)

OrigamiOS is a hobby operating system built from scratch for x86, written in C++.

## How to Build and Run OrigamiOS

> [!NOTE]
> OrigamiOS currently only has a basic bootloader done

> [!WARNING]
> I could only get OrigamiOS to build on Linux.
> It is unknown if it will build on Windows.

### Step 1: Clone repository

If you don't already have the source code, you'll have to clone it using the following commands:
```
$ git clone https://github.com/BlueSillyDragon/OrigamiOS.git
$ cd OrigamiOS
```
### Step 2: Install Dependencies

you'll need to install the needed tools to be able to build
OrigamiOS.
```
$ sudo apt install make mtools g++ xorriso qemu-system-x86 ovmf
```

### Step 2.5: Build/Run OrigamiOS (shell)

You can build and run OrigamiOS using the included `aqua.sh` file.
simply run
```
$ ./aqua.sh
```
And you should be met with a blue screen.

> [!NOTE]
> If you see something like `bash: ./aqua.sh: Permission denied` you might have to run chmod first.
> ```
> chmod u+x ./aqua.sh
> ```
> Run the shell file again, and now it should work.

Below are the steps to make and run OrigamiOS manually.

### Step 2: Build OrigamiOS

First, cd to the 'efi' folder
```
$ cd src/boot/efi
```
then run make
```
$ make
```

### Step 3: Run OrigamiOS

Running OrigamiOS is fairly easy, first, cd back to the base directory:
```
cd ../../../
```
Now run the command below:
```
$ make run
```

And done, you should now see a blue screen along with the AquaOS (Have not changed bootloader's logo yet) logo, you can also look at the serial log if you please.

If it does not work, make sure you have ovmf installed, you can check with
```
$ whereis ovmf
```
it should display something like `ovmf: /usr/share/ovmf` if it's installed.

After confirming ovmf is installed, open up the Makefile, located in the base folder, then find this line.
```
qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -net none -cdrom $(BUILD_DIR)/$(OS_NAME).iso
```
it should be at line 163, change `/usr/share/ovmf/` to the installation directory of ovmf (If the installation directory is something else).

Also make sure you have the other dependencies installed.

## Technologies
Uses eyalroz's printf lib, limine-efi as a UEFI library, and AquaBoot is the custom bootloader.

## License
AquaOS is under an MIT License.
