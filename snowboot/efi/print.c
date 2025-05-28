#include "inc/print.h"
#include "inc/globals.h"
#include <stdarg.h>

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


void printEfiStatus(EFI_STATUS sta) {
    switch (sta) {
        case EFI_SUCCESS:
            print(u"EFI_SUCCESS\r\n");
            break;
        case EFI_INVALID_PARAMETER:
            print(u"EFI_INVALID_PARAMETER\r\n");
            break;
        case EFI_NOT_FOUND:
            print(u"EFI_NOT_FOUND\r\n");
            break;
        default:
            print(u"Not implemented!");
            break;
    }   
}