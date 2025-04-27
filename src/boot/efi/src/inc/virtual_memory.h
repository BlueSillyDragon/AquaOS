#pragma once

#include <stdint.h>

/*struct pt_entry_t
{
    unsigned int present : 1;   // Present; Points to a PDPT if 1
    unsigned int read_write : 1;    // Read/Write; Cannot write to 512GB reigon controlled by this entry if 0
    unsigned int user_kernel : 1;   // User/Supervisor; Usermode may not access this 512GB reigon if 0
    unsigned int page_wt : 1;   // Page-level Write-through;
    unsigned int page_cd : 1;   // Page-level Cache disable;
    unsigned int accessed : 1; // Accessed;
    unsigned int ignore : 1; // Ignored
    unsigned int reserved : 1; // Reserved; Must be 0
    unsigned int ignore2 : 3; // Ignored
    unsigned int restart : 1; // Restart; Ignored for normal paging, used for HLAT Paging
    unsigned int pdpt_paddr : 32;   // Physical Address of the 4KiB aligned Page-Directory-Pointer-Table;
    unsigned int reserved2 : 8; // Reserved; Must be 0
    unsigned int ignore3 : 11;  // Ignored
    unsigned int execute_disable : 1;   // Execute disable;
};

struct page_t
{
    unsigned int present : 1;   // Present; Points to a 4KB page if 1
    unsigned int read_write : 1;    // Read/Write; Cannot write to 4KB page controlled by this entry if 0
    unsigned int user_kernel : 1;   // User/Supervisor; Usermode may not access this 4KB page if 0
    unsigned int page_wt : 1;   // Page-level Write-through;
    unsigned int page_cd : 1;   // Page-level Cache disable;
    unsigned int accessed : 1; // Accessed;
    unsigned int dirty : 1; // Dirty; Indicates whether or not software has been written to this page
    unsigned int pat : 1; // PAT
    unsigned int global : 1;    // Global;
    unsigned int ignore2 : 2; // Ignored
    unsigned int restart : 1; // Restart; Ignored for normal paging, used for HLAT Paging
    unsigned int page_paddr : 32;   // Physical Address of the 4KiB Page
    unsigned int reserved : 8; // Reserved; Must be 0
    unsigned int ignore3 : 7;  // Ignored
    unsigned int protc_key : 4;     // Protection Key; If CR4.PKE = 1 or CR4.PKS = 1, then this mat control access rights, otherwise, ignored
    unsigned int execute_disable : 1;   // Execute disable;
};*/

typedef struct page_table_t
{
    uintptr_t entries[512];
} __attribute__((aligned(4096))) page_table_t;

typedef struct {
    int   levels;
    uint64_t top_level;
} pagemap_t;

pagemap_t new_pagemap();

void map_page(pagemap_t pagemap, uint64_t virt_address, uint64_t phys_address, uint64_t flags);
void map_pages(pagemap_t pagemap, uint64_t virt_address, uint64_t phys_address, uint64_t flags, uint64_t count);

uint64_t virt_to_phys(pagemap_t pagemap, uint64_t virt_address);