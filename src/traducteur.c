#include "traducteur.h"
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
    "    jge .loop\n"
    "    imul rax, -1\n"
    ".loop:\n"
    "    cmp rax, 0\n"
    "    jle .check\n"
    "    mov rdx, 0\n"
    "    idiv r10\n"
    "    add dl, '0'\n"
    "    mov [rsp+r9], dl\n"
    "    inc r8\n"
    "    dec r9\n"
    "    jmp .loop\n"
    ".check:\n"
    "    cmp r9, 15\n"
    "    je .end\n"
    "    cmp rdi, 0\n"
    "    jge .return\n"
    "    mov dl, '-'\n"
    "    mov [rsp+r9], dl\n"
    "    dec r9\n"
    ".return:\n"
    "    inc r9\n"
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

int write_start(FILE * anonymous, Symbols_Table* globals) {
    int res_static = 0;
    if (globals->index)
        res_static = get_last_adress(globals); //globals->tab[globals->index].size;
    return fprintf(anonymous,   "section .bss\n"
                                "VarGlobals: resb %d\n"
                                "section .text\n"
                                "global _start\n"
                                "%s"
                                "%s"
                                "%s"
                                "%s"
                                "_start:\n",
                                res_static,
                                txt_getchar,
                                txt_getint,
                                txt_putchar,
                                txt_putint);
}

int write_end(FILE * anonymous) {
    return fprintf(anonymous, "\tmov rax, 60\n\tmov rdi, 0\n\tsyscall\n");
}

int write_add(FILE * anonymous, Node * node, Program_Table* program) {
    eval_expr(anonymous, FIRSTCHILD(node), program);
    eval_expr(anonymous, SECONDCHILD(node), program);
    return fprintf(anonymous, "\tpop r8\n\tpop r9\n\tadd r8, r9\n\tpush r8\n");
}

int write_sub(FILE * anonymous, Node * node, Program_Table* program) {
    eval_expr(anonymous, FIRSTCHILD(node), program);
    eval_expr(anonymous, SECONDCHILD(node), program);
    return fprintf(anonymous, "\tpop r9\n\tpop r8\n\tsub r8, r9\n\tpush r8\n");
}

int write_mul(FILE * anonymous, Node * node, Program_Table* program) {
    eval_expr(anonymous, FIRSTCHILD(node), program);
    eval_expr(anonymous, SECONDCHILD(node), program);
    return fprintf(anonymous, "\tpop r8\n\tpop r9\n\timul r8, r9\n\tpush r8\n");
}

int write_div(FILE * anonymous, Node * node, Program_Table* program) {
    eval_expr(anonymous, FIRSTCHILD(node), program);
    eval_expr(anonymous, SECONDCHILD(node), program);
    return fprintf(anonymous, "\tpop r8\n\tmov rdx, 0\n\tpop rax\n\tidiv r8\n\tpush rax\n");
}

int write_mod(FILE * anonymous, Node * node, Program_Table* program) {
    eval_expr(anonymous, FIRSTCHILD(node), program);
    eval_expr(anonymous, SECONDCHILD(node), program);
    return fprintf(anonymous, "\tpop r8\n\tmov rdx, 0\n\tpop rax\n\tidiv r8\n\tpush rdx\n");
}

int write_number(FILE * anonymous, int val) {
    return fprintf(anonymous, "\tpush %d\n", val);
}

int eval_expr(FILE * anonymous, Node * node, Program_Table* program) {
    switch (node->label) {
        case num:
            write_number(anonymous, node->data.num);
            break;
        case addsubUnaire:
            write_number(anonymous, -FIRSTCHILD(node)->data.num);
            break;
        case addsub:
            if (node->data.byte == '+') {write_add(anonymous, node, program);}
            else                        {write_sub(anonymous, node, program);}
            break;
        case divstar:
            if (node->data.byte == '*')         {write_mul(anonymous, node, program);}
            else if (node->data.byte == '/')    {write_div(anonymous, node, program);}
            else                                {write_mod(anonymous, node, program);}
            break;
        case ident: { // Les accolades servent à pouvoir faire une déclaration dans un case (impossible sinon)
            Symbol * tmp = find_Symbol(program->globals, node->data.ident);
            if (tmp){
                int indice = 0;
                if (FIRSTCHILD(node)){
                    eval_expr(anonymous, FIRSTCHILD(node), program);
                    indice = 1;
                }
                write_global_value(anonymous, tmp->deplct, tmp->type, indice);
            }
        }
            break;
        case IDENTs:
            break;
        default:
            break;
    }
    return 0;
}

int write_aff_global(FILE * anonymous, int deplct, Type type, int indice){
    if (indice)
        fprintf(anonymous, "\tpop r8\n");
    else
        fprintf(anonymous, "\tmov r8, 0\n");
    fprintf(anonymous, "\tpop r9\n");
    switch (type) {
        case CHAR:
            fprintf(anonymous, "\tmov byte[VarGlobals+%d+r8], r9b\n", deplct);
            break;
        case INT:
            fprintf(anonymous, "\tmov dword[VarGlobals+%d+r8*4], r9d\n", deplct);
            break;    
        default:
            return 0;
            break;
    }
    return 1;
}

int write_global_value(FILE * anonymous, int deplct, Type type, int indice){
    if (indice)
        fprintf(anonymous, "\tpop r8\n");
    else
        fprintf(anonymous, "\tmov r8, 0\n");
    fprintf(anonymous, "\tmov r9, 0\n");
    switch (type) {
        case CHAR:
            fprintf(anonymous, "\tmov r9b, byte[VarGlobals+%d+r8]\n", deplct);
            break;
        case INT:
            fprintf(anonymous, "\tmov r9d, dword[VarGlobals+%d+r8*4]\n", deplct);
            break;
        default:
            return 0;
            break;
    }
    fprintf(anonymous, "\tpush r9\n");
    return 1;
}

// int write_if(FILE * anonymous, Node *node, Program_Table* program){
//     // do stuff
// }

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


void cToAsm(Node *node, FILE * file, Program_Table* program) {
    switch (node->label) {
        case affectation:
            eval_expr(file, SECONDCHILD(node), program);
            Symbol * tmp = find_Symbol(program->globals, FIRSTCHILD(node)->data.ident);
            if (tmp){
                int indice = 0;
                if (FIRSTCHILD(FIRSTCHILD(node))){
                    eval_expr(file, FIRSTCHILD(FIRSTCHILD(node)), program);
                    indice = 1;
                }
                write_aff_global(file, tmp->deplct, tmp->type, indice);
            }
            break;
        case fonction:
            if (strcmp(SECONDCHILD(FIRSTCHILD(node))->data.ident, "main") == 0) { /* main */
                main_flag = write_start(file, program->globals);
            }
            break;
        case ident:
            if (strcmp(node->data.ident, "getint") == 0) my_getint(file);
            else if (strcmp(node->data.ident, "getchar") == 0) my_getchar(file);
            else if (strcmp(node->data.ident, "putchar") == 0) my_putchar(file);
            else if (strcmp(node->data.ident, "putint") == 0) my_putint(file);
            break;
        case _if:
            if(!THIRDCHILD(node)){ // Si if sans else
                // write_if(file, node, program);
            }
        default:
            break;
    }
    for (Node *child = FIRSTCHILD(node); child != NULL; child = child->nextSibling) {
        cToAsm(child, file, program);
    }
    if (node->label == fonction && main_flag > 0) {
        write_end(file);
        main_flag = 0;
    }
}


