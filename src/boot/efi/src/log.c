#include "inc/globals.h"
#include "inc/print.h"
#include "inc/log.h"
#include <stdarg.h>

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

void bdebug(enum DEBUG_TYPE type, char* string, ...)
{
    UINTN bufSize;

    switch (type)
    {
        case INFO:
            bufSize = 19;
            serial->Write(serial, &bufSize, "[ \033[32mInfo\033[0m ] ");
            break;
        case WARNING:
            bufSize = 22;
            serial->Write(serial, &bufSize, "[ \033[33mWarning\033[0m ] ");
            break;
        case ERROR:
            bufSize = 20;
            serial->Write(serial, &bufSize, "[ \033[31mError\033[0m ] ");
            break;
    }

    int int_to_print;
    int number[100];
    int j;
    char c;
    va_list args;

    va_start(args, string);

    for (int i = 0; string[i] != u'\0'; i++) {
        if (string[i] == u'%') {
            i++; // Get character after %

            switch (string[i]) {
                case 'c':
                    c = va_arg(args, int);
                    ser_putchar(c);
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
                        c = number[j] + '0';
                        ser_putchar(c);
                    }

                    continue;
                default:
                    break;
            }
        }

        else {
            ser_putchar(string[i]);
        }
    }

    va_end(args);
}