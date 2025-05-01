[BITS 64]

global setGDT

section .text

gdtr DW 0
     DQ 0

setGDT:
   cli
   mov   [gdtr], di
   mov   [gdtr+2], rsi
   lgdt  [gdtr]
   ret

global reloadSegments

reloadSegments:
    push 0x08   ; Push the Code segment to the stack
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