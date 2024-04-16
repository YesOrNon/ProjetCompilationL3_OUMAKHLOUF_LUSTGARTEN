section .data
    negatif db '-'
section .text
global my_getint

my_getint:
    mov r8, -1
body:
    imul r8, -1
    push 0
    mov rsi, rsp
    mov rdx, 1 ; taille
    mov rax, 0 ; read
    mov rdi, 0 ; stdin
    syscall
    pop rax
    cmp al, byte[negatif]
    je body
    sub rax, 48
    cmp al, 10
    jl return
    mov rax, 10
    ret
return:
    imul rax, r8
    ret