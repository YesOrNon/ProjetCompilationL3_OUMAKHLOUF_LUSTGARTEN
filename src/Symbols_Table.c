/* Exercice 2 */
#include <stdio.h>
#include <stdlib.h>
#include "../include/Symbols_Table.h"
#include "../try.h"
#include <stdbool.h>

int err_line = 0;


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

Type get_return_type(Node *node, Function_Table * table, Program_Table * program, Type wanted, Type *last) {
    if (node->label == _return) {
        if (FIRSTCHILD(node))
            *last = expr_type(program, table, FIRSTCHILD(node), 0);
        else
            *last = VOID_;
    }
    for (Node *child = FIRSTCHILD(node); child != NULL && table->type_ret != VOID_; child = child->nextSibling) {
        err_line = child->lineno;
        if (*last != DEFAULT) {
            if (*last == VOID_) {
                fprintf(stderr, "Line %d -> Semantic Error : Function \"%s\" returns a value of type \"void\"\n", err_line, table->ident);
                return DEFAULT;
            }
            if ((wanted == INT || wanted == CHAR) && (*last != INT && *last != CHAR)) {
                return DEFAULT;
            }
        }
        get_return_type(child, table, program, wanted, last);
    }
    return *last == VOID_ ? DEFAULT : *last;
}

Type find_Symbol_type(Symbols_Table * sym_table, char * ident) {
    for (int i = 0; i < sym_table->index; i++) {
        if (strcmp(sym_table->tab[i].ident, ident) == 0) {
            return sym_table->tab[i].type;
        }
    }
    return DEFAULT;
}

Symbol * find_Symbol(Symbols_Table * sym_table, char * ident) {
    for (int i = 0; i < sym_table->index; i++) {
        if (strcmp(sym_table->tab[i].ident, ident) == 0) {
            return &sym_table->tab[i];
        }
    }
    return NULL;
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
    globals = isPresent(table->globals, node->data.ident);
    err_line = node->lineno;    
    if (func != NULL) {
        while (func->next != NULL) {
            func = func->next;
        }
        header = isPresent(func->header, node->data.ident);
        body = isPresent(func->body, node->data.ident);
    }
    return  body || header || globals;
}

int check_name_conflict(Symbols_Table *local_vars_table, Symbols_Table *param_table) {
    for (int i = 0; i < local_vars_table->index; i++) {
        if (isPresent(param_table, local_vars_table->tab[i].ident)) {
            fprintf(stderr, "Line %d -> Semantic Error : identifier \"%s\" already exists in parameters\n", err_line, local_vars_table->tab[i].ident);
            return 1;
        }
    }
    return 0;
}

Function_Table * get_function(Program_Table* program, char *ident) {
    Function_Table* temp = program->functions;
    if (temp != NULL) {
        if (strcmp(temp->ident, ident) == 0) return temp;
        while(temp->next != NULL) {
            if (strcmp(temp->ident, ident) == 0) return temp;
            temp = temp->next;
        }
    }
    return NULL;
}

int count_args(Node * node, Program_Table * program, Function_Table * function, Function_Table * used_from) {
    int i = 0; Type type;
    Node * child = FIRSTCHILD(node);
    err_line = node->lineno;
    while (child != NULL)   {
        type = expr_type(program, used_from, child, 0);
        if (type != function->header->tab[i].type) {
            fprintf(stderr, "Line %d -> Semantic Error : Type of the argument \"%s\" in function \"%s\"\n", err_line, child->data.ident, function->ident);
            fprintf(stderr, "Line %d -> Expected : %s, Actual : %s\n", err_line, type_to_string(function->header->tab[i].type), type_to_string(type));
            return -1;
        }
        child = child->nextSibling;
        i++;
        }
    return i;
}

int function_parameters(Function_Table * table, int count) {
    if (count == -1) return 0;
    int signature = table->header->index;
    int message = signature - count;
    if (message > 0) 
        fprintf(stderr, "Line %d -> Semantic Error : Missing %d arguments in the function \"%s\"\n", err_line, message, table->ident);
    if (message < 0)
        fprintf(stderr, "Line %d -> Semantic Error : Too many arguments in the function \"%s\"\n", err_line, table->ident);
    return signature == count;
}

