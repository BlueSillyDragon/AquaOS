#include <efi.h>
#include <stdint.h>
#include "inc/elf.h"
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

uint64_t load_elf(uint64_t ino_num)
{
    struct ext2_inode *ino;
    struct elf_header *ino_hdr;
    struct program_header *ino_phdr;
    char block_buf[4096];
    uint64_t phys_addr;

    ino = read_inode(ino_num);
    read_block(ino->i_block[0], block_buf);
    ino_hdr = &block_buf;

    ino_phdr = &block_buf[ino_hdr->p_table_offs];

    bdebug(INFO, "Kernel load offs: 0x%x\r\n", ino_hdr->p_entry_offs);

    // Load into memory
    ino = read_inode(ino_num);  // Read again just to insure that nothing has changed
    read_block(ino->i_block[1], block_buf); // Just reading second block for now

    for(int i = 0; i < 4096; i++)
    {
        bdebug(NONE, "%c", block_buf[i]);
    }
    bdebug(NONE, "\r\n");

    uefi_allocate_pages(1, &phys_addr);

    char *pt = (uint64_t *)phys_addr;

    for(uint64_t i = 0; i < 4096; i++)
    {
        pt[i] = block_buf[i];
    }

    for(int i = 0; i < 4096; i++)
    {
        bdebug(NONE, "%c", pt[i]);
    }
    bdebug(NONE, "\r\n");

    return phys_addr;
}