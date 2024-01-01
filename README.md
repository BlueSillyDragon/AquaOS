# AquaOS
![logo-color](https://github.com/BlueSillyDragon/AquaOS/assets/137140267/6f4ebc66-f195-4c9b-aef8-98412433e9f4)

A x86-64 64-bit OS for programmed in C/C++.

It uses a custom UEFI Bootloader.

## How to Build and Run AquaOS

> [!NOTE]
> AquaOS currently only has a basic bootloader done

### Step 1: Clone repository

If you don't already have the source code, you'll have to clone it using the following commands:
```
$ git clone https://github.com/BlueSillyDragon/AquaOS.git
$ cd AquaOS
```
### Step 1.5: Build/Run AquaOS (shell)

You can build and run AquaOS using the included `aqua.sh` file.
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

Below are the steps to make and run AquaOS manually.

### Step 2: Build AquaOS

First, cd to the 'efi' folder
```
$ cd src/boot/efi
```
then run make
```
$ make
```

### Step 3: Run AquaOS

> [!WARNING]
> You **MUST** have OVMF installed for this to work!
> You can install ovmf using the following command (for Ubuntu/Debian)
> ```
> $ sudo apt install ovmf
> ```

Running AquaOS is fairly easy, first, cd back to the base directory:
```
cd ../../../
```
Now run the command below:
```
$ make run
```

And done, you should now see a blue screen, you can also look at the serial log if you please.

If it does not work, make sure you have ovmf installed, you can check with
```
$ whereis ovmf
```
it should display something like `ovmf: /usr/share/ovmf` if it's installed.

After confirming ovmf is installed, open up the Makefile, located in the base folder, then find this line.
```
qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -net none -cdrom $(BUILD_DIR)/$(OS_NAME).iso
```
it should be at line 163, change `/usr/share/ovmf/` to the installation directory of ovmf.

## Technologies
Uses eyalroz's printf lib, and limine-efi as a UEFI library.

## License
AquaOS is under an MIT License.
