[bits 16]
[org 0x0]

start:
    mov ah, 0x0e
    mov al, 'S'
    int 0x10
    hlt
.halt:
    jmp .halt

loader db "AQUALOAD", 0x0D, 0x0A, 0