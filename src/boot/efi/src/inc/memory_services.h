#pragma once
#include <efi.h>

void uefi_allocate_pool(UINTN size, void **buffer);