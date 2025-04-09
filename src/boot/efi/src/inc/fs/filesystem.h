#pragma once
#include "efi/x86_64/efibind.h"
#include "ext2.h"
#include <stdint.h>

// Reads from all Partitions, and checks byte 56 against EXT2_MAGIC_NUMBER. Stores all Ext2 Partition's Index in an array for later use
void init_fs_services();

// Reads 
void read_block(uint32_t block, void *buffer);

// Reads the inode idx
struct ext2_inode* read_inode(uint32_t inode);

// Recursively reads inodes until it gets to the last item in the filepath
int read_filepath(char *filepath, int filepath_size);