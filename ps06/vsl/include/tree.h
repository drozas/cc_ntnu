#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include "symtab.h"

/*
 * Enumerated type for describing the various types of tree nodes.
 * Each node is labelled with one of these, to identify its nature
 * when traversing the syntax tree.
 */
typedef enum {
    /* Root node: program */
    PROGRAM_N,

    /* Node types for the lists */
    FUNCTION_LIST_N, STATEMENT_LIST_N, PRINT_LIST_N, EXPRESSION_LIST_N,
    VARIABLE_LIST_N, ARGUMENT_LIST_N, PARAMETER_LIST_N, DECLARATION_LIST_N,

    /* Function declarations */
    FUNCTION_N,

    /* Statements and blocks */
    STATEMENT_N, BLOCK_N, ASSIGNMENT_STATEMENT_N, RETURN_STATEMENT_N,
    PRINT_STATEMENT_N, NULL_STATEMENT_N, IF_STATEMENT_N, WHILE_STATEMENT_N,

    /* Expressions and terminals */
    PRINT_ITEM_N, EXPRESSION_N, DECLARATION_N, VARIABLE_N, INTEGER_N, TEXT_N
} nodetype_t;


/*
 * Basic data structure for syntax tree nodes.
 * Both the label data and the list of children are consistently allocated
 * in a dynamic fashion, even if data is just a single character, integer,
 * etc., because it simplifies using a recursive traversal of the tree, both
 * for decoration, printing and destruction.
 */
typedef struct n {
    nodetype_t type;        // Type of this node
    int n_children;         // Number of children
    struct n **children;    // Pointers to child nodes
    void *data;             // Data label for terminals and expressions
    symtab_t *entry;        // Pointer to symtab entry
} node_t;

//We make here the declaration to manage the access in scope
extern const char *type_lookup[];

/*
 *  Function prototypes: implementations are found in tree.c
 */
node_t *create_node ( nodetype_t type, int n_children, void *data, ... );
node_t *allocate_node ( nodetype_t type, int n_children, void *data );
void destroy_node ( node_t *discard );
void destroy_subtree ( node_t *discard );
void print_node ( FILE *output, node_t *root, int nesting );
node_t *simplify_tree ( node_t *root );

#endif
