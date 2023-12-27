# LWD-OS
A x86-64 64-bit OS for programmed in C/C++.

It uses a custom UEFI Bootloader.

## How to Build and Run LWD-OS

> [!NOTE]
> LWD-OS currently only has a basic bootloader done

### Step 0: Clone repository

If you don't already have the source code, you'll have to clone it using the following commands:

```$ git clone https://github.com/BlueSillyDragon/LWD-OS.git```

You can then move to the directory:

```$ cd LWD-OS```

### Step 2: Build LWD-OS

First, cd to the 'efi' folder

```$ cd src/boot/efi```

then run make

```$ make```

### Step 3: Run LWD-OS

> [!WARNING]
> You **MUST** have OVMF installed for this to work!
> You can install ovmf using the following command (for Ubuntu/Debian)
> ```$ sudo apt install ovmf```

Running LWD-OS is fairly easy, simply run the following command:

```make run```

And done!
