#ifndef __TRAD__
#define __TRAD__

#include <stdio.h>
#include "tree.h"

int write_start(FILE * anonymous);

int write_end(FILE * anonymous);

void cToAsm(Node *node, FILE * file);

#endif