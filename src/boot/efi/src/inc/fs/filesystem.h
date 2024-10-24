#pragma once
#include <stdint.h>

// Reads from all Partitions, and checks byte 56 against EXT2_MAGIC_NUMBER. Stores all Ext2 Partition's Index in an array for later use
void init_fs_services();

// Reads the inode idx
void read_inode(uint64_t inode);