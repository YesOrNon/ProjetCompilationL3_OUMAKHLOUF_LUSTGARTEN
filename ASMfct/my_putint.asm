section .text
global my_putint

my_putint:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov rax, rdi ; récupération du chiffre en parametre
    mov r8, 1 ; taille
    mov r9, 15 ; indice adresse
    mov r10, 10
negatif:
    cmp rdi, 0
    jge loop
    imul rax, -1
loop:
    cmp rax, 0
    jle check
    mov rdx, 0 ; initialisation à 0 pour la div
    idiv r10
    add dl, '0' ; convertir en char
    mov [rsp+r9], dl ; rdx sur 1 octet
    inc r8
    dec r9
    jmp loop
check:    
    cmp r9, 15 ; Vérification
    je end
    cmp rdi, 0
    jge return
    mov dl, '-'
    mov [rsp+r9], dl
    inc r8
    dec r9
return:
    mov rax, 1 ; écrire
    mov rdi, 1 ; stdout
    add rsp, r9
    mov rsi, rsp ; adresse
    mov rdx, r8 ; taille
    syscall
end:
    mov rsp, rbp
    pop rbp
    ret
