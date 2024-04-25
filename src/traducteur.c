#include "traducteur.h"
#include "Symbols_Table.h"
#include <string.h>

static const char * const txt_getchar = 
    "my_getchar:\n"
    "    push 0\n"
    "    mov rsi, rsp\n"
    "    mov rdx, 1\n"
    "    mov rax, 0\n"
    "    mov rdi, 0\n"
    "    syscall\n"
    "    pop rax\n"
    "    ret\n";

static const char * const txt_getint = 
    "my_getint:\n"
    "    mov r8, -1\n"
    "    mov r9b, '-'\n"
    /*Le point avant l'étiquette permet de dire que c'est le body de l'étiquette
    c'est le body de l'étiquette complète en cours (ici .body = my_getint.body)*/
    ".body:\n"
    "    imul r8, -1\n"
    "    push 0\n"
    "    mov rsi, rsp\n"
    "    mov rdx, 1\n"
    "    mov rax, 0\n"
    "    mov rdi, 0\n"
    "    syscall\n"
    "    pop rax\n"
    "    cmp al, r9b\n"
    "    je .body\n"
    "    sub rax, 48\n"
    "    cmp al, 10\n"
    "    jl .return\n"
    "    mov rax, 10\n"
    "    ret\n"
    ".return:\n"
    "    imul rax, r8\n"
    "    ret\n";

static const char * const txt_putchar = 
    "my_putchar:\n"
    "    push rdi\n"
    "    mov rsi, rsp\n"
    "    mov rdx, 1\n"
    "    mov rax, 1\n"
    "    mov rdi, 1\n"
    "    syscall\n"
    "    pop rdi\n"
    "    ret\n";

static const char * const txt_putint = 
    "my_putint:\n"
    "    push rbp\n"
    "    mov rbp, rsp\n"
    "    sub rsp, 16\n"
    "    mov rax, rdi\n"
    "    mov r8, 1\n"
    "    mov r9, 15\n"
    "    mov r10, 10\n"
    ".negatif:\n"
    "    cmp rdi, 0\n"
    "    jge loop\n"
    "    imul rax, -1\n"
    ".loop:\n"
    "    cmp rax, 0\n"
    "    jle check\n"
    "    mov rdx, 0\n"
    "    idiv r10\n"
    "    add dl, '0'\n"
    "    mov [rsp+r9], dl\n"
    "    inc r8\n"
    "    dec r9\n"
    "    jmp loop\n"
    ".check:\n"
    "    cmp r9, 15\n"
    "    je end\n"
    "    cmp rdi, 0\n"
    "    jge return\n"
    "    mov dl, '-'\n"
    "    mov [rsp+r9], dl\n"
    "    inc r8\n"
    "    dec r9\n"
    ".return:\n"
    "    mov rax, 1\n"
    "    mov rdi, 1\n"
    "    add rsp, r9\n"
    "    mov rsi, rsp\n"
    "    mov rdx, r8\n"
    "    syscall\n"
    ".end:\n"
    "    mov rsp, rbp\n"
    "    pop rbp\n"
    "    ret\n";

int main_flag = 0;

int write_start(FILE * anonymous) {
    return fprintf(anonymous,   "section .text\n"
                                "global _start\n"
                                "%s"
                                "%s"
                                "%s"
                                "%s"
                                "_start:\n",
                                txt_getchar,
                                txt_getint,
                                txt_putchar,
                                txt_putint);
}

int write_end(FILE * anonymous) {
    return fprintf(anonymous, "\tmov rax, 60\n\tmov rdi, 0\n\tsyscall\n");
}

int write_add(FILE * anonymous, Node * node) {
    eval_expr(anonymous, FIRSTCHILD(node));
    eval_expr(anonymous, SECONDCHILD(node));
    return fprintf(anonymous, "\tpop r8\n\tpop r9\n\tadd r8, r9\n\tpush r8\n");
}

int write_sub(FILE * anonymous, Node * node) {
    eval_expr(anonymous, FIRSTCHILD(node));
    eval_expr(anonymous, SECONDCHILD(node));
    return fprintf(anonymous, "\tpop r9\n\tpop r8\n\tsub r8, r9\n\tpush r8\n");
}

int write_mul(FILE * anonymous, Node * node) {
    eval_expr(anonymous, FIRSTCHILD(node));
    eval_expr(anonymous, SECONDCHILD(node));
    return fprintf(anonymous, "\tpop r8\n\tpop r9\n\timul r8, r9\n\tpush r8\n");
}

int write_div(FILE * anonymous, Node * node) {
    eval_expr(anonymous, FIRSTCHILD(node));
    eval_expr(anonymous, SECONDCHILD(node));
    return fprintf(anonymous, "\tpop r8\n\tmov rdx, 0\n\tpop rax\n\tidiv r8\n\tpush rax\n");
}

int write_mod(FILE * anonymous, Node * node) {
    eval_expr(anonymous, FIRSTCHILD(node));
    eval_expr(anonymous, SECONDCHILD(node));
    return fprintf(anonymous, "\tpop r8\n\tmov rdx, 0\n\tpop rax\n\tidiv r8\n\tpush rdx\n");
}

int write_number(FILE * anonymous, int val) {
    return fprintf(anonymous, "\tpush %d\n", val);
}

int eval_expr(FILE * anonymous, Node * node) {
    switch (node->label) {
        case num:
            write_number(anonymous, node->data.num);
            break;
        case addsubUnaire:
            write_number(anonymous, -FIRSTCHILD(node)->data.num);
            break;
        case addsub:
            if (node->data.byte == '+') {write_add(anonymous, node);}
            else                        {write_sub(anonymous, node);}
            break;
        case divstar:
            if (node->data.byte == '*')         {write_mul(anonymous, node);}
            else if (node->data.byte == '/')    {write_div(anonymous, node);}
            else                                {write_mod(anonymous, node);}
            break;
        case ident:
            break;
        case IDENTs:
            break;
        default:
            break;
    }
    return 0;
}

void my_getint(FILE * file) {
    fprintf(file, "\tcall my_getint\n");
}

void my_getchar(FILE * file) {
    fprintf(file, "\tcall my_getchar\n");
}

void my_putint(FILE * file) {
    fprintf(file, "\tcall my_putint\n");
}

void my_putchar(FILE * file) {
    fprintf(file, "\tcall my_putchar\n");
}


void cToAsm(Node *node, FILE * file) {
    switch (node->label) {
        case affectation:
            eval_expr(file, SECONDCHILD(node));
            break;
        case fonction:
            if (strcmp(SECONDCHILD(FIRSTCHILD(node))->data.ident, "main") == 0) { /* main */
                main_flag = write_start(file);
            }
            break;
        case ident:
            if (strcmp(node->data.ident, "getint") == 0) my_getint(file);
            else if (strcmp(node->data.ident, "getchar") == 0) my_getchar(file);
            else if (strcmp(node->data.ident, "putchar") == 0) my_putchar(file);
            else if (strcmp(node->data.ident, "putint") == 0) my_putint(file);
            break;
        default:
            break;
    }
    for (Node *child = FIRSTCHILD(node); child != NULL; child = child->nextSibling) {
        cToAsm(child, file);
    }
    if (node->label == fonction && main_flag > 0) {
        write_end(file);
        main_flag = 0;
    }
}


