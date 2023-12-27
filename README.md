# LWD-OS
A x86-64 64-bit OS for programmed in C/C++.

It uses a custom UEFI Bootloader.

## How to Build and Run LWD-OS

> [!NOTE]
> LWD-OS currently only has a basic bootloader done

### Step 1: Clone repository

If you don't already have the source code, you'll have to clone it using the following commands:
```
$ git clone https://github.com/BlueSillyDragon/LWD-OS.git
$ cd LWD-OS
```

### Step 2: Build LWD-OS

First, cd to the 'efi' folder
```
$ cd src/boot/efi
```
then run make
```
$ make
```

### Step 3: Run LWD-OS

> [!WARNING]
> You **MUST** have OVMF installed for this to work!
> You can install ovmf using the following command (for Ubuntu/Debian)
> ```
> $ sudo apt install ovmf
> ```

Running LWD-OS is fairly easy, simply run the following command:
```
$ make run
```

And done, you should now see the text 'LWD-OS Bootloader has successfully started!'

If it does not work, make sure you have ovmf installed, you can check with
```
$ whereis ovmf
```
it should display something like `ovmf: /usr/share/ovmf` if it's installed.

After confirming ovmf is installed, open up the Makefile, located in efi, then find this line.
```
qemu-system-$(ARCH) -net none -M q35 -bios /usr/share/ovmf/OVMF.fd -drive file=fat:rw:boot
```
it should be at line 163, change `/usr/share/ovmf` to the installation directory of ovmf.
