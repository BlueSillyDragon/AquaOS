#include <efi.h>
#include <stdarg.h>
#include "efi/efidef.h"
#include "inc/print.h"
#include "inc/log.h"
#include "inc/disk_services.h"
#include "inc/video_services.h"
#include "inc/fs/filesystem.h"
#include "inc/logo.h"

#define AQUABOOT_MAJOR 0
#define AQUABOOT_MINOR 1
#define AQUABOOT_PATCH 0

EFI_SYSTEM_TABLE *sysT = NULL;
EFI_HANDLE imgH = NULL;

BOOLEAN found_kernel = FALSE;

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

    print(u"Initializing FileSystem Services...\r\n");

    init_fs_services();

    found_kernel = read_filepath("/Aqua64/System/aquakernel.elf", sizeof("/Aqua64/System/aquakernel.elf"));

    if (found_kernel)
    {
        print(u"AquaOS kernel found! Loading into memory...\r\n");
    }

    else
    {
        print(u"AquaOS kernel could not be located! Filesystem may be corrupted! Aborting...\r\n");
    }

    for(;;);
}