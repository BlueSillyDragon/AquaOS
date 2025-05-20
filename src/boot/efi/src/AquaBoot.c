#include <efi.h>
#include <stdarg.h>
#include <stdint.h>
#include "inc/boot_protocol/aquaboot.h"
#include "efi/efidef.h"
#include "efi/efishell.h"
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

EFI_SYSTEM_TABLE *sysT = NULL;
EFI_HANDLE imgH = NULL;

EFI_STATUS status;

BOOLEAN found_kernel = FALSE;

EFI_MEMORY_DESCRIPTOR *memory_map;

uint64_t map_key;

uint64_t hhdm_offset = 0xFFFF800000000000;

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

    found_kernel = read_filepath("/Aqua64/System/aquakernel.elf", sizeof("/Aqua64/System/aquakernel.elf"), &kernel_inode_num);

    if (found_kernel)
    {
        print(u"AquaOS kernel found! Loading into memory...\r\n", kernel_inode_num);

        if(!is_elf(kernel_inode_num))
        {
            print(u"Kernel's ELF header could not be validated! May be corrupted. Aborting...\r\n");
            bpanic();
        }

        uint64_t kernel_load_offs;
        uint64_t kernel_paddr = load_elf(kernel_inode_num, &kernel_load_offs);
        uint64_t kernel_vaddr = 0xffffffff80000000;
        bdebug(INFO, "Kernel loaded at 0x%x\r\n", kernel_paddr);

        print(u"Allocated Memory for kernel! Loading...\r\n");

        aquaboot_info boot_info;
        uefi_allocate_pages(1, &boot_info, EfiReservedMemoryType);
        boot_info.aquaboot_major = AQUABOOT_MAJOR;
        boot_info.aquaboot_minor = AQUABOOT_MINOR;
        boot_info.aquaboot_patch = AQUABOOT_PATCH;

        boot_info.hhdm = hhdm_offset;

        boot_info.kernel_paddr = kernel_paddr;

        aquaboot_framebuffer *framebuffer;
        framebuffer = init_video_services();

        boot_info.framebuffer = framebuffer;

        bdebug(INFO, "Setting up page tables...\r\n");

        pagemap_t pagemap;
        pagemap = new_pagemap();

        bdebug(INFO, "Mapping Pages...\r\n");

        map_pages(pagemap, hhdm_offset, 0x0, 0x3, 0x100000000);
        map_pages(pagemap, (pagemap.top_level & ~0xfffffff), (pagemap.top_level & ~0xfffffff), 0x3, 0x10000000);    // Insure where the page tables are is identity mapped
        map_pages(pagemap, kernel_vaddr, kernel_paddr, 0x3, 0x100000);

        bdebug(INFO, "Page maps located around 0x%x\r\n", (pagemap.top_level & ~0xfffffff));

        memory_map = get_memory_map(pagemap);

        boot_info.memory_map = memory_map;

        map_key = get_map_key();

        boot_info.mem_map_entries = get_entry_count();
        boot_info.desc_size = get_desc_size();

        set_memory_types(boot_info.memory_map);

        status = SystemTable->BootServices->ExitBootServices(imgH, map_key);

        if (EFI_ERROR(status))
        {
            bdebug(ERROR, "Problem occured exiting boot services! Aborting...\r\n");
            bpanic();
        }

        extern void loadPageTables(uint64_t pml4);

        loadPageTables((pagemap.top_level));

        void (*kernel_main)(aquaboot_info*) = (void(*)(aquaboot_info*)) kernel_load_offs;

        kernel_main(&boot_info);

        hlt();    // We can't call bpanic anymore, so just halt the system
    }

    // TODO: Check Boot Medium (eg. USB) for kernel

    else
    {
        print(u"AquaOS kernel could not be located! Filesystem may be corrupted! Aborting...\r\n");
        bpanic();
    }

    // Shouldn't ever reach here, but if we do, panic
    bpanic();
}