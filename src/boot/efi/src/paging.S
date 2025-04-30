.text
.globl loadPageTables
loadPageTables:
    mov rax, rdi
    mov cr3, rax
    ret