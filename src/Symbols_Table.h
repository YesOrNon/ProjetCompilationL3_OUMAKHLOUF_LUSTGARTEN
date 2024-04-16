#ifndef __SYM__
#define __SYM__

#include "tree.h"

#define INIT_SIZE 128 // Constante raisonable

// STRUCTURE //

typedef enum Type
{
    INT,
    CHAR,
    VOID_,
    FUNCTION,
    DEFAULT
} Type;

typedef struct Symbol
{
    char* ident;        /* name of the symbol */
    Type type;          /* type of the symbol */
    int size;           /* size (in bytes) based on the type */
    long int deplct;    /* next space in memory */
} Symbol;

typedef struct Symbols_Table
{
    Symbol tab[INIT_SIZE];     /* array of symbols */
    long int index;             /* index of the next symbol */
} Symbols_Table;

typedef struct Function_Table
{
    char* ident;                    /* name of the function */
    Type type_ret;                  /* type */
    Symbols_Table* header;          /* parameters */
    Symbols_Table* body;            /* local variables */
    struct Function_Table* next;    /* next function */
} Function_Table;

typedef struct Program_Table
{
    Symbols_Table* globals;         /* global variables */
    Function_Table* functions;      /* functions */
} Program_Table;


// INIT //

/* Create a symbol */
Symbol make_symbol(char* ident, Type type);

/* Initialize a symbol table */
Symbols_Table* init_Sym_table();

/* Initialize a function table */ // Empty header and body
Function_Table* init_Func_table();

/* Initialize a program table */
Program_Table* init_Program_table();


// FREE //

/* Free a symbol */
void free_symbol(Symbol* symbol);

/* Free a symbol table */
void free_Sym_table(Symbols_Table* sym_table);

/* Free a function table */
void free_Func_table(Function_Table* func_table);

/* Free a program table */
void free_Program_table(Program_Table* prog_table);


// USEFULL FUNCTIONS //

/* Verify if a symbol with the same ident is already in the table 
 * Return 1 if the symbol is already in the table, 0 otherwise
*/
int isPresent(Symbols_Table* sym_table, char* symbol);

int isPresent_all(Program_Table * table, Node * node);

/* Return the size (in bytes) based on the type */
int determine_size(Type type);

/* Return the string corresponding to the type */
char* type_to_string(Type type);

/* Return the last adress of the symbol table */
int get_last_adress(Symbols_Table* sym_table);


// CORE FUNCTIONS //

/* Add a symbol to the symbol table 
 * we suppose that the table as enough left for the symbol
 * Return 1 if a problem occured, 0 otherwise
*/
int add_symbol(Symbols_Table* sym_table, Symbol symbol);

/* Add symbols to a symbol table 
 * Returns 1 if a problem occured, 0 otherwise
*/
int add_Symbols_to_Table(Node *node, Symbols_Table * table);

/* Add function to a function table
 * Returns 1 if a problem occured, 0 otherwise
*/
int add_Function(Node *node, Function_Table * table);

/* Create the symbol table of a tree 
 * Returns 1 if a problem occured, 0 otherwise
*/
int treeToSymbol(Node *node, Program_Table * table);


// DISPLAY IN TERMINAL //

/* Print a symbol */
void print_symbol(Symbol* symbol);

/* Print a symbol table */
void print_sym_table(Symbols_Table* sym_table);

/* Print a function table */
void print_func_table(Function_Table* func_table);

/* Print a program table */
void print_program_table(Program_Table* prog_table);


#endif