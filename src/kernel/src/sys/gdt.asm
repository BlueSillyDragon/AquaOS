global loadGdtAsm

section .text

gdtr    dw 0
        dq 0

loadGdtAsm:
    mov [gdtr], di
    mov [gdtr+2], rsi
    lgdt [gdtr]
    ret

global reloadSegs

reloadSegs:
    push 0x08
    lea rax, [rel .reload_CS]
    push rax
    retfq
.reload_CS:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret
