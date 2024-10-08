#pragma once

// Reads from all Partitions, and checks byte 56 against EXT2_MAGIC_NUMBER. Stores all Ext2 Partition's Index in an array for later use
void init_fs_services();

// Probes all Partitions for Ext2 magic number, and returns number of Ext2 Filesystems in out
void num_of_ext2(int prt_cnt, int out);