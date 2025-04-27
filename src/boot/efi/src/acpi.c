#include <efi.h>
#include <stdint.h>
#include "efi/efiapi.h"
#include "efi/efidef.h"
#include "inc/globals.h"
#include "inc/log.h"
#include "inc/print.h"
#include "inc/acpi.h"

EFI_GUID acpi = ACPI_TABLE_GUID;
EFI_GUID acpi2 = ACPI_20_TABLE_GUID;

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

void retrieve_acpi_tables()
{
    // Check the ACPI version
    const void *acpi_p = &acpi2;
    const void *acpi2_p = &acpi2;
    void *vendorGuid = &sysT->ConfigurationTable[9].VendorGuid;

    bdebug(INFO, "Number of table entries %d\r\n", sysT->NumberOfTableEntries);

    for (uint64_t i = 0; i < sysT->NumberOfTableEntries; i++)
    {
        if (memcmp(vendorGuid, acpi_p, 128) == 0)
        {
            print(u"Found ACPI 1.0!\r\n");
        }

        else if (memcmp(vendorGuid, acpi2_p, 128) == 0)
        {
            print(u"Found ACPI 2.0!\r\n");
        }

        bdebug(INFO, "Vendor Guid 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\r\n",
            sysT->ConfigurationTable[i].VendorGuid.Data1,
            sysT->ConfigurationTable[i].VendorGuid.Data2,
            sysT->ConfigurationTable[i].VendorGuid.Data3,
            sysT->ConfigurationTable[i].VendorGuid.Data4[0],
            sysT->ConfigurationTable[i].VendorGuid.Data4[1],
            sysT->ConfigurationTable[i].VendorGuid.Data4[2],
            sysT->ConfigurationTable[i].VendorGuid.Data4[3],
            sysT->ConfigurationTable[i].VendorGuid.Data4[4],
            sysT->ConfigurationTable[i].VendorGuid.Data4[5],
            sysT->ConfigurationTable[i].VendorGuid.Data4[6],
            sysT->ConfigurationTable[i].VendorGuid.Data4[7]);
    }
}