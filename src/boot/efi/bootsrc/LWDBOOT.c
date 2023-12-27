#include <efi.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    (void)ImageHandle;
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"LWD-OS Bootloader has loaded successfully!\r\n");
    for (;;);
}