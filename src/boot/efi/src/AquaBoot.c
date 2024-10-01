#include <efi.h>
#include <stdarg.h>
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

void print(CHAR16 *fmt, ...) {
    CHAR16 c[2];
    int int_to_print;
    int number[100];
    int j;
    va_list args;

    c[0] = u'\0', c[1] = u'\0';

    va_start(args, fmt);

    for (int i = 0; fmt[i] != u'\0'; i++) {
        if (fmt[i] == u'%') {
            i++; // Get character after %

            switch (fmt[i]) {
                case 'c':
                    c[0] = va_arg(args, int);
                    sysT->ConOut->OutputString(sysT->ConOut, c);
                    break;
                case 'd':
                    int_to_print = va_arg(args, int);
                    j = 0;
                    do {
                        number[j] = (int_to_print % 10);
                        int_to_print = (int_to_print - int_to_print % 10) / 10;
                        j++;
                    }
                    while(int_to_print > 0);

                    j--;

                    for (; j>=0; j--) {
                        c[0] = number[j] + '0';
                        sysT->ConOut->OutputString(sysT->ConOut, c);
                    }

                    continue;
                default:
                    break;
            }
        }

        else {
            c[0] = fmt[i];
            sysT->ConOut->OutputString(sysT->ConOut, c);
        }
    }

    va_end(args);

}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
	(void)ImageHandle;
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
        bufferSize = sizeof(buffer);

        serial->Reset(serial);
        serial->Write(serial, &bufferSize, (void *)buffer);
    }

    // Detect Disks

    print(u"Detecting Block and Disk I/O Protocols...\r\n");
    
    EFI_GUID blockIoGuid = EFI_BLOCK_IO_PROTOCOL_GUID;
    EFI_BLOCK_IO_PROTOCOL *blockIo;
    EFI_GUID diskIoGuid = EFI_DISK_IO_PROTOCOL_GUID;
    EFI_DISK_IO_PROTOCOL *diskIo;

    status = SystemTable->BootServices->LocateProtocol(&blockIoGuid, NULL, (void**)&blockIo);

    if (EFI_ERROR(status)) {
        print(u"Unable to locate Block I/O Protocol!\r\n");
        hlt();
    }

    else {
        print(u"Block I/O Protocol located!\r\n");
    }

    status = SystemTable->BootServices->LocateProtocol(&diskIoGuid, NULL, (void**)&diskIo);

    if (EFI_ERROR(status)) {
        print(u"Unable to locate Disk I/O Protocol!\r\n");
        hlt();
    }

    else {
        print(u"Disk I/O Protocol located!\r\n");
    }

    BOOLEAN extendedVerification = 0;

    status = blockIo->Reset(blockIo, extendedVerification);

    if (EFI_ERROR(status)) {
        print(u"ERROR RESETTING BLOCK DEVICE!\r\n");
    }

    for(;;);
}