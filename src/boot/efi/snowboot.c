#include <efi.h>
#include <stdarg.h>
#include <stdint.h>
#include "inc/boot_protocol/snowboot.h"
#include "efi/efidef.h"
#include "inc/memory_services.h"
#include "inc/print.h"
#include "inc/log.h"
#include "inc/disk_services.h"
#include "inc/video_services.h"
#include "inc/fs/filesystem.h"
#include "inc/elf.h"
#include "inc/virtual_memory.h"

#define SNOWBOOT_MAJOR 0
#define SNOWBOOT_MINOR 1
#define SNOWBOOT_PATCH 0

EFI_SYSTEM_TABLE *sysT = NULL;
EFI_HANDLE imgH = NULL;

EFI_STATUS status;

BOOLEAN foundKernel = FALSE;

EFI_MEMORY_DESCRIPTOR *memoryMap;

uint64_t mapKey;

uint64_t hhdmOffset = 0xFFFF800000000000;

void hlt()
{
    asm volatile ( "hlt" );
}

void bpanic(void) {
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
    
    print(u"SnowBoot version %d.%d.%d\r\n",
                                            SNOWBOOT_MAJOR,
                                            SNOWBOOT_MINOR,
                                            SNOWBOOT_PATCH);

    print(u"Initializing Serial Services...\r\n");

    initSerialServices();

    print(u"Initializing Disk Services...\r\n");
    
    initDiskServices();

    print(u"Initializing Memory Services...\r\n");

    print(u"Initializing FileSystem Services...\r\n");

    initFsServices();

    // Extra feature: Check if Windows is installed (check EFI/Microsoft/* for bootmgr.efi), and allow user to boot to it if they want

    int kernelInodeNum = 0;

    foundKernel = readFilepath("/Snow64/System/yuki.elf", sizeof("/Snow64/System/yuki.elf"), &kernelInodeNum);

    if (foundKernel)
    {
        print(u"SnowOS kernel found! Loading into memory...\r\n", kernelInodeNum);

        if(!isElf(kernelInodeNum))
        {
            print(u"Kernel's ELF header could not be validated! May be corrupted. Aborting...\r\n");
            bpanic();
        }

        uint64_t kernelLoadOffset;
        uint64_t kernelPaddr = loadElf(kernelInodeNum, &kernelLoadOffset);
        uint64_t kernelVaddr = 0xffffffff80000000;
        bdebug(INFO, "Kernel loaded at 0x%x\r\n", kernelPaddr);

        print(u"Allocated Memory for kernel! Loading...\r\n");

        snowboot_info bootInfo;
        uefiAllocatePages(1, &bootInfo, EfiReservedMemoryType);
        bootInfo.snowbootMajor = SNOWBOOT_MAJOR;
        bootInfo.snowbootMinor = SNOWBOOT_MINOR;
        bootInfo.snowbootPatch = SNOWBOOT_PATCH;

        bootInfo.hhdm = hhdmOffset;

        bootInfo.kernelPaddr = kernelPaddr;

        snowboot_framebuffer *framebuffer;
        framebuffer = initVideoServices();

        bootInfo.framebuffer = framebuffer;

        bdebug(INFO, "Setting up page tables...\r\n");

        pagemap_t pagemap;
        pagemap = newPagemap();

        bdebug(INFO, "Mapping Pages...\r\n");

        mapPages(pagemap, hhdmOffset, 0x0, 0x3, 0x100000000);
        mapPages(pagemap, (pagemap.topLevel & ~0xfffffff), (pagemap.topLevel & ~0xfffffff), 0x3, 0x10000000);    // Insure where the page tables are is identity mapped
        mapPages(pagemap, kernelVaddr, kernelPaddr, 0x3, 0x100000);

        bdebug(INFO, "Page maps located around 0x%x\r\n", (pagemap.topLevel & ~0xfffffff));

        memoryMap = getMemoryMap(pagemap);

        bootInfo.memoryMap = memoryMap;

        mapKey = getMapKey();

        bootInfo.memMapEntries = getEntryCount();
        bootInfo.descSize = getDescSize();

        setMemoryTypes(bootInfo.memoryMap);

        status = SystemTable->BootServices->ExitBootServices(imgH, mapKey);

        if (EFI_ERROR(status))
        {
            bdebug(ERROR, "Problem occured exiting boot services! Aborting...\r\n");
            bpanic();
        }

        extern void loadPageTables(uint64_t pml4);

        loadPageTables((pagemap.topLevel));

        void (*kernelMain)(snowboot_info*) = (void(*)(snowboot_info*)) kernelLoadOffset;

        kernelMain(&bootInfo);

        hlt();    // We can't call bpanic anymore, so just halt the system
    }

    // TODO: Check Boot Medium (eg. USB) for kernel

    else
    {
        print(u"SnowOS kernel could not be located! Filesystem may be corrupted! Aborting...\r\n");
        bpanic();
    }

    // Shouldn't ever reach here, but if we do, panic
    bpanic();
}