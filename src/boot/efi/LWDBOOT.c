#include <efi.h>
#include <efilib.h>
 
EFI_STATUS 
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
   InitializeLib(ImageHandle, SystemTable);
 
    EFI_STATUS Status;
    EFI_INPUT_KEY Key;
 
    ST = SystemTable;
 
    Status = ST->ConOut->OutputString(ST->ConOut, L"LWD-OS Bootloader has started succesfully!\r\n");
    if (EFI_ERROR(Status)) {
        
        return Status;
    }      

    Status = ST->ConIn->Reset(ST->ConIn, FALSE);
    if (EFI_ERROR(Status)) {

        return Status;
    }
 
    while ((Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key)) == EFI_NOT_READY) ;
 
    return Status;
}
