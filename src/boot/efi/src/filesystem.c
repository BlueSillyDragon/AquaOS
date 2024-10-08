#include "inc/globals.h"
#include "inc/fs/ext2.h"
#include "inc/fs/filesystem.h"
#include "inc/disk_services.h"
#include "inc/print.h"

uint8_t ext2_parts[10];
uint8_t ext2_count;

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

            ext2_parts[ext2_count] = i;
        }

        else
        {
            print(u"Part: %d is not an Ext2 Partition.\r\n", i);
        }
    }
}