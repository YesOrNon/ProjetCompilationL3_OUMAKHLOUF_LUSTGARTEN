#include "traducteur.h"
#include <string.h>

int main_flag = 0;

int write_start(FILE * anonymous) {
    return fprintf(anonymous,"global _start\nsection .text\n_start:\n");
}

int write_end(FILE * anonymous) {
    return fprintf(anonymous, "mov rax, 60\nmov rdi, 0\nsyscall\n");
}

int write_sub(FILE * anonymous) {
    return fprintf(anonymous, ";ceci est un test\n");
}

void cToAsm(Node *node, FILE * file) {
    switch (node->label) {
        case fonction:
            if (strcmp(SECONDCHILD(FIRSTCHILD(node))->data.ident, "main") == 0) { /* main */
                main_flag = write_start(file);
            }
            break;
        case addsub:
            if (node->data.byte == '-' && main_flag > 0){
                write_sub(file);
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

