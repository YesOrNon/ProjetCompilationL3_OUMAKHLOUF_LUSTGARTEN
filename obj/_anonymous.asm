global _start
section .text
_start:
push -2
push 4
push 2
pop r8
pop r9
add r8, r9
push r8
push 2
push 5
push 2
pop r9
pop r8
sub r8, r9
push r8
push 2
push 2
pop r8
pop r9
imul r8, r9
push r8
push 1
pop r8
mov rdx, 0
pop rax
idiv r8
push rax
push 3
push 1
pop r8
pop r9
add r8, r9
push r8
push 122
push 11
pop r8
mov rdx, 0
pop rax
idiv r8
push rdx
mov rax, 60
mov rdi, 0
syscall
