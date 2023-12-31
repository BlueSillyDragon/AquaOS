# AquaOS
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

Running AquaOS is fairly easy, simply run the following command:
```
$ make run
```

And done, you should now see the text 'AquaOS Bootloader has loaded successfully!'

If it does not work, make sure you have ovmf installed, you can check with
```
$ whereis ovmf
```
it should display something like `ovmf: /usr/share/ovmf` if it's installed.

After confirming ovmf is installed, open up the Makefile, located in the base folder, then find this line.
```
qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -net none -cdrom $(BUILD_DIR)/$(OS_NAME).iso
```
it should be at line 163, change `/usr/share/ovmf` to the installation directory of ovmf.
