#include "inc/globals.h"
#include "inc/disk_services.h"
#include "inc/disk_services.h"
#include "inc/log.h"
#include <stdint.h>

static struct disk disks[10];
static struct disk parts[10];

int disk_count = 0;
int part_count = 0;

void detect_disks()
{

    EFI_STATUS sta;

    EFI_GUID blockIoGuid = EFI_BLOCK_IO_PROTOCOL_GUID;
    EFI_GUID diskIoGuid = EFI_DISK_IO_PROTOCOL_GUID;

    EFI_HANDLE *handles = NULL;
    UINTN bufferSize = 0;

    sta = sysT->BootServices->LocateHandle(                       // First we call LocateProtocol with a buffer size of 0
        ByProtocol,                                                         // to figure out how much space we need to allocate for it
        &blockIoGuid, 
        NULL, 
        &bufferSize,
        handles);
    
    if (sta == EFI_BUFFER_TOO_SMALL)
    {
        sysT->BootServices->AllocatePool(EfiLoaderData, bufferSize, (void**)&handles);
    }

    sta = sysT->BootServices->LocateHandle(
        ByProtocol,
        &blockIoGuid, 
        NULL, 
        &bufferSize,
        handles);

    if (EFI_ERROR(sta))
    {
        bdebug(ERROR, "Could not obtain BlockIO handles!\r\n");
        for (;;);
    }

    for(int i = 0; (UINTN)i < bufferSize / sizeof(EFI_HANDLE); ++i)
    {
        EFI_BLOCK_IO_PROTOCOL *blockIo = NULL;
        EFI_DISK_IO_PROTOCOL *diskIo = NULL;
        struct disk *dsk;
        struct disk *prt;
        sta = sysT->BootServices->HandleProtocol(handles[i], &blockIoGuid, (void**)&blockIo);

        // Save the partition so we can probe it later
        if (blockIo->Media->LogicalPartition)
        {
            sta = sysT->BootServices->HandleProtocol(handles[i], &diskIoGuid, (void**)&diskIo);

            if (EFI_ERROR(sta))
            {
                bdebug(WARNING, "Partition %d does not support Disk I/O\r\n", part_count);
            }

            prt = &parts[part_count];
            prt->bio = blockIo;
            prt->dio = diskIo;
            prt->sectors = blockIo->Media->LastBlock + 1;
            prt->blkSize = blockIo->Media->BlockSize;
            prt->id = i;
            part_count++;
            continue;
        }

        else if (!blockIo->Media->MediaPresent)
        {
            continue;
        }

        else if (EFI_ERROR(sta))
        {
            bdebug(ERROR, "Error Obtaining Block I/O Protocol!\r\n");
            continue;
        }

        sta = sysT->BootServices->HandleProtocol(handles[i], &diskIoGuid, (void**)&diskIo);

        if (EFI_ERROR(sta))
        {
            bdebug(ERROR, "Error Obtaining Disk I/O Protocol!\r\n");
            continue;
        }

        dsk = &disks[i];
        dsk->bio = blockIo;
        dsk->dio = diskIo;
        dsk->sectors = blockIo->Media->LastBlock + 1;
        dsk->blkSize = blockIo->Media->BlockSize;
        dsk->id = i;
        disk_count++;
    }

    bdebug(INFO, "Number of disks is: %d\r\n", disk_count);
}

int get_disk_count()
{
    return disk_count;
}

int get_part_count()
{
    return part_count;
}

void read_disk(int idx, int offset, int bytes, void *buffer)
{
    EFI_STATUS sta;
    struct disk *dsk;

    dsk = &disks[idx];

    sta = dsk->dio->ReadDisk(dsk->dio, dsk->bio->Media->MediaId, offset, bytes, &buffer);

    if (EFI_ERROR(sta))
    {
        bdebug(ERROR, "Could not read from disk: %d\r\n", idx);
    }
}

void read_part(int idx, uint64_t offset, int bytes, void *buffer)
{
    EFI_STATUS sta;
    struct disk *prt;

    prt = &parts[idx];

    bdebug(INFO, "offset: %d\r\n", offset);
    sta = prt->dio->ReadDisk(prt->dio, prt->bio->Media->MediaId, offset, bytes, buffer);

    if (EFI_ERROR(sta))
    {
        bdebug(ERROR, "Could not read from partition: %d\r\n", idx);
    }
}

void init_disk_services(void)
{
    detect_disks();
}