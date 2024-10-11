#include <efi.h>
#include <stdarg.h>
#include "inc/print.h"
#include "inc/log.h"
#include "inc/disk_services.h"
#include "inc/video_services.h"
#include "inc/fs/filesystem.h"
#include "inc/logo.h"

#define AQUABOOT_MAJOR 0
#define AQUABOOT_MINOR 1
#define AQUABOOT_PATCH 0

#define AQUABOOT_BG 0x2B60DE

EFI_SYSTEM_TABLE *sysT = NULL;
EFI_HANDLE imgH = NULL;

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
    EFI_STATUS status;

    sysT = SystemTable;
    imgH = ImageHandle;

    // Clear the screen
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    print(u"AquaBoot loaded!\r\n");
    print(u"AquaBoot version %d.%d.%d\r\n",
                                            AQUABOOT_MAJOR,
                                            AQUABOOT_MINOR,
                                            AQUABOOT_PATCH);
    print(u"Detecting Serial I/O Protocol...\r\n");

    //AQUABOOT_FRAMEBUFFER framebuffer;

    //framebuffer = initGop(SystemTable);

    //changeBackgroundColor(framebuffer, AQUABOOT_BG);

    // Display fancy logo =)

    //int k = 0;

    //for (int i = 0; i < AQUAOS_LOGO_HEIGHT; i++) {
    //    for (int j = 0; j < AQUAOS_LOGO_WIDTH; j++) {

    //        if (aquaos_logo[k] == 0x000000) {
    //            plotPixels(i + (framebuffer.verticalRes / 4), j + (framebuffer.horizontalRes / 4), AQUABOOT_BG, framebuffer);
    //        }

    //        else {
    //            plotPixels(i + (framebuffer.verticalRes / 4), j + (framebuffer.horizontalRes / 4), aquaos_logo[k], framebuffer);
    //        }
    //        k++;
    //    }
    //}

    print(u"Initializing Disk Services...\r\n");
    
    init_disk_services();

    print(u"Initializing FileSystem Services...\r\n");

    init_fs_services();

    print(u"Initializing Serial Services...\r\n");

    init_serial_services();

    bdebug(INFO, "[Bootloader] \033[34mThis is some blue text!\033[0m\r\n");

    read_inode(12);

    for(;;);
}