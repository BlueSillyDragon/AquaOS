#include "inc/globals.h"
#include "inc/print.h"
#include "inc/log.h"

EFI_SERIAL_IO_PROTOCOL *serial;

void init_serial_services()
{
    EFI_STATUS sta;
    EFI_GUID serialGuid = EFI_SERIAL_IO_PROTOCOL_GUID;

    sta = sysT->BootServices->LocateProtocol(&serialGuid, NULL, (void**)&serial);

    if (EFI_ERROR(sta)) {
        print(u"Unable to locate Serial I/O Protocol!\r\n");
    }
}

void ser_putchar(char c)
{
    UINTN bufSize = 1;
    char buf[1];

    buf[0] = c;

    serial->Write(serial, &bufSize, (void *)buf);
}

void bdebug(enum DEBUG_TYPE type, char* string)
{
    for (int i = 0; string[i] != '\0'; i++)
    {
        ser_putchar(string[i]);
    }
}