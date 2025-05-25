#pragma once
#include <stdint.h>

#define EXT2_MAGIC_NUMBER 0xEF53

#define EXT2_GOOD_OLD_REV 0
#define EXT2_DYANMIC_REV 1

#define BS_1KIB 1024
#define BS_2KIB 2048
#define BS_4KIB 4096
#define BS_8KIB 8192

// i_mode values
#define EXT2_S_IFSOCK 0xC000
#define EXT2_S_IFLNK 0xA000
#define EXT2_S_IFREG 0x8000
#define EXT2_S_IFBLK 0x6000
#define EXT2_S_IFDIR 0x4000
#define EXT2_S_IFCHR 0x2000
#define EXT2_S_IFIFO 0x1000

#define EXT2_S_ISUID 0x0800
#define EXT2_S_ISGID 0x0400
#define EXT2_S_ISVTX 0x0200

#define EXT2_S_IRUSR 0x0100
#define EXT2_S_IWUSR 0x0080
#define EXT2_S_IXUSR 0x0040
#define EXT2_S_IRGRP 0x0020
#define EXT2_S_IWGRP 0x0010
#define EXT2_S_IXGRP 0x0008
#define EXT2_S_IROTH 0x0004
#define EXT2_S_IWOTH 0x0002
#define EXT2_S_IXOTH 0x0000

#define EXT2_ROOT_INO 2

struct ext2_superblock
{
    uint32_t s_inodes_count;
    uint32_t s_blocks_count;
    uint32_t s_r_blocks_count;
    uint32_t s_free_blocks_count;
    uint32_t s_free_inodes_count;
    uint32_t s_first_data_block;
    uint32_t s_log_block_size;
    uint32_t s_log_frag_size;
    uint32_t s_blocks_per_group;
    uint32_t s_frags_per_group;
    uint32_t s_inodes_per_group;
    uint32_t s_mtime;
    uint32_t s_wtime;
    uint16_t s_mnt_count;
    uint16_t s_max_mnt_count;
    uint16_t s_magic;
    uint16_t s_state;
    uint16_t s_errors;
    uint16_t s_minor_rev_level;
    uint32_t s_lastcheck;
    uint32_t s_checkinterval;
    uint32_t s_creator_os;
    uint32_t s_rev_level;
    uint16_t s_resuid;
    uint16_t s_resgid;
    // Ext2 Dynamic Rev Specific
    uint32_t s_first_ino;
    uint16_t s_inode_size;
    uint16_t s_block_group_nr;
    uint32_t s_feature_compat;
    uint32_t s_feature_incompat;
    uint32_t s_feature_ro_compat;
    uint64_t s_uuid[2];
    char s_volume_name[16];
    uint64_t s_last_mounted[8];
    uint32_t s_algo_bitmap;
    // Performance hints
    uint8_t s_prealloc_blocks;
    uint8_t s_prealloc_dir_blocks;
    uint16_t alignment; // Don't touch!
    // Journaling Support
    uint64_t s_journal_uuid[2];
    uint32_t s_journal_inum;
    uint32_t s_journal_dev;
    uint32_t s_last_orphan;
    // Directory Indexing Support
    uint32_t s_hash_seed[4];
    uint8_t s_def_hash_version;
    uint8_t padding_res[3];     // Padding - Reserved for future expansion
    // Other options
    uint32_t s_default_mount_options;
    uint32_t s_first_meta_bg;
    // Rest of the super block is reserved for future revisions
};

struct ext2_bgdt
{
    uint32_t bg_block_bitmap;
    uint32_t bg_inode_bitmap;
    uint32_t bg_inode_table;
    uint16_t bg_free_blocks_count;
    uint16_t bg_free_inodes_count;
    uint16_t bg_used_dirs_count;
    uint16_t bg_pad;    // Padding
    uint16_t bg_res[6]; // Reserved for future revisions
};

struct ext2_i_linux
{
    uint8_t l_i_frag;
    uint8_t l_i_fsize;
    uint16_t res1;
    uint16_t l_i_uid_high;
    uint16_t l_i_gid_high;
    uint32_t res2;
};

struct ext2_inode
{
    uint16_t i_mode;
    uint16_t i_uid;
    uint32_t i_size;
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint32_t i_dtime;
    uint16_t i_gid;
    uint16_t i_links_count;
    uint32_t i_blocks;
    uint32_t i_flags;
    uint32_t i_osd1;
    uint32_t i_block[15];
    uint32_t i_generation;
    uint32_t i_file_acl;
    uint32_t i_dir_acl;
    uint32_t i_faddr;
    struct ext2_i_linux i_osd2;
};

struct ext2_dir_entry
{
    uint32_t inode;
    uint16_t rec_len;
    uint8_t name_len;
    uint8_t file_type;      // Higher 8 bits of name_len in Revision 0
    char name [255];
};