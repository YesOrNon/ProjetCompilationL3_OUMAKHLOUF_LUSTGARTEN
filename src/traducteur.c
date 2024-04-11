#include "traducteur.h"
#include <string.h>

int main_flag = 0;

int write_start(FILE * anonymous) {
    return fprintf(anonymous,"global _start\nsection .text\n_start:\n");
}

int write_end(FILE * anonymous) {
    return fprintf(anonymous, "mov rax, 60\nmov rdi, 0\nsyscall\n");
}

int write_add(FILE * anonymous, Node * node) {
    eval_expr(anonymous, FIRSTCHILD(node));
    eval_expr(anonymous, SECONDCHILD(node));
    return fprintf(anonymous, "pop r8\npop r9\nadd r8, r9\npush r8\n");
}

int write_sub(FILE * anonymous, Node * node) {
    eval_expr(anonymous, FIRSTCHILD(node));
    eval_expr(anonymous, SECONDCHILD(node));
    return fprintf(anonymous, "pop r9\npop r8\nsub r8, r9\npush r8\n");
}

int write_mul(FILE * anonymous, Node * node) {
    eval_expr(anonymous, FIRSTCHILD(node));
    eval_expr(anonymous, SECONDCHILD(node));
    return fprintf(anonymous, "pop r8\npop r9\nimul r8, r9\npush r8\n");
}

int write_div(FILE * anonymous, Node * node) {
    eval_expr(anonymous, FIRSTCHILD(node));
    eval_expr(anonymous, SECONDCHILD(node));
    return fprintf(anonymous, "pop r8\nmov rdx, 0\npop rax\nidiv r8\npush rax\n");
}

int write_mod(FILE * anonymous, Node * node) {
    eval_expr(anonymous, FIRSTCHILD(node));
    eval_expr(anonymous, SECONDCHILD(node));
    return fprintf(anonymous, "pop r8\nmov rdx, 0\npop rax\nidiv r8\npush rdx\n");
}

int write_number(FILE * anonymous, int val) {
    return fprintf(anonymous, "push %d\n", val);
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

