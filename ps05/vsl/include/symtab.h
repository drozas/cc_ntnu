#ifndef SYMTAB_H
#define SYMTAB_H

#define TABLE_SIZE 4096

typedef enum {
    VARIABLE_S, PARAMETER_S, CONSTANT_S, FUNCTION_S
} symtype_t;

typedef struct {
    symtype_t type;
    int stack_offset;
    char *label;
} symtab_t;

void create_symtab ( void );
void destroy_symtab ( void );
void enter_symbol ( char *key, symtab_t *value );
symtab_t *lookup_symbol ( char * key );

#endif
