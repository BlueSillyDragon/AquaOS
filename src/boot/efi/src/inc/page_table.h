#pragma once

#include <stdint.h>

struct pml4_entry_t
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
    uint64_t pdpt_paddr : 38;   // Physical Address of the 4KiB aligned Page-Directory-Pointer-Table;
    unsigned int reserved2 : 2; // Reserved; Must be 0
    unsigned int ignore3 : 11;  // Ignored
    unsigned int execute_disable : 1;   // Execute disable;
};

struct pdpt_entry_t
{
    unsigned int present : 1;   // Present; Points to a PDPT if 1
    unsigned int read_write : 1;    // Read/Write; Cannot write to 512GB reigon controlled by this entry if 0
    unsigned int user_kernel : 1;   // User/Supervisor; Usermode may not access this 512GB reigon if 0
    unsigned int page_wt : 1;   // Page-level Write-through;
    unsigned int page_cd : 1;   // Page-level Cache disable;
    unsigned int accessed : 1; // Accessed;
    unsigned int ignore : 1; // Ignored
    unsigned int page_size : 1; // Reserved; If this is note 0, it maps a 1GB page
    unsigned int ignore2 : 3; // Ignored
    unsigned int restart : 1; // Restart; Ignored for normal paging, used for HLAT Paging
    uint64_t pd_paddr : 38;   // Physical Address of the 4KiB aligned Page-Directory-Pointer-Table;
    unsigned int reserved : 2; // Reserved; Must be 0
    unsigned int ignore3 : 11;  // Ignored
    unsigned int execute_disable : 1;   // Execute disable;
};

struct pd_entry_t
{
    unsigned int present : 1;   // Present; Points to a PDPT if 1
    unsigned int read_write : 1;    // Read/Write; Cannot write to 512GB reigon controlled by this entry if 0
    unsigned int user_kernel : 1;   // User/Supervisor; Usermode may not access this 512GB reigon if 0
    unsigned int page_wt : 1;   // Page-level Write-through;
    unsigned int page_cd : 1;   // Page-level Cache disable;
    unsigned int accessed : 1; // Accessed;
    unsigned int ignore : 1; // Ignored
    unsigned int page_size : 1; // Reserved; If this is note 0, it maps a 2MB page
    unsigned int ignore2 : 3; // Ignored
    unsigned int restart : 1; // Restart; Ignored for normal paging, used for HLAT Paging
    uint64_t pt_paddr : 38;   // Physical Address of the 4KiB aligned Page-Directory-Pointer-Table;
    unsigned int reserved : 2; // Reserved; Must be 0
    unsigned int ignore3 : 11;  // Ignored
    unsigned int execute_disable : 1;   // Execute disable;
};

struct pt_entry_t
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
    uint64_t page_paddr : 38;   // Physical Address of the 4KiB Page
    unsigned int reserved : 2; // Reserved; Must be 0
    unsigned int ignore3 : 7;  // Ignored
    unsigned int protc_key : 4;     // Protection Key; If CR4.PKE = 1 or CR4.PKS = 1, then this mat control access rights, otherwise, ignored
    unsigned int execute_disable : 1;   // Execute disable;
};