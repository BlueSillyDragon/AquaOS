[bits 16]
[org 0x7E00]

start:
    mov ah, 0x0e
    mov al, 'S'
    int 0x10
    hlt
.halt:
    jmp .halt

loader db "LWDLOADER", 0x0D, 0x0A, 0