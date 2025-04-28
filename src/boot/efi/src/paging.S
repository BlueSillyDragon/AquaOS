.text
.globl loadPageTables
loadPageTables:
    mov rax, rdi
    mov cr3, rax
    ret
.globl kernel_jmp
kernel_jmp:
    mov rax, 0x7e0d1000
    jmp rax