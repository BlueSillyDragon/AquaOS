#include <efi.h>
#include <stdint.h>
#include "inc/elf.h"
#include "efi/efidef.h"
#include "inc/fs/ext2.h"
#include "inc/log.h"
#include "inc/fs/filesystem.h"
#include "inc/memory_services.h"
#include "inc/print.h"

int is_elf(uint64_t ino_num)
{
    struct ext2_inode *ino;
    struct elf_header *ino_hdr;
    char block_buf[4096];

    ino = read_inode(ino_num);
    read_block(ino->i_block[0], block_buf);
    ino_hdr = &block_buf;

    if (ino_hdr->magic_number == ELF_MAGIC_NUMBER)
    {
        bdebug(INFO, "Is ELF!\r\n");

        // AquaOS only runs on 64bit systems, so make sure ELF is 64bit
        if (ino_hdr->arch != 2)
        {
            print(u"AquaBoot can only run 64bit executables! However ELF is 32bits!\r\n");
            return 0;
        }
        // Finally, confirm the ELF as executable
        if(ino_hdr->type != ELF_EXECUTABLE)
        {
            print(u"ELF File is not executable!\r\n");
            return 0;
        }

        // ELF is valid, so return true (1)
        return 1;

    }
    return 0;
}

uint64_t load_elf(uint64_t ino_num, uint64_t *entry_offs)
{
    struct ext2_inode *ino;
    struct elf_header *ino_hdr;
    struct program_header *ino_phdr;
    char block_buf[4096];
    char indir_buf[4096];
    uint64_t phys_addr;

    ino = read_inode(ino_num);
    read_block(ino->i_block[0], block_buf);
    ino_hdr = &block_buf;
    *entry_offs = ino_hdr->p_entry_offs;

    bdebug(INFO, "Kernel load offs: 0x%x\r\n", *entry_offs);

    ino_phdr = &block_buf[ino_hdr->p_table_offs];

    // Load into memory
    uefi_allocate_pages(256, &phys_addr, EfiReservedMemoryType);    // Should probably un-hardcode this at some point

    char *pt = (uint64_t *)phys_addr;

    for(int i = 1; i < 13; i++)
    {
        ino = read_inode(ino_num);  // Read again just to insure that nothing has changed

        if(ino->i_block[i] == 0){break;}

        if(i < 12)
        {
            read_block(ino->i_block[i], block_buf);
            for(uint64_t i = 0; i < 4096; i++)
            {
                pt[i] = block_buf[i];
            }
            pt += 0x1000;
        }

        else if (i == 12)
        {
            read_block(ino->i_block[i], block_buf);
            for (uint64_t j = 0; j < 128; j++)
            {
                read_block(block_buf[j], indir_buf);
                if(block_buf[j] == 0){break;}
                for (uint64_t k = 0; k < 4096; k++)
                {
                    pt[k] = indir_buf[k];
                }
                pt += 0x1000;
            }
        }
    }

    return phys_addr;
}