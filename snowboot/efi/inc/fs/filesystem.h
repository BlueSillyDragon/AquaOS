#pragma once
#include "efi/x86_64/efibind.h"
#include "ext2.h"
#include <stdint.h>

// Reads from all Partitions, and checks byte 56 against EXT2_MAGIC_NUMBER. Stores all Ext2 Partition's Index in an array for later use
void initFsServices();

// Reads 
void readBlock(uint64_t block, void *buffer);

// Reads the inode idx
struct ext2_inode* readInode(uint32_t inode);

// Recursively reads inodes until it gets to the last item in the filepath. Returns the index of the last inode in inode_id
int readFilepath(char *filepath, uint64_t filepath_size, int *inode_id);