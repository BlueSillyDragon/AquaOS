#include "inc/globals.h"
#include "inc/fs/ext2.h"
#include "inc/fs/filesystem.h"
#include "inc/disk_services.h"
#include "inc/log.h"
#include "inc/print.h"
#include <stdint.h>

uint8_t ext2_parts[10];
uint8_t ext2_count;

int block_size;

uint64_t bgdt_start;
uint64_t inode_size;

int inodes_per_group;

struct ext2_superblock *sb;

void init_fs_services()
{
    uint8_t buf[1024];
    int prt_cnt;

    prt_cnt = get_part_count();

    for (int i = 0; i < prt_cnt; ++i)
    {
        read_part(i, 1024, 1024, &buf);

        sb = &buf;

        if (sb->s_magic == EXT2_MAGIC_NUMBER)
        {
            bdebug(INFO, "Part: %d is Ext2 Filesystem!\r\n", i);
            bdebug(INFO, "Revision %d.%d\r\n", sb->s_rev_level, sb->s_minor_rev_level);

            bdebug(INFO, "First Data Block: %d\r\n", sb->s_first_data_block);


            block_size = (1024 << sb->s_log_block_size);

            switch (block_size)
            {
            case BS_1KIB:
                bdebug(INFO, "Using 1KIB Blocks\r\n");
                break;
            case BS_2KIB:
                bdebug(INFO, "Using 2KIB Blocks\r\n");
                break;
            case BS_4KIB:
                bdebug(INFO, "Using 4KIB Blocks\r\n");
                break;
            case BS_8KIB:
                bdebug(INFO, "Using 8KIB Blocks\r\n");
                break;
            
            default:
                break;
            }

            inodes_per_group = sb->s_inodes_per_group;

            inode_size = sb->s_inode_size;

            bgdt_start = block_size >= BS_2KIB ? block_size : block_size * 2;

            ext2_parts[ext2_count] = i;
            ext2_count++;
        }

        else
        {
            bdebug(INFO, "Part: %d is not an Ext2 Partition.\r\n", i);
        }
    }

    // TODO: Check if we're booting off of installation media (e.g. USB), in which case the kernel will be located on the current media.
    if (ext2_count < 1)
    {
        print(u"The AquaOS Kernel must be located on an Ext2 Partition!\r\n");
        for(;;);
    }
}

void read_block(uint64_t block, uint8_t *buffer)
{
    uint8_t buf[block_size];

    int offset = block_size * block;

    read_part(ext2_parts[0], offset, block_size, &buf);
}

void read_inode(uint64_t inode)
{
    bdebug(INFO, "Inodes per group: %d\r\n", inodes_per_group);

    uint64_t block_group = (inode - 1) / inodes_per_group;
    uint64_t local_i_idx = (inode - 1) % inodes_per_group;

    bdebug(INFO, "Block Group Descriptor Start: %d\r\n", bgdt_start);

    bdebug(INFO, "Inode Number: %d, Block Group Number: %d, Local Inode Index: %d\r\n", inode, block_group, local_i_idx);

    uint8_t bgdt_buf[32];

    struct ext2_bgdt *bgdt;

    read_part(ext2_parts[0], bgdt_start + (sizeof(struct ext2_bgdt) * block_group), 32, &bgdt_buf);

    bgdt = &bgdt_buf;

    bdebug(INFO, "Directories in Inode's block group: %d\r\n", bgdt->bg_used_dirs_count);
    bdebug(INFO, "Starting block of Inode Table: %d\r\n", bgdt->bg_inode_table);
    bdebug(INFO, "Inode size: %d\r\n", inode_size);

    uint8_t ino_buf[inode_size];

    read_part(ext2_parts[0], (bgdt->bg_inode_table * block_size) + (local_i_idx * inode_size), 32, &ino_buf);

    struct ext2_inode *ino;

    ino = &ino_buf;

    if ((ino->i_mode & 0xF000) == EXT2_S_IFDIR)
    {
        bdebug(INFO, "Is directory!\r\n");
    }

    bdebug(INFO, "Mode is: %d\r\n", ino->i_mode);
}