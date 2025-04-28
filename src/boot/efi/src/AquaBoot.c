#include <efi.h>
#include <stdarg.h>
#include <stdint.h>
#include "inc/aquaboot.h"
#include "inc/fs/ext2.h"
#include "inc/memory_services.h"
#include "inc/print.h"
#include "inc/log.h"
#include "inc/disk_services.h"
#include "inc/acpi.h"
#include "inc/video_services.h"
#include "inc/fs/filesystem.h"
#include "inc/logo.h"
#include "inc/elf.h"
#include "inc/virtual_memory.h"

#define AQUABOOT_MAJOR 0
#define AQUABOOT_MINOR 1
#define AQUABOOT_PATCH 0

#define PT_ADDR_MASK ((uint64_t)0x0000fffffffff000)
#define pte_addr(pte) ((pte) & PT_ADDR_MASK)

EFI_SYSTEM_TABLE *sysT = NULL;
EFI_HANDLE imgH = NULL;

EFI_STATUS status;

BOOLEAN found_kernel = FALSE;

EFI_MEMORY_DESCRIPTOR *memory_map;

uint64_t map_key;

void *memcpy(void *dest, const void *src, size_t n) {
    asm volatile(
        "rep movsb"
        : "=D"(dest), "=S"(src), "=c"(n)
        : "D"(dest), "S"(src), "c"(n)
        : "memory"
    );
    return dest;
}

void hlt()
{
    asm volatile ( "hlt" );
}

void bpanic(void)
{
    sysT->ConOut->OutputString(sysT->ConOut, L"BOOTLOADER PANIC! ABORTING...");
    sysT->BootServices->Stall(10 * 1000 * 1000);
    
    sysT->BootServices->Exit(imgH, EFI_ABORTED, 0, NULL);
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    sysT = SystemTable;
    imgH = ImageHandle;

    aquaboot_info *boot_info;
    boot_info->aquaboot_major = AQUABOOT_MAJOR;
    boot_info->aquaboot_minor = AQUABOOT_MINOR;
    boot_info->aquaboot_patch = AQUABOOT_PATCH;

    // Clear the screen
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
    
    print(u"AquaBoot version %d.%d.%d\r\n",
                                            AQUABOOT_MAJOR,
                                            AQUABOOT_MINOR,
                                            AQUABOOT_PATCH);

    print(u"Initializing Serial Services...\r\n");

    init_serial_services();

    print(u"Initializing Disk Services...\r\n");
    
    init_disk_services();

    print(u"Initializing Memory Services...\r\n");

    print(u"Initializing FileSystem Services...\r\n");

    init_fs_services();

    // Extra feature: Check if Windows is installed (check EFI/Microsoft/* for bootmgr.efi), and allow user to boot to it if they want

    int kernel_inode_num = 0;
    struct ext2_inode *kernel_ino;
    struct elf_header *kernel_hdr;
    struct program_header *kernel_phdr;
    uint8_t block_buf[4096];

    found_kernel = read_filepath("/Aqua64/System/aquakernel.elf", sizeof("/Aqua64/System/aquakernel.elf"), &kernel_inode_num);

    if (found_kernel)
    {
        print(u"AquaOS kernel found! Inode: %d, Loading into memory...\r\n", kernel_inode_num);

        if(!is_elf(kernel_inode_num))
        {
            print(u"Kernel's ELF header could not be validated! May be corrupted. Aborting...\r\n");
        }

        uint64_t kernel_paddr = load_elf(kernel_inode_num);
        uint64_t kernel_vaddr = 0xffffffff80000000;
        bdebug(INFO, "Kernel loaded at 0x%x\r\n", kernel_paddr);

        print(u"Allocated Memory for kernel! Loading...\r\n");

        aquaboot_framebuffer *framebuffer;
        init_video_services(framebuffer);

        changeBackgroundColor(0x2b60de);
        display_logo();

        bdebug(INFO, "Setting up page tables...\r\n");

        pagemap_t pagemap;
        pagemap = new_pagemap();

        bdebug(INFO, "Identity Mapping...\r\n");

        map_pages(pagemap, 0x70000000, 0x70000000, 0x7, 0x10000000);
        map_pages(pagemap, kernel_vaddr, kernel_paddr, 0x7, 0x6000);

        changeBackgroundColor(0x000000);

        boot_info->framebuffer = framebuffer;

        get_memory_map(map_key);

        SystemTable->BootServices->ExitBootServices(ImageHandle, map_key);

        if (EFI_ERROR(status))
        {
            bdebug(ERROR, "Problem occured exiting boot services! Aborting...\r\n");
            bpanic();
        } else {bdebug(INFO, "Successfully exited boot services!\r\n");}

        extern void loadPageTables(uint64_t pml4);

        loadPageTables(pagemap.top_level);

        void (*kernel_main)(aquaboot_info *boot_info) = (void(*)(aquaboot_info *boot_info)) kernel_vaddr;

        kernel_main(boot_info);

        asm volatile("mov $1, %eax");   // For debugging purposes, tells us if we didn't jump to kernel entry
        for(;;);    // We can't call bpanic anymore, so just halt the system
    }

    // TODO: Check Boot Medium (eg. USB) for 

    else
    {
        print(u"AquaOS kernel could not be located! Filesystem may be corrupted! Aborting...\r\n");
        bpanic();
    }

    // Shouldn't ever reach here, but if we do, panic
    bpanic();
}