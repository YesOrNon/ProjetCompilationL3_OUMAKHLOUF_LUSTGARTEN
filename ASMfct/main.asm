section .text
global _start
extern my_putchar
extern my_putint
extern my_getchar
extern my_getint
_start:
    mov r11, rsp ; Début alignement de la pile
    sub rsp, 8 
    and rsp, -16 ; Masque
    mov qword [rsp], r11 ; Fin alignement de la pile
    mov dil, 's' ; dil 1 octet de rdi
    call my_putchar
    mov dil, 10
    call my_putchar
    mov rdi, -3571113
    call my_putint
    mov dil, 10
    call my_putchar
    ;call my_getchar
    ;mov rdi, rax
    ;call my_putchar
    call my_getint
    mov rdi, rax
    call my_putint
return:
    mov rax, 60
    mov rdi, 0
    syscall