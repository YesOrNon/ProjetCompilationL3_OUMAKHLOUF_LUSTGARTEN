#ifndef __TRAD__
#define __TRAD__

#include <stdio.h>
#include "tree.h"
#include "Symbols_Table.h"

int write_start(FILE * anonymous, Symbols_Table * globals);

int write_end(FILE * anonymous);

int write_add(FILE * anonymous, Node *node, Program_Table* program);

int write_sub(FILE * anonymous, Node *node, Program_Table* program);

int write_mul(FILE * anonymous, Node *node, Program_Table* program);

int write_div(FILE * anonymous, Node *node, Program_Table* program);

int write_mod(FILE * anonymous, Node *node, Program_Table* program);

int write_number(FILE * anonymous, int val);

int eval_expr(FILE * anonymous, Node * node, Program_Table* program);

int write_aff_global(FILE * anonymous, int deplct, Type type, int indice);

int write_global_value(FILE * anonymous, int deplct, Type type, int indice);

int write_if(FILE * anonymous, Node *node, Program_Table* program);

void cToAsm(Node *node, FILE * file, Program_Table* program);

#endif