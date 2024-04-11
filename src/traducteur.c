#include "traducteur.h"
#include <string.h>

int write_start(FILE * anonymous) {
    return fprintf(anonymous,"global _start\nsection .text\n_start:\n");
}

int write_end(FILE * anonymous) {
    return fprintf(anonymous, "mov rax, 60\nmov rdi, 0\nsyscall\n");
}

void cToAsm(Node *node, FILE * file) {
    int main_flag = 0;
  switch (node->label) {
    case program:
        break;
    case fonction:
        if (strcmp(SECONDCHILD(FIRSTCHILD(node))->data.ident, "main") == 0) { /* main */
            main_flag = write_start(file);
        }

        if (main_flag) {
            write_end(file);
        }
        break;
    default:
      break;
  }
  for (Node *child = FIRSTCHILD(node); child != NULL; child = child->nextSibling) {
    cToAsm(child, file);
  }
}

