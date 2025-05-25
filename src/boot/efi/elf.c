#include <efi.h>
#include <stdint.h>
#include "inc/elf.h"
#include "efi/efidef.h"
#include "inc/fs/ext2.h"
#include "inc/log.h"
#include "inc/fs/filesystem.h"
#include "inc/memory_services.h"
#include "inc/print.h"

int isElf(uint64_t inoNum)
{
    struct ext2_inode *ino;
    struct elf_header *inoHdr;
    char blockBuf[4096];

    ino = readInode(inoNum);
    readBlock(ino->i_block[0], blockBuf);
    inoHdr = &blockBuf;

    if (inoHdr->magic_number == ELF_MAGIC_NUMBER)
    {
        bdebug(INFO, "Is ELF!\r\n");

        // AquaOS only runs on 64bit systems, so make sure ELF is 64bit
        if (inoHdr->arch != 2)
        {
            print(u"SnowBoot can only run 64bit executables! However ELF is 32bits!\r\n");
            return 0;
        }
        // Finally, confirm the ELF as executable
        if(inoHdr->type != ELF_EXECUTABLE)
        {
            print(u"ELF File is not executable!\r\n");
            return 0;
        }

        // ELF is valid, so return true (1)
        return 1;

    }
    return 0;
}

uint64_t loadElf(uint64_t inoNum, uint64_t *entryOffset)
{
    struct ext2_inode *ino;
    struct elf_header *inoHdr;
    struct program_header *inoPhdr;
    char blockBuf[4096];
    char indir_buf[4096];
    uint64_t physAddr;

    ino = readInode(inoNum);
    readBlock(ino->i_block[0], blockBuf);
    inoHdr = &blockBuf;
    *entryOffset = inoHdr->p_entry_offs;

    bdebug(INFO, "Kernel load offs: 0x%x\r\n", *entryOffset);

    inoPhdr = &blockBuf[inoHdr->p_table_offs];

    // Load into memory
    uefiAllocatePages(256, &physAddr, EfiReservedMemoryType);    // Should probably un-hardcode this at some point

    char *pt = (uint64_t *)physAddr;

    for(int i = 1; i < 13; i++)
    {
        ino = readInode(inoNum);  // Read again just to insure that nothing has changed

        if(ino->i_block[i] == 0){break;}

        if(i < 12)
        {
            readBlock(ino->i_block[i], blockBuf);
            for(uint64_t i = 0; i < 4096; i++)
            {
                pt[i] = blockBuf[i];
            }
            pt += 0x1000;
        }

        else if (i == 12)
        {
            readBlock(ino->i_block[i], blockBuf);
            for (uint64_t j = 0; j < 128; j++)
            {
                readBlock(blockBuf[j], indir_buf);
                if(blockBuf[j] == 0){break;}
                for (uint64_t k = 0; k < 4096; k++)
                {
                    pt[k] = indir_buf[k];
                }
                pt += 0x1000;
            }
        }
    }

    return physAddr;
}