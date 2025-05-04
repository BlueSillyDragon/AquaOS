#include "inc/globals.h"
#include "inc/print.h"
#include "inc/log.h"
#include <stdarg.h>
#include <stdint.h>

EFI_SERIAL_IO_PROTOCOL *serial;

uint8_t serial_supported;

void init_serial_services()
{
    EFI_STATUS sta;
    EFI_GUID serialGuid = EFI_SERIAL_IO_PROTOCOL_GUID;

    sta = sysT->BootServices->LocateProtocol(&serialGuid, NULL, (void**)&serial);

    if (EFI_ERROR(sta)) {
        print(u"Unable to locate Serial I/O Protocol!\r\n");
        serial_supported = 0;
    } else serial_supported = 1;
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
    if (!serial_supported) {goto not_supported;} 

    UINTN bufSize;

    switch (type)
    {
        case INFO:
            bufSize = 19;
            serial->Write(serial, &bufSize, "[ \033[94mBoot\033[0m ] ");
            break;
        case WARNING:
            bufSize = 22;
            serial->Write(serial, &bufSize, "[ \033[33mWarning\033[0m ] ");
            break;
        case ERROR:
            bufSize = 20;
            serial->Write(serial, &bufSize, "[ \033[31mError\033[0m ] ");
            break;
        case NONE:
            break;
    }

    uint64_t int_to_print;
    uint64_t number[256];
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
                    int_to_print = va_arg(args, uint64_t);
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
                case 'x':
                    int_to_print = va_arg(args, uint64_t);
                    j = 0;
                    do {
                        number[j] = (int_to_print % 16);
                        int_to_print = (int_to_print - int_to_print % 16) / 16;
                        j++;
                    }
                    while(int_to_print > 0);

                    j--;

                    for (; j>=0; j--) {
                        if (number[j] > 0x9)
                        {
                            c = number[j] + ('0' + 7);
                        }

                        else
                        {
                            c = number[j] + '0';
                        }
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

    not_supported:
}