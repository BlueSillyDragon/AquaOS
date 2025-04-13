#pragma once
#include <stdint.h>

#define ELF_MAGIC_NUMBER 0x464C457F // 0x7F and then ELF

// Instruction set values
#define ELF_INST_NONE 0x00
#define ELF_INST_SPARC 0x02
#define ELF_INST_X86 0x03
#define ELF_INST_MIPS 0x08
#define ELF_INST_POWERPC 0x14
#define ELF_INST_ARM 0x28
#define ELF_INST_SUPERH 0x2A
#define ELF_INST_IA64 0x32
#define ELF_INST_X86_64 0x3E
#define ELF_INST_AARCH64 0xB7
#define ELF_INST_RISCV 0xF3

// Type values
#define ELF_RELOCATABLE 1
#define ELF_EXECUTABLE 2
#define ELF_SHARED 3
#define ELF_CORE 4

struct elf_header
{
    uint32_t magic_number;
    uint8_t arch;
    uint8_t endian;
    uint8_t hdr_version;
    uint8_t os_abi;
    uint8_t padding[8];
    uint16_t type;
    uint16_t instruc_set;
    uint32_t version;
    uint64_t p_entry_offs;
    uint64_t p_table_offs;
    uint64_t s_table_offs;
    uint32_t flags;
    uint16_t header_size;
    uint16_t p_entry_size;
    uint16_t p_entry_num;
    uint16_t s_entry_size;
    uint16_t s_entry_num;
    uint16_t s_index;
};

struct program_header
{
    uint32_t seg_type;
    uint32_t flags;
    uint64_t offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t alignment;
};