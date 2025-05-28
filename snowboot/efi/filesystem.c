#include <efi.h>
#include "efi/efidef.h"
#include "inc/globals.h"
#include "inc/fs/ext2.h"
#include "inc/fs/filesystem.h"
#include "inc/disk_services.h"
#include "inc/memory_services.h"
#include "inc/log.h"
#include "inc/print.h"
#include <stdint.h>

uint8_t ext2Parts[10];
uint8_t ext2Count;

int blockSize;

uint64_t bgdtStart;
uint64_t inodeSize;

int inodesPerGroup;

struct ext2_superblock *sb;

int neededBlockId = 0;

void initFsServices()
{
    uint8_t buf[1024];
    int prtCnt;

    prtCnt = getPartCount();

    for (int i = 0; i < prtCnt; ++i)
    {
        readPart(i, 1024, 1024, &buf);

        sb = &buf;

        if (sb->s_magic == EXT2_MAGIC_NUMBER)
        {
            bdebug(INFO, "Part: %d is Ext2 Filesystem!\r\n", i);
            bdebug(INFO, "Revision %d.%d\r\n", sb->s_rev_level, sb->s_minor_rev_level);

            bdebug(INFO, "First Data Block: %d\r\n", sb->s_first_data_block);


            blockSize = (1024 << sb->s_log_block_size);

            switch (blockSize)
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

            inodesPerGroup = sb->s_inodes_per_group;

            inodeSize = sb->s_inode_size;

            bgdtStart = blockSize >= BS_2KIB ? blockSize : blockSize * 2;

            ext2Parts[ext2Count] = i;
            ext2Count++;
        }

        else
        {
            bdebug(INFO, "Part: %d is not an Ext2 Partition.\r\n", i);
        }
    }

    // TODO: Check if we're booting off of installation media (e.g. USB), in which case the kernel will be located on the current media.
    if (ext2Count < 1)
    {
        print(u"The SnowOS Kernel must be located on an Ext2 Partition!\r\n");
        for(;;);
    }
}

void readBlock(uint64_t block, void *buffer)
{
    int offset = blockSize * block;

    readPart(ext2Parts[0], offset, blockSize, buffer);

    bdebug(INFO, "Finished block read!\r\n");
}

struct ext2_inode* readInode(uint32_t inode)
{
    bdebug(INFO, "Inodes per group: %d\r\n", inodesPerGroup);

    uint64_t block_group = (inode - 1) / inodesPerGroup;
    uint64_t local_i_idx = (inode - 1) % inodesPerGroup;

    bdebug(INFO, "Block Group Descriptor Start: %d\r\n", bgdtStart);

    bdebug(INFO, "Inode Number: %d, Block Group Number: %d, Local Inode Index: %d\r\n", inode, block_group, local_i_idx);

    uint8_t bgdt_buf[32];

    struct ext2_bgdt *bgdt;

    bdebug(INFO, "Size of BGDT: %d\r\n", sizeof(struct ext2_bgdt));

    readPart(ext2Parts[0], bgdtStart + (sizeof(struct ext2_bgdt) * block_group), 32, bgdt_buf);

    bgdt = &bgdt_buf;

    bdebug(INFO, "Directories in Inode's block group: %d\r\n", bgdt->bg_used_dirs_count);
    bdebug(INFO, "Starting block of Inode Table: %d\r\n", bgdt->bg_inode_table);
    bdebug(INFO, "Inode size: %d\r\n", inodeSize);

    uint8_t ino_buf[inodeSize];

    readPart(ext2Parts[0], (bgdt->bg_inode_table * blockSize) + (local_i_idx * inodeSize), inodeSize, ino_buf);

    struct ext2_inode *ino;

    ino = &ino_buf;

    if ((ino->i_mode & 0xF000) == EXT2_S_IFDIR)
    {
        bdebug(INFO, "Is directory!\r\n");
    }

    bdebug(INFO, "Blocks used by inode: %d\r\n", ino->i_blocks);

    for (int i = 0; i < 15; i++)
    {
        bdebug(INFO, "Block %d ID: %d\r\n", i, ino->i_block[i]);
    }

    bdebug(INFO, "Mode is: 0x%x\r\n", ino->i_mode);

    return ino;
}

int readFilepath(char *filepath, uint64_t filepath_size, int *inode_id)
{
    struct ext2_inode *ino;
    struct ext2_dir_entry *dir;
    BOOLEAN shouldCont = TRUE; // Tells us to continue searching
    BOOLEAN found;           // Tells us we found the next file
    uint64_t nameSize;   // Amount of characters in the current name
    char *name;           // Name of the current directory entry
    uint64_t filepathOffset = 1;     // Offset in the filepath to start looking
    char blockBuf[blockSize];
    int i = 0;

    uefiAllocatePool(256, &name);     // Allocate memory for the name
    memset(name, 0, 256);

    if(filepath[0] != '/')
    {
        bdebug(ERROR, "Need to start with '/'");
    }

    ino = readInode(EXT2_ROOT_INO);

    cont:
    readBlock(ino->i_block[0], blockBuf);
    memset(name, 0, 256);
    nameSize = 0;
    // Grab the next name
    for(int i = 0; filepath[filepathOffset] != '/'; i++)
    {
        name[i] = filepath[filepathOffset];
        bdebug(NONE, "%c", name[i]);
        filepathOffset++;
        nameSize++;

        if(filepathOffset == filepath_size)
        {
            bdebug(INFO, "Reached end of filepath\r\n");
            shouldCont = FALSE;
            break;
        }
    }
    bdebug(NONE, "\r\n");
    filepathOffset++;    // Skip over the next /

    i = 0;

    while(!(i >= blockSize))
    {
        if(memcmp(&blockBuf[i], &name[0], nameSize) == 0)
        {
            bdebug(INFO, "Found next file!\r\n");
            found = TRUE;
            break;
        } else found = FALSE;
        i++;
    }

    if (!found)
    {
        return FALSE;
    }
    bdebug(INFO, "I is %d\r\n", i);
    dir = &blockBuf[i - 8];
    
    ino = readInode(dir->inode);

    if(!shouldCont && found)
    {
        *inode_id = dir->inode;
        return TRUE;
    }
    else if (shouldCont) goto cont;
    else return FALSE;
}