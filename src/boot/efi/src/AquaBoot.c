#include <efi.h>
#include "inc/video_services.h"
#include "inc/logo.h"

#define AQUABOOT_MAJOR 0
#define AQUABOOT_MINOR 1
#define AQUABOOT_PATCH 0
#define AQUABOOT_BUILD 1

#define AQUABOOT_BG 0x2B60DE

void hlt() {
    asm volatile ( "hlt" );
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
	(void)ImageHandle;
    EFI_STATUS status;

    // Clear the screen
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"AquaBoot loaded!\r\n");
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Detecting Block and Disk I/O Protocols...\r\n");

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

    // Detect Disks
    
    EFI_GUID blockIoGuid = EFI_BLOCK_IO_PROTOCOL_GUID;
    EFI_BLOCK_IO_PROTOCOL *blockIo;
    EFI_GUID diskIoGuid = EFI_DISK_IO_PROTOCOL_GUID;
    EFI_DISK_IO_PROTOCOL *diskIo;

    status = SystemTable->BootServices->LocateProtocol(&blockIoGuid, NULL, (void**)&blockIo);

    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to locate Block I/O Protocol!\n\r");
        hlt();
    }

    else {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Block I/O Protocol located!\n\r");
    }

    status = SystemTable->BootServices->LocateProtocol(&diskIoGuid, NULL, (void**)&diskIo);

    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to locate Disk I/O Protocol!\n\r");
        hlt();
    }

    else {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Disk I/O Protocol located!\n\r");
    }

    BOOLEAN extendedVerification = 0;

    status = blockIo->Reset(blockIo, extendedVerification);

    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ERROR RESETTING BLOCK DEVICE!\n\r");
    }

    for(;;);
}