#include <stdio.h>
#include "../src/Symbols_Table.h"
#include "../src/tree.h"


void closeFile(int status, void *file) {
    (void)status; // Ignorer le paramètre status
    fclose((FILE *)file);
}

void closeProgTable(int status, void *table) {
    (void)status; // Ignorer le paramètre status
    free_Program_table((Program_Table *)table);
}

void closeTree(int status, void *tree) {
    (void)status; // Ignorer le paramètre status
    Node *node = (Node *)tree;
    printf("before delete\n");
    if (node == NULL) printf("l'abre est vide (on_exit)\n");
    else deleteTree(node);
    printf("after delete\n");
}