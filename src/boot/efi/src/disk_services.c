#include "inc/globals.h"
#include "inc/disk_services.h"
#include "inc/print.h"

void detect_disks() {

    EFI_STATUS sta;

    EFI_GUID blockIoGuid = EFI_BLOCK_IO_PROTOCOL_GUID;
    EFI_BLOCK_IO_PROTOCOL *blockIo;
    EFI_GUID diskIoGuid = EFI_DISK_IO_PROTOCOL_GUID;
    EFI_DISK_IO_PROTOCOL *diskIo;

    EFI_HANDLE *handles = NULL;
    UINTN bufferSize = 0;
    int dskCnt = 0;
    int part_count = 0;

    sta = sysT->BootServices->LocateHandle(                       // First we call LocateProtocol with a buffer size of 0
        ByProtocol,                                                         // to figure out how much space we need to allocate for it
        &blockIoGuid, 
        NULL, 
        &bufferSize,
        handles);
    
    if (sta == EFI_BUFFER_TOO_SMALL) {
        print(u"Buffer size needed: %d\r\n", bufferSize);

        sysT->BootServices->AllocatePool(EfiLoaderData, bufferSize, (void**)&handles);
    }

    sta = sysT->BootServices->LocateHandle(
        ByProtocol,
        &blockIoGuid, 
        NULL, 
        &bufferSize,
        handles);

    if (EFI_ERROR(sta)) {
        print(u"Could not obtain BlockIO handles!\r\n");

        if (sta == EFI_INVALID_PARAMETER) {
            print(u"Invalid Paramter!\r\n");
        }

        else if (sta == EFI_NOT_FOUND) {
            print(u"No handles matching search!\r\n");
        }
    }

    static struct disk *disks;
    static struct disk *parts;

    for(int i = 0; (UINTN)i < bufferSize / sizeof(EFI_HANDLE); ++i) {
        struct disk *dsk;
        struct disk *prt;
        sta = sysT->BootServices->HandleProtocol(handles[i], &blockIoGuid, (void**)&blockIo);

        // Save the partition so we can probe it later
        if (blockIo->Media->LogicalPartition) {
            print(u"Is Logical Partition.\r\n");
            prt = &parts[part_count];
            prt->bio = blockIo;
            prt->sectors = blockIo->Media->LastBlock + 1;
            prt->blkSize = blockIo->Media->BlockSize;
            prt->id = i;
            part_count++;
            continue;
        }

        else if (!blockIo->Media->MediaPresent) {
            print(u"Media not present.\r\n");
            continue;
        }

        else if (EFI_ERROR(sta)) {
            print(u"Nope!\r\n");
            continue;
        }

        else if (blockIo->Media->RemovableMedia) {
            print(u"Is removable media.\r\n");
        }

        dsk = &disks[dskCnt];
        dsk->bio = blockIo;
        dsk->sectors = blockIo->Media->LastBlock + 1;
        dsk->blkSize = blockIo->Media->BlockSize;
        dsk->id = i;

        print(u"Disk detected!\r\n");
        dskCnt++;
    }

    print (u"Number of disks is: %d\r\n", dskCnt);
}

void init_disk_services(void) {
    detect_disks();
}