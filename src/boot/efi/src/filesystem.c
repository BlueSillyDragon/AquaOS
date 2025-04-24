#include <efi.h>
#include "efi/efidef.h"
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

int needed_block_id = 0;

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

void read_block(uint32_t block, void *buffer)
{
    int offset = block_size * block;

    read_part(ext2_parts[0], offset, block_size, buffer);

    bdebug(INFO, "Finished block read!\r\n");
}

struct ext2_inode* read_inode(uint32_t inode)
{
    bdebug(INFO, "Inodes per group: %d\r\n", inodes_per_group);

    uint64_t block_group = (inode - 1) / inodes_per_group;
    uint64_t local_i_idx = (inode - 1) % inodes_per_group;

    bdebug(INFO, "Block Group Descriptor Start: %d\r\n", bgdt_start);

    bdebug(INFO, "Inode Number: %d, Block Group Number: %d, Local Inode Index: %d\r\n", inode, block_group, local_i_idx);

    uint8_t bgdt_buf[32];

    struct ext2_bgdt *bgdt;

    bdebug(INFO, "Size of BGDT: %d\r\n", sizeof(struct ext2_bgdt));

    read_part(ext2_parts[0], bgdt_start + (sizeof(struct ext2_bgdt) * block_group), 32, bgdt_buf);

    bgdt = &bgdt_buf;

    bdebug(INFO, "Directories in Inode's block group: %d\r\n", bgdt->bg_used_dirs_count);
    bdebug(INFO, "Starting block of Inode Table: %d\r\n", bgdt->bg_inode_table);
    bdebug(INFO, "Inode size: %d\r\n", inode_size);

    uint8_t ino_buf[inode_size];

    read_part(ext2_parts[0], (bgdt->bg_inode_table * block_size) + (local_i_idx * inode_size), inode_size, ino_buf);

    struct ext2_inode *ino;

    ino = &ino_buf;

    if ((ino->i_mode & 0xF000) == EXT2_S_IFDIR)
    {
        bdebug(INFO, "Is directory!\r\n");
    }

    bdebug(INFO, "Blocks used by inode: %d\r\n", ino->i_blocks);
    bdebug(INFO, "Block 0 ID: %d\r\n", ino->i_block[0]);

    bdebug(INFO, "Mode is: 0x%x\r\n", ino->i_mode);

    return ino;
}

// TODO: Clean this function up at some point

int read_filepath(char *filepath, int filepath_size, int *inode_id)
{
    struct ext2_inode *current_inode;
    struct ext2_dir_entry *current_entry;

    int next_name_offset = 1; // Accounting for the fact that a filepath starts with /

    BOOLEAN same_char = FALSE; // Used during the recursive lookup
    BOOLEAN still_looking = TRUE;

    // First thing we need to do is read the root inode (which is always inode 2)

    if (filepath[0] != '/')
    {
        bdebug(ERROR, "AquaOS filepaths must start with '/'!\r\n");
        bdebug(INFO, "first char of filepath is: %c\r\n", filepath[0]);
        return FALSE;
    }

    current_inode = read_inode(EXT2_ROOT_INO);

    if ((current_inode->i_mode & 0xF000) == EXT2_S_IFDIR)
    {
        bdebug(INFO, "Is directory! Continuing recursive lookup...\r\n");
    }

    uint8_t block_buf[block_size];

    bdebug(INFO, "current inode block 0 id: %d\r\n", current_inode->i_block[0]);

    bdebug(NONE, "\r\n");

    // Start looking into directories

    char next_dir_name[255];

    while(still_looking)
    {
        bdebug(INFO, "%d\r\n", current_inode->i_block[0]);
        read_block(current_inode->i_block[0], block_buf);

        bdebug(INFO, "%d\r\n", filepath_size);

        for(int i = 0; i < block_size; i++)
        {
            bdebug(NONE, "%c", block_buf[i]);
        }

        bdebug(NONE, "\r\n");

        for (int i = 0; i < filepath_size; i++)
        {
            if(filepath[next_name_offset] == '/')
            {
                next_name_offset++;
                bdebug(INFO, "/ found, exiting...\r\n");
                break;
            }

            if (next_name_offset == filepath_size)
            {
                still_looking = FALSE;
                break;
            }

            next_dir_name[i] = filepath[next_name_offset];
            next_name_offset++;
            bdebug(NONE, "%c", next_dir_name[i]);
        }

        bdebug(NONE, "\r\n");

        int name_idx = 0;
        int name_offset = 0; // Need this to calculate the starting byte of the directory entry

        for(int i = 0; i < block_size; i++)
        {
            if(block_buf[i] == next_dir_name[name_idx])
            {
                same_char = TRUE;
                name_idx++;

                if(name_idx == 256)
                {
                    name_offset = i;
                    bdebug(INFO, "Name offset: %d\r\n", name_offset);
                    break;
                }
            }

            else
            {
                same_char = FALSE;
                name_idx = 0;
            }
        }

        if (same_char)
        {
            bdebug(INFO, "Found!\r\n");
            current_entry = &block_buf[name_offset - 263];
            bdebug(INFO, "Current Entry inode: %d\r\n", current_entry->inode);
            if (next_name_offset == filepath_size)
            {
                *inode_id = current_entry->inode;
                return TRUE;
            }
        }

        else
        {
            bdebug(INFO, "Not found! Ending recursive lookup...\r\n");
            return FALSE;
        }

        current_inode = read_inode(current_entry->inode);
        
    }

}