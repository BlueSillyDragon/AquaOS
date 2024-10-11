#include <stdint.h>
#include "inc/globals.h"
#include "inc/fs/ext2.h"
#include "inc/fs/filesystem.h"
#include "inc/disk_services.h"
#include "inc/print.h"
#include "inc/log.h"

uint8_t ext2_parts[10];
uint8_t ext2_count;

int block_size;

struct ext2_superblock *sb;

void init_fs_services()
{
    UINT8 buf[1024];
    int prt_cnt;

    prt_cnt = get_part_count();

    for (int i = 0; i < prt_cnt; ++i)
    {
        read_part(i, 1024, 1024, &buf);

        sb = &buf;

        if (sb->s_magic == EXT2_MAGIC_NUMBER)
        {
            print(u"Part: %d is Ext2 Filesystem!\r\n", i);
            print(u"Inodes per group: %d\r\n", sb->s_inodes_per_group);

            ext2_parts[ext2_count] = i;
            ext2_count++;
        }

        else
        {
            print(u"Part: %d is not an Ext2 Partition.\r\n", i);
        }
    }

    block_size = (1024 << sb->s_log_block_size);

    switch (block_size)
    {
    case BS_1KIB:
        print(u"Using 1KIB Blocks\r\n");
        break;
    case BS_2KIB:
        print(u"Using 2KIB Blocks\r\n");
        break;
    case BS_4KIB:
        print(u"Using 4KIB Blocks\r\n");
        break;
    case BS_8KIB:
        print(u"Using 8KIB Blocks\r\n");
        break;
    
    default:
        break;
    }
}

void read_inode(uint64_t inode)
{
    print(u"Inodes per group: %d\r\n", sb->s_inodes_per_group);

    const uint64_t block_group = (inode - 1) / sb->s_inodes_per_group;
    const uint64_t local_i_idx = (inode - 1) % sb->s_inodes_per_group;

    print(u"Inode Number: %d\r\nBlock Group Number: %d\r\nLocal Inode Index: %d\r\n", inode, block_group, local_i_idx);

    bdebug(INFO, sb->s_volume_name);
}