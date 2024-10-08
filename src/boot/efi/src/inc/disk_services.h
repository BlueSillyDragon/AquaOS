#pragma once

#include "efi.h"

struct disk {
    uint64_t sectors;
    uint32_t id;
    uint16_t blkSize;
    EFI_BLOCK_IO_PROTOCOL *bio;
    EFI_DISK_IO_PROTOCOL *dio;
};

// Enumerates through disks (as well as partitions) and stores them in an array for later use
void init_disk_services(void);

// Returns the disk count 
int get_disk_count();

// Returns the partition count 
int get_part_count();

// Reads from disk defined by idx, at offset, with the size in bytes, and returns the bytes read in buffer
void read_disk(int idx, int offset, int bytes, void *buffer);

// Reads from partition defined by idx, at offset, with the size in bytes, and returns the bytes read in buffer
void read_part(int idx, int offset, int bytes, void *buffer);