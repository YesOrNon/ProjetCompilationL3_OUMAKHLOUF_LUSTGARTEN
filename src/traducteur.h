#ifndef __TRAD__
#define __TRAD__

#include <stdio.h>
#include "tree.h"

int write_start(FILE * anonymous);

int write_end(FILE * anonymous);

int write_add(FILE * anonymous, Node *node);

int write_sub(FILE * anonymous, Node *node);

int write_mul(FILE * anonymous, Node *node);

int write_div(FILE * anonymous, Node *node);

int write_mod(FILE * anonymous, Node *node);

int write_number(FILE * anonymous, int val);

int eval_expr(FILE * anonymous, Node * node);

void cToAsm(Node *node, FILE * file);

#endif