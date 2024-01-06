#pragma once

#include <efi.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#include "bootinc/acpi_tables.h"
#include "bootinc/bootgraphics.h"
#include "bootinc/print.h"

UINT32 BootloaderMajorVersion = (UINT32)1;
UINT32 BootloaderMinorVersion = (UINT32)0;