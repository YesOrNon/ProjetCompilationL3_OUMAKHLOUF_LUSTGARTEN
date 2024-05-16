#ifndef __TRAD__
#define __TRAD__

#include <stdio.h>
#include "tree.h"
#include "Symbols_Table.h"

int write_start(FILE * anonymous, Symbols_Table * globals);

int write_end(FILE * anonymous);

int write_add_sub(FILE * anonymous, Node *node, Program_Table* program);

int write_mul(FILE * anonymous, Node *node, Program_Table* program);

int write_div_mod(FILE * anonymous, Node *node, Program_Table* program);

int write_number(FILE * anonymous, int val);

int write_eq_order(FILE * anonymous, Node *node, Program_Table* program);

int write_negation(FILE * anonymous, Node *node, Program_Table* program);

int write_and_or(FILE * anonymous, Node *node, Program_Table* program);

int eval_expr(FILE * anonymous, Node * node, Program_Table* program);

int write_aff_global(FILE * anonymous, Symbol * tmp, int indice);

int write_global_value(FILE * anonymous, Symbol * tmp, int indice);

int write_if(FILE * anonymous, Node *node, Program_Table* program, int _else);

int write_else(FILE * anonymous, Node *node, Program_Table* program, int ifToElse);

void cToAsm(Node *node, FILE * file, Program_Table* program);

#endif