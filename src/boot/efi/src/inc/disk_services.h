#pragma once

#include "efi.h"

struct disk {
    uint64_t sectors;
    uint32_t id;
    uint16_t blkSize;
    EFI_BLOCK_IO_PROTOCOL *bio;
    EFI_DISK_IO_PROTOCOL *dio;
};

void init_disk_services(void);