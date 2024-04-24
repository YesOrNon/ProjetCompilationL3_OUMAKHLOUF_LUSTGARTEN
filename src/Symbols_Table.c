/* Exercice 2 */
#include <stdio.h>
#include <stdlib.h>
#include "Symbols_Table.h"
#include "../try.h"
#include <stdbool.h>

// using try might be a bad idea, stops the program if malloc fails

// INIT //

Symbol make_symbol(char* ident, Type type) {
    Symbol symbol;
    symbol.ident = try(strdup(ident), NULL);
    symbol.type = type;
    symbol.size = determine_size(type);
    symbol.deplct = 0;
    return symbol;
}

Symbols_Table* init_Sym_table() {
    Symbols_Table* sym_table = try((Symbols_Table*)malloc(sizeof(Symbols_Table)), NULL);
    sym_table->index = 0;
    return sym_table;
}

Function_Table* init_Func_table() {
    Function_Table* func_table = try((Function_Table*)malloc(sizeof(Function_Table)), NULL);
    func_table->next = NULL;
    func_table->ident = NULL;
    func_table->type_ret = DEFAULT;
    func_table->header = init_Sym_table();
    func_table->body = init_Sym_table();
    return func_table;
}

Program_Table* init_Program_table() {
    Program_Table* prog_table = try((Program_Table*)malloc(sizeof(Program_Table)), NULL);
    prog_table->globals = init_Sym_table();
    prog_table->functions = NULL;
    return prog_table;
}


// FREE //

void free_symbol(Symbol* symbol) {
    free(symbol->ident);
}

void free_Sym_table(Symbols_Table* sym_table) {
    for (int i = 0; i < sym_table->index; i++) {
        free_symbol(&sym_table->tab[i]);
    }
    free(sym_table);
}

void free_Func_table(Function_Table* func_table) {
    free_Sym_table(func_table->header);
    free_Sym_table(func_table->body);
    free(func_table);
}

void free_Program_table(Program_Table* prog_table) {
    free_Sym_table(prog_table->globals);
    Function_Table* func = prog_table->functions;
    while (func != NULL) {
        Function_Table* next = func->next;
        free_Func_table(func);
        func = next;
    }
    free(prog_table);
}


// USEFULL FUNCTIONS //

Type find_Symbol_type(Symbols_Table * sym_table, char * ident) {
    for (int i = 0; i < sym_table->index; i++) {
        if (strcmp(sym_table->tab[i].ident, ident) == 0) {
            return sym_table->tab[i].type;
        }
    }
    return DEFAULT;
}

int isPresent(Symbols_Table* sym_table, char* ident) {
    for (int i = 0; i < sym_table->index; i++) {
        if (strcmp(sym_table->tab[i].ident, ident) == 0) {
            return 1;
        }
    }
    return 0;
}

int isPresent_all(Program_Table *table, Node* node) {
    int globals = 0, header = 0, body = 0;
    Function_Table* func = table->functions;
    // printf("CHECKING FOR IDENT : %s\n", node->data.ident);
    globals = isPresent(table->globals, node->data.ident);    
    if (func != NULL) {
        while (func->next != NULL) {
            func = func->next;
        }
        // printf("\tCHECKING FOR IDENT IN FUNCTION : %s\n", func->ident);
        header = isPresent(func->header, node->data.ident);
        body = isPresent(func->body, node->data.ident);
    }
    // if (globals)    printf("\tIn globals\n");
    // if (header)     printf("\tIn header of %s\n", func->ident);
    // if (body)       printf("\tIn body of %s\n", func->ident);
    return  body || header || globals;
}

Type expr_type(Program_Table* program, Function_Table* table, Node * node, int Lvalue) {
    Type left, right;
    switch (node->label) {
    case ident:
        if (Lvalue) {
            printf("%s = ", node->data.ident);
            left = find_Symbol_type(table->body, node->data.ident);
            if (left == DEFAULT) left = find_Symbol_type(table->header, node->data.ident);
            if (left == DEFAULT) left = find_Symbol_type(program->globals, node->data.ident);
            right = expr_type(program, table, node->nextSibling, 0);
            if ((left == CHAR && right == INT) || (left == INT && right == CHAR)) {
                fprintf(stderr, "\nWarning : Operation between CHAR and INT variables\n");
                return INT;
            }
            else if (left == INT && right == INT) return INT;
            else if (left == CHAR && right == CHAR) return CHAR;
            else return DEFAULT;
        }
        else printf("%s ", node->data.ident);
        if (strcmp(node->data.ident, "getint") == 0) {printf("GETINT"); return INT;}
        else if (strcmp(node->data.ident, "getchar") == 0) return CHAR;
        else {
            right = find_Symbol_type(table->body, node->data.ident);
            if (right == DEFAULT) left = find_Symbol_type(table->header, node->data.ident);
            if (right == DEFAULT) right = find_Symbol_type(program->globals, node->data.ident);
        }
        return right;

    case num:
        printf("%d ", node->data.num);
        return INT;

    case addsubUnaire:
        printf("%c ", node->data.byte);
        return expr_type(program, table, FIRSTCHILD(node), 0);

    case addsub:
        left = expr_type(program, table, FIRSTCHILD(node), 0);
        printf("%c ", node->data.byte);
        right = expr_type(program, table, SECONDCHILD(node), 0);
        if ((left == CHAR && right == INT) || (left == INT && right == CHAR))
            fprintf(stderr, "\nWarning : Operation between CHAR and INT variables\n");
        return INT;

    case divstar:
        left = expr_type(program, table, FIRSTCHILD(node), 0);
        printf("%c ", node->data.byte);
        right = expr_type(program, table, SECONDCHILD(node), 0);
        if (left == INT && right == INT) return INT;
        else return DEFAULT;

    case charac:
        printf("%s ", node->data.ident);
        return CHAR;

    case ident_tab:
        printf("%s[ ", node->data.ident);
        expr_type(program, table, FIRSTCHILD(node), 0);
        printf("] = ");
        return expr_type(program, table, node->nextSibling, 0);

    default:
        printf("DEFAULT : %s\n", node->data.ident);
        return DEFAULT; 
    }
}

int determine_size(Type type) {
    switch (type) {
        case INT: return 4;
        case CHAR: return 1;
        case VOID_: return 0;
        case FUNCTION: return '?';
        default: return -1;
    }
}

char* type_to_string(Type type) {
    switch (type) {
        case INT: return "INT";
        case CHAR: return "CHAR";
        case VOID_: return "VOID";
        case FUNCTION: return "FUNCTION";
        default: return "UNKNOWN";
    }
}

Type string_to_type(char * type) {
    switch (type[0]) {
        case 'i': return INT;
        case 'c': return CHAR;
        case 'v': return VOID_;
        case 'f': return FUNCTION;
        default: return DEFAULT;
    }
}

int get_last_adress(Symbols_Table* sym_table) {
    if (sym_table->index == 0) {return 0;}
    return sym_table->tab[sym_table->index - 1].deplct;
}


// CORE FUNCTIONS //

int add_symbol(Symbols_Table* sym_table, Symbol symbol) {
    if (isPresent(sym_table, symbol.ident)) {
        fprintf(stderr, "Error : identifier already exists : %s\n", symbol.ident);
        return 1;
    }
    symbol.deplct = get_last_adress(sym_table) + symbol.size;
    sym_table->tab[sym_table->index++] = symbol;
    return 0;
}

int add_Symbols_to_Table(Node *node, Symbols_Table * table){
    if (node->label == type) {
        if (add_symbol(table, make_symbol(FIRSTCHILD(node)->data.ident, string_to_type(node->data.comp))))  return 1;
        if (FIRSTCHILD(node)->nextSibling) {
            Node * sibling = FIRSTCHILD(node)->nextSibling;
            while(sibling) {
                if (add_symbol(table, make_symbol(sibling->data.ident, string_to_type(node->data.comp)))) return 1;
                sibling = sibling->nextSibling;
            }
        }
    }
    for (Node *child = FIRSTCHILD(node); child != NULL; child = child->nextSibling) {
        if (add_Symbols_to_Table(child, table)) return 1;
    }
    return 0;
}

int add_Function(Node *node, Function_Table * table){
    Node * header = FIRSTCHILD(node);
    Node * type = FIRSTCHILD(header);
    Node * ident = SECONDCHILD(header);
    Node * param = THIRDCHILD(header);
    table->type_ret = string_to_type(type->data.comp);
    table->ident = ident->data.ident;
    if (add_Symbols_to_Table(param, table->header))  return 1;
    Node * body = SECONDCHILD(node);
    if (add_Symbols_to_Table(FIRSTCHILD(body), table->body)) return 1;
    return 0;
}

int treeToSymbol(Node *node, Program_Table * table) {
    Type aff;
    Function_Table* func = table->functions;
    switch (node->label) {
        case program:
            if (add_Symbols_to_Table(FIRSTCHILD(node), table->globals)) return 1;
            break;
        case fonction:
            if (!table->functions){
                table->functions = init_Func_table();
                if (add_Function(node, table->functions))   return 1;
            }
            else{
                Function_Table * tmp;
                for (tmp = table->functions; tmp->next != NULL; tmp = tmp->next) {}
                tmp->next = init_Func_table();
                if (add_Function(node, tmp->next)) return 1;
            }
            if (add_symbol(table->globals, make_symbol(SECONDCHILD(FIRSTCHILD(node))->data.ident, FUNCTION)))   return 1;
            break;
        case ident:
            if (!isPresent_all(table, node)) {
                if (strcmp(node->data.ident, "getint") == 0);
                else if (strcmp(node->data.ident, "getchar") == 0);
                else if (strcmp(node->data.ident, "putchar") == 0);
                else if (strcmp(node->data.ident, "putint") == 0);
                else {printf("Error : %s is not defined\n", node->data.ident); return 1;}
            }
            break;
        case affectation:
            if (func != NULL) {
                while (func->next != NULL) {
                    func = func->next;
                }
            }
            printf("\naffectation : ");
            aff = expr_type(table, func, FIRSTCHILD(node), 1);
            printf("\n\tType : %s\n", type_to_string(aff));
            if (aff  == DEFAULT) {
                fprintf(stderr, "\nError : Type of Expr\n");
                return 1;
            }
            break;
        case IDENTs:
            printf("IDENTS : %s\n", node->firstChild->data.ident);
            break;
        default:
            break;
    }
    for (Node *child = FIRSTCHILD(node); child != NULL; child = child->nextSibling) {
            if (treeToSymbol(child, table)) return 1;
    }

    return 0;
}


// DISPLAY IN TERMINAL //

void print_symbol(Symbol* symbol) {
    printf("\t\tIdent: %-10s\n", symbol->ident);
    printf("\t\t\tType: %-10s\n", type_to_string(symbol->type));
    printf("\t\t\tSize: %-10d\n", symbol->size);
    printf("\t\t\tDeplct: %-10ld\n", symbol->deplct);
}

void print_sym_table(Symbols_Table* sym_table) {
    for (int i = 0; i < sym_table->index; i++) {
        print_symbol(&sym_table->tab[i]);
    }
}

void print_func_table(Function_Table* func) {
    printf("\nFunction -> ");
    printf("Type: %s | Ident: %-10s\n", type_to_string(func->type_ret), func->ident);
    printf("\tHeader:\n");
    print_sym_table(func->header);
    printf("\tBody:\n");
    print_sym_table(func->body);
}

void print_program_table(Program_Table* prog) {
    printf("\nGlobals:\n");
    print_sym_table(prog->globals);
    Function_Table* func = prog->functions;
    while (func != NULL) {
        print_func_table(func);
        func = func->next;
    }
}

