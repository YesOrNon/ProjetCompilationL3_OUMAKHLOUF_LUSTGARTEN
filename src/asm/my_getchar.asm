section .text
global my_getchar

my_getchar:
    push 0
    mov rsi, rsp
    mov rdx, 1 ; taille
    mov rax, 0 ; read
    mov rdi, 0 ; stdin
    syscall
    pop rax
    ret