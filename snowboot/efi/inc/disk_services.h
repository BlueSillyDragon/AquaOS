#pragma once

#include "efi.h"
#include <stdint.h>

struct disk {
    uint64_t sectors;
    uint32_t id;
    uint16_t blkSize;
    EFI_BLOCK_IO_PROTOCOL *bio;
    EFI_DISK_IO_PROTOCOL *dio;
};

// Enumerates through disks (as well as partitions) and stores them in an array for later use
void initDiskServices(void);

// Returns the disk count 
int getDiskCount();

// Returns the partition count 
int getPartCount();

// Reads from disk defined by idx, at offset, with the size in bytes, and returns the bytes read in buffer
void readDisk(int idx, int offset, int bytes, void *buffer);

// Reads from partition defined by idx, at offset, with the size in bytes, and returns the bytes read in buffer
void readPart(int idx, uint64_t offset, int bytes, void *buffer);