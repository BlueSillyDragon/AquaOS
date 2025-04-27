.text
.globl disablePaging
disablePaging:
  mov rbx, cr0
  mov rax, 0x7fffffff
  and rbx, rax
  mov rax, rdi
  mov cr0, rbx
  ret

.globl enablePaging
enablePaging:
    mov cr3, rax
    mov rbx, cr0
    mov rbx, 0x80010033
    mov cr0, rbx
    ret