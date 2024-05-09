#include <stdio.h>
#include "../include/Symbols_Table.h"
#include "../include/tree.h"


void closeFile(int status, void *file) {
    (void)status; // Ignorer le paramètre status
    fclose((FILE *)file);
}

void closeProgTable(int status, void *table) {
    (void)status; // Ignorer le paramètre status
    free_Program_table((Program_Table *)table);
}