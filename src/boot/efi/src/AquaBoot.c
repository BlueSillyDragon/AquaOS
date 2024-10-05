#include <efi.h>
#include <stdarg.h>
#include "inc/print.h"
#include "inc/disk_services.h"
#include "inc/video_services.h"
#include "inc/logo.h"

#define AQUABOOT_MAJOR 0
#define AQUABOOT_MINOR 1
#define AQUABOOT_PATCH 0

#define AQUABOOT_BG 0x2B60DE

EFI_SYSTEM_TABLE *sysT = NULL;
EFI_HANDLE imgH = NULL;

void hlt() {
    asm volatile ( "hlt" );
}

void bpanic(void) {

    sysT->ConOut->OutputString(sysT->ConOut, L"BOOTLOADER PANIC! ABORTING...");
    sysT->BootServices->Stall(10 * 1000 * 1000);
    
    sysT->BootServices->Exit(imgH, EFI_ABORTED, 0, NULL);
}

void bdebug(EFI_SERIAL_IO_PROTOCOL* ser, char* str) {
    UINTN strBuf = sizeof(str);
    ser->Write(ser, &strBuf, (void *)str);
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
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

    // Setup Serial I/O
    EFI_GUID serialGuid = EFI_SERIAL_IO_PROTOCOL_GUID;
    EFI_SERIAL_IO_PROTOCOL *serial;

    status = SystemTable->BootServices->LocateProtocol(&serialGuid, NULL, (void**)&serial);

    if (EFI_ERROR(status)) {
        print(u"Unable to locate Serial I/O Protocol!\r\n");
    }

    else {
        print(u"Serial I/O Protocol located!\r\n");
        UINTN bufferSize;
        char buffer[] = "[Bootloader] \033[34mThis is some blue text!\033[0m\r\n";
        bufferSize = sizeof(buffer) / sizeof(char);

        serial->Reset(serial);
        serial->Write(serial, &bufferSize, (void *)buffer);
    }

    // Detect Disks

    print(u"Detecting Disks...\r\n");
    
    init_disk_services();

    for(;;);
}