global loadIdtAsm

section .text

idtr    dw 0
        dq 0

loadIdtAsm:
    mov [idtr], di
    mov [idtr+2], rsi
    lidt [idtr]
    sti
    ret