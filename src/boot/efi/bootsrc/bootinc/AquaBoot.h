#pragma once

#include <efi.h>
#include <cstdint>
#include <cstdarg>
#include <cstddef>

#include "bootinc/acpi_tables.h"
#include "bootinc/bootgraphics.h"
#include "bootinc/print.h"
#include "bootinc/vmm.h"
#include "bootinc/pmm.h"

UINT32 BootloaderMajorVersion = (UINT32)1;
UINT32 BootloaderMinorVersion = (UINT32)0;

void statusError(EFI_STATUS s);
bool compareRSDP(EFI_GUID *vendGuid, EFI_GUID *rsdp);