Function_Table * get_last_function(Function_Table * func) {
        if (func != NULL) {
        while (func->next != NULL) {
            func = func->next;
        }
    }
    return func;
}

int determine_size(Type type) {
    switch (type) {
        case INT: return 4;
        case CHAR: return 1;
        case VOID_: return 0;
        case FUNCTION: return 0;
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

Type process_ident_expr_type(Program_Table* program, Function_Table* table, Node * node, int Lvalue) {
    Type left, right;
    Function_Table * tmp;
    if (Lvalue) {   // Lvalue
        printf("%s = ", node->data.ident);
        left = find_Symbol_type(table->body, node->data.ident);
        if (left == DEFAULT) left = find_Symbol_type(table->header, node->data.ident);
        if (left == DEFAULT) left = find_Symbol_type(program->globals, node->data.ident);
        right = expr_type(program, table, node->nextSibling, 0);
        if (left == CHAR && right == INT) {
            fprintf(stderr, "\nLine %d -> Warning : Operation between CHAR and INT variables\n", err_line);
            return INT;
        }
        else if (left == INT && right == CHAR)  return INT;
        else if (left == INT && right == INT)   return INT;
        else if (left == CHAR && right == CHAR) return CHAR;
        else if (right == VOID_)                return VOID_;
        else return DEFAULT;
    }
    else printf("%s ", node->data.ident);   // Rvalue
    if (strcmp(node->data.ident, "getint") == 0) return INT;
    else if (strcmp(node->data.ident, "getchar") == 0) return CHAR;
    else if (strcmp(node->data.ident, "putint") == 0) return VOID_;
    else if (strcmp(node->data.ident, "putchar") == 0) return VOID_;
    else if ((FIRSTCHILD(node) && FIRSTCHILD(node)->label == args) || (node->nextSibling && node->nextSibling->label == args)){  // Function call
        printf("( ");
        Symbol * symbol = find_Symbol(program->globals, node->data.ident);
        if (symbol == NULL || symbol->type != FUNCTION) {
            fprintf(stderr, "Line %d -> Semantic Error : Function \"%s\" is not defined\n", err_line, node->data.ident);
            return DEFAULT;
        }
        tmp = get_function(program, node->data.ident);
        right = tmp->type_ret;
        if (FIRSTCHILD(node) && !function_parameters(tmp, count_args(node->firstChild, program, tmp, table))) right = DEFAULT;
        else if (node->nextSibling && !function_parameters(tmp, count_args(node->nextSibling, program, tmp, table))) right = DEFAULT;
        printf(") ");
        return right;
    }
    else {
        right = find_Symbol_type(table->body, node->data.ident);
        if (right == DEFAULT) right = find_Symbol_type(table->header, node->data.ident);
        if (right == DEFAULT) right = find_Symbol_type(program->globals, node->data.ident);
    }
    return right;
}


// CORE FUNCTIONS //

Type expr_type(Program_Table* program, Function_Table* table, Node * node, int Lvalue) {
    Type left, right;
    Symbol * symbol;
    err_line = node->lineno;
    switch (node->label) {

    case ident:
        return process_ident_expr_type(program, table, node, Lvalue);

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
            fprintf(stderr, "\nLine -> %d Warning : Operation between CHAR and INT variables\n", err_line);
        return INT;

    case divstar:
        left = expr_type(program, table, FIRSTCHILD(node), 0);
        printf("%c ", node->data.byte);
        right = expr_type(program, table, SECONDCHILD(node), 0);
        if ((left == INT) && (right == INT)) return INT;
        else return DEFAULT;

    case charac:
        printf("%s ", node->data.ident);
        return CHAR;

    case ident_tab:
        symbol = find_Symbol(table->body, node->data.ident);
        if (symbol == NULL) symbol = find_Symbol(table->header, node->data.ident);
        if (symbol == NULL) symbol = find_Symbol(program->globals, node->data.ident);
        if (symbol == NULL) {
            fprintf(stderr, "Line %d -> Semantic Error : Identifier \"%s\" is not defined\n", err_line, node->data.ident);
            return DEFAULT;
        }
        if (Lvalue) {
            printf("%s[ ", node->data.ident);
            expr_type(program, table, FIRSTCHILD(node), 0);
            printf("] = ");
            return expr_type(program, table, node->nextSibling, 0);
        }
        printf("%s[ ", node->data.ident);
        expr_type(program, table, FIRSTCHILD(node), 0);
        printf("]");
        return INT;

    case or:
        left = expr_type(program, table, FIRSTCHILD(node), 0);
        printf("|| ");
        right = expr_type(program, table, SECONDCHILD(node), 0);
        return INT;
    case and:
        left = expr_type(program, table, FIRSTCHILD(node), 0);
        printf("&& ");
        right = expr_type(program, table, SECONDCHILD(node), 0);
        return INT;
    
    case order:
    case eq:
        left = expr_type(program, table, FIRSTCHILD(node), 0);
        printf("%s ", node->data.ident);
        right = expr_type(program, table, SECONDCHILD(node), 0);
        if ((left == CHAR && right == INT) || (left == INT && right == CHAR))
            fprintf(stderr, "\nLine %d -> Warning : Operation between CHAR and INT variables\n", err_line);
        return INT;

    case exclamation:
        printf("!");
        return expr_type(program, table, FIRSTCHILD(node), 0);

    default:
        printf("DEFAULT : %s\n", node->data.ident);
        return DEFAULT; 
    }
}

int add_symbol(Symbols_Table* sym_table, Symbol symbol) {
    if (isPresent(sym_table, symbol.ident)) {
        fprintf(stderr, "Line %d -> Semantic Error : identifier \"%s\" already exists\n", err_line, symbol.ident);
        return 1;
    }
    symbol.deplct = get_last_adress(sym_table) + symbol.size;
    sym_table->tab[sym_table->index++] = symbol;
    return 0;
}

int add_Symbols_to_Table(Node *node, Symbols_Table * table){
    if (node->label == type) {
        if (FIRSTCHILD(node)->label == ident_tab) {
            err_line = FIRSTCHILD(node)->lineno;
            Symbol tab = make_symbol(FIRSTCHILD(node)->data.ident, INT);
            if (FIRSTCHILD(FIRSTCHILD(node))) { // Array with index
                tab.size *= FIRSTCHILD(FIRSTCHILD(node))->data.num;
                if (tab.size == 0) {
                    fprintf(stderr, "Line %d -> Semantic Error : Array size must be greater than 0\n", err_line);
                    return 1;
                }
            }
            if (add_symbol(table, tab)) return 1;
        }
        else {
            err_line = FIRSTCHILD(node)->lineno;
            if (add_symbol(table, make_symbol(FIRSTCHILD(node)->data.ident, string_to_type(node->data.comp))))  return 1;
            if (FIRSTCHILD(node)->nextSibling) {
                Node * sibling = FIRSTCHILD(node)->nextSibling;
                while(sibling) {
                    if (add_symbol(table, make_symbol(sibling->data.ident, string_to_type(node->data.comp)))) return 1;
                    sibling = sibling->nextSibling;
                }
            }
        }
    }
    for (Node *child = FIRSTCHILD(node); child != NULL; child = child->nextSibling) {
        err_line = child->lineno;
        if (add_Symbols_to_Table(child, table)) return 1;
    }
    return 0;
}

int add_Function(Node *node, Function_Table * table, Program_Table * program){
    int error = 0;
    err_line = node->lineno;
    Node * header = FIRSTCHILD(node);
    Node * type = FIRSTCHILD(header);
    Node * ident = SECONDCHILD(header);
    Node * param = THIRDCHILD(header);
    table->type_ret = string_to_type(type->data.comp);
    table->ident = ident->data.ident;
    if (add_Symbols_to_Table(param, table->header))  return 1;
    Node * body = SECONDCHILD(node);
    if (add_Symbols_to_Table(FIRSTCHILD(body), table->body)) return 1;
    if (check_name_conflict(table->body, table->header)) {fprintf(stderr, "of the function : %s\n", ident->data.ident); return 1;}
    printf("\nFunction : %s\n", ident->data.ident);
    Type last = DEFAULT; Type *last_ptr = &last;
    Type ret = get_return_type(body, table, program, table->type_ret, last_ptr);
    printf("\n\tReturn type : %s\n", type_to_string(ret));
    if ((strcmp(ident->data.ident, "main") == 0) && (table->type_ret != INT || (ret != INT && ret != CHAR))) {
        fprintf(stderr, "Line %d ->Semantic Error : \"main\" function must have the type \"int\" and return an \"int\"\n", err_line);
        fprintf(stderr, "Actual : %s\n", type_to_string(table->type_ret));
        fprintf(stderr, "Type of the value returned : %s\n", type_to_string(ret));
        error++;
    }
    else if (ret == INT && table->type_ret == CHAR)
        fprintf(stderr, "Line %d -> Warning : Function \"%s\" has the type \"char\" and returns an \"int\"\n", err_line, ident->data.ident);
    else if (ret == CHAR && table->type_ret == INT)
        fprintf(stderr, "Line %d -> Warning : Function \"%s\" has the type \"int\" and returns a \"char\"\n", err_line, ident->data.ident);
    else if (ret == DEFAULT && table->type_ret != VOID_) {
        fprintf(stderr, "Line %d -> Semantic Error : Function \"%s\" does not return a value\n", err_line, ident->data.ident);
        error++;
    }
    return error;
}

int treeToSymbol(Node *node, Program_Table * table) {
    Type type;
    Function_Table* func = table->functions;
    switch (node->label) {
        case program:
            err_line = node->lineno;
            if (add_Symbols_to_Table(FIRSTCHILD(node), table->globals)) return 1;
            break;
        case fonction:
            if (!table->functions){
                table->functions = init_Func_table();
                if (add_Function(node, table->functions, table))   return 1;
            }
            else{
                Function_Table * tmp;
                for (tmp = table->functions; tmp->next != NULL; tmp = tmp->next) {}
                tmp->next = init_Func_table();
                if (add_Function(node, tmp->next, table)) return 1;
            }
            if (add_symbol(table->globals, make_symbol(SECONDCHILD(FIRSTCHILD(node))->data.ident, FUNCTION)))   return 1;
            break;
        case ident:
            err_line = node->lineno;
            if (!isPresent_all(table, node)) {
                if (strcmp(node->data.ident, "getint") == 0);
                else if (strcmp(node->data.ident, "getchar") == 0);
                else if (strcmp(node->data.ident, "putchar") == 0);
                else if (strcmp(node->data.ident, "putint") == 0);
                else    {fprintf(stderr, "Line %d -> Semantic Error : \"%s\" is not defined\n", err_line, node->data.ident); return 1;}
            }
            break;
        case affectation:
            err_line = node->lineno;
            printf("\naffectation : ");
            type = expr_type(table, get_last_function(func), FIRSTCHILD(node), 1);
            printf("\n\tType : %s\n", type_to_string(type));
            if (type == DEFAULT) {
                fprintf(stderr, "\nLine %d -> Semantic Error : Type of Expr\n", err_line);
                return 1;
            }
            else if (type == VOID_) {
                fprintf(stderr, "Line %d -> Semantic Error : A function of type \"void\" is used as an Rvalue\n", err_line);
                return 1;
            }
            break;
        case IDENTs:
            printf("IDENTS : %s\n", node->firstChild->data.ident);
            type = expr_type(table, get_last_function(func), FIRSTCHILD(node), 0);
            printf("\n\tType : %s\n", type_to_string(type));
            if (type == DEFAULT) {
                fprintf(stderr, "\nSemantic Error : Type of Expr\n");
                return 1;
            }
            break;
        case _if:
            printf("\nif ( ");
            type = expr_type(table, get_last_function(func), FIRSTCHILD(node), 0);
            printf(") \n");
            printf("\tIF type : %s\n", type_to_string(type));
            break;
        case _else:           
            break;
        case _while:
            printf("\nwhile ( ");
            type = expr_type(table, get_last_function(func), FIRSTCHILD(node), 0);
            printf(") \n");
            printf("\tWHILE type : %s\n", type_to_string(type));
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

