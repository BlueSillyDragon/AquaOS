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

    int kernel_inode_num = 0;
    struct ext2_inode *kernel_ino;
    struct elf_header *kernel_hdr;
    struct program_header *kernel_phdr;
    uint8_t block_buf[4096];

    found_kernel = read_filepath("/Aqua64/System/aquakernel.elf", sizeof("/Aqua64/System/aquakernel.elf"), &kernel_inode_num);

    if (found_kernel)
    {
        print(u"AquaOS kernel found! Inode: %d, Loading into memory...\r\n", kernel_inode_num);
        kernel_ino = read_inode(kernel_inode_num);
        read_block(kernel_ino->i_block[0], block_buf);
        kernel_hdr = &block_buf;

        if (kernel_hdr->magic_number == ELF_MAGIC_NUMBER)
        {
            bdebug(INFO, "Is ELF!\r\n");
        }

        if (kernel_hdr->arch == 2)
        {
            bdebug(INFO, "64bit!\r\n");
        }

        else
        {
            bdebug(ERROR, "AquaOS is a 64bit system, but kernel file is 32bit, something must've went wrong, aborting...\r\n");
            bpanic();
        }

        if (kernel_hdr->type == ELF_EXECUTABLE)
        {
            bdebug(INFO, "Kernel is executable!\r\n");
        }

        bdebug(INFO, "Number of program headers: %d\r\n", kernel_hdr->p_entry_num);
        bdebug(INFO, "Kernel entry offset: 0x%x\r\n", kernel_hdr->p_entry_offs);

        kernel_phdr = &block_buf[64];

        if (kernel_phdr->seg_type == 1)
        {
            bdebug(INFO, "This kernel segment is loadable! Allocating memory and loading...\r\n");
        }
        uint64_t kernel_addr;
        uefi_allocate_pages(1, &kernel_addr);
        bdebug(INFO, "Kernel located at 0x%x\r\n", (uint64_t *)kernel_addr);
        print(u"Allocated Memory for kernel! Loading...\r\n");

        kernel_ino = read_inode(kernel_inode_num);
        read_block(kernel_ino->i_block[1], block_buf);
        memcpy((uint64_t *)kernel_addr, &block_buf[kernel_phdr->offset], kernel_phdr->p_filesz);

        bdebug(NONE, "\r\n");

        aquaboot_framebuffer *framebuffer;
        init_video_services(framebuffer);

        changeBackgroundColor(0x2b60de);
        display_logo();

        bdebug(INFO, "Setting up page tables...\r\n");

        pagemap_t pagemap;
        pagemap = new_pagemap();

        bdebug(INFO, "Identity Mapping...\r\n");

        map_page(pagemap, 0xffffffff80000000, kernel_addr, 0x7);
        map_pages(pagemap, 0x70000000, 0x70000000, 0x7, 0x10000000);

        changeBackgroundColor(0x000000);

        get_memory_map(map_key);

        SystemTable->BootServices->ExitBootServices(ImageHandle, map_key);

        if (EFI_ERROR(status))
        {
            bdebug(ERROR, "Problem occured exiting boot services! Aborting...\r\n");
            bpanic();
        } else {bdebug(INFO, "Successfully exited boot services!\r\n");}

        extern void disablePaging(uint64_t pml4);
        extern void enablePaging(void);

        disablePaging(pagemap.top_level);
        enablePaging();

        void (*kernel_main)(void) = (void(*)(void)) kernel_hdr->p_entry_offs + 10;

        kernel_main();

        asm volatile("mov $1, %rax");
        for(;;);
    }

    else
    {
        print(u"AquaOS kernel could not be located! Filesystem may be corrupted! Aborting...\r\n");
        bpanic();
    }

    // Shouldn't ever reach here, but if we do, panic
    bpanic();
}