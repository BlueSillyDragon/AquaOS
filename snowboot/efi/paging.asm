global loadPageTables

section .text

loadPageTables:
    mov rax, rdi
    mov cr3, rax
    ret