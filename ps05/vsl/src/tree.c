#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "tree.h"


const char *type_lookup[24] = {
    "PROGRAM_N",

    "FUNCTION_LIST_N", "STATEMENT_LIST_N", "PRINT_LIST_N", "EXPRESSION_LIST_N",
    "VARIABLE_LIST_N", "ARGUMENT_LIST_N", "PARAMETER_LIST_N",
    "DECLARATION_LIST_N",

    "FUNCTION_N",

    "STATEMENT_N", "BLOCK_N", "ASSIGNMENT_STATEMENT_N", "RETURN_STATEMENT_N",
    "PRINT_STATEMENT_N", "NULL_STATEMENT_N", "IF_STATEMENT_N",
    "WHILE_STATEMENT_N",

    "PRINT_ITEM_N", "EXPRESSION_N", "DECLARATION_N", "VARIABLE_N", "INTEGER_N",
    "TEXT_N"
};


/*
 * Create a node with space for n children, but don't allocate any of them.
 * This is useful when we are manipulating the tree after it has already
 * been constructed.
 */
node_t *
allocate_node ( nodetype_t type, int n_children, void *data )
{
    node_t *result = calloc ( 1, sizeof(node_t) );
    result->type = type;
    result->n_children = n_children;
    result->data = data;
    result->entry = NULL;
    result->children = calloc ( n_children, sizeof(node_t *) );
    return result;
}


/*
 * Create a node with an arbitrary number of children; the argument count
 * is expected to correspond to the second argument. This is shorter
 * and more general than creating separate construction code for nodes
 * with every possible number of children.
 */
node_t *
create_node ( nodetype_t type, int n_children, void *data, ... )
{
    va_list argp;

    node_t *result = allocate_node ( type, n_children, data );
    va_start ( argp, data );
    for ( int i=0; i<n_children; i++ )
        result->children[i] = va_arg ( argp, node_t * );
    va_end ( argp );
    return result;
}


/*
 * Destroy a single node, but leave the subtrees below it alone.
 * Again, this is useful when changing the tree without wanting
 * to blow it all away.
 */
void
destroy_node ( node_t *discard )
{
    free ( discard->children );
    free ( discard->data );
    free ( discard );
}


/*
 * Destroy a subtree, getting rid of all children recursively.
 * This lets us throw away the entire tree at the end of the program.
 */
void
destroy_subtree ( node_t *discard )
{
    if ( discard != NULL )
    {
        for ( int i=0; i<discard->n_children; i++ )
            destroy_subtree ( discard->children[i] );
        destroy_node ( discard );
    }
}


void
print_node ( FILE *output, node_t *root, int nesting )
{
    for ( int i=0; i<nesting && fputc ( ' ', output ); i++ );
    if ( root == NULL && fprintf ( output, "<empty>\n" ) )
        return;

    switch ( root->type )
    {
        case VARIABLE_N:
            fprintf ( output, "VAR(%s)\n", (char *)root->data );
            break;
        case INTEGER_N:
            fprintf ( output, "INT(%d)\n", *((int *)root->data) );
            break;
        case TEXT_N:
            fprintf ( output, "TEXT(%s)\n", (char *)root->data );
            break;
        default:
            fprintf ( output, "%s, %d c\n", 
                type_lookup[root->type], root->n_children
            );
            break;
    }
    for ( int i=0; i<root->n_children; i++ )
        print_node ( output, root->children[i], nesting+1 );
}


/*
 * Simplify the tree after its construction.
 * Some nodes exist only because the grammar needed to group some nodes
 * into categories (syntactic information), and some nodes exist because
 * their meaning may already be clear (semantic information).
 * 
 * Since we are done with the syntactic analysis (parsing), the syntactic
 * information is no longer valuable, and can be thrown away.
 * Some of the information we no longer need is encoded in
 * - statement nodes (just a container for various kinds of statements)
 * - recursive list structures (we only need their elements now)
 * - expression nodes which only contain a single variable or constant
 * - terms to be grouped due to parentheses have already been grouped
 *   by the parser - the tree tells where the parentheses must have been,
 *   so we don't need to remember them
 *
 * The program semantics (i.e. what it means) will almost never be clear
 * before it has been completed and run. Some things, however, we already
 * know: constant literals will not change their value, and by extension,
 * neither will expressions which involve only constants.
 * This gives us two cases to simplify:
 * - evaluate negative constants, eliminating the unary minus node
 * - evaluate expressions with only constant terms, substituting the 
 *   value for the expression subtree.
 */
node_t *
simplify_tree ( node_t *root )
{
    int n;
    node_t *candidate, **c;
    void *data;

    if ( root == NULL ) // Stop right away if this is an empty node
        return NULL;

    // Recur before handling the present node, bottom-up traversal
    for ( int i=0; i<root->n_children; i++ )
        root->children[i] = simplify_tree ( root->children[i] );

    // The tree below is now simplified, so we can look at our candidate
    n         = root->n_children;
    candidate = root;
    c         = root->children;
    data      = root->data;

    // This selects any appropriate transformation of the candidate
    switch ( root->type )
    {
        case STATEMENT_N :  // This is a statement node - we don't need it
            root = c[0];
            break;

        case EXPRESSION_N : // This is an expression: do we know the value?

            // Parentheses and 1-variable expressions are now junk
            if ( n==1 && (c[0]->type==EXPRESSION_N || c[0]->type==VARIABLE_N) )
                root = c[0];

            // Integer constants don't need expressions to hold them
            else if ( n==1 && c[0]->type==INTEGER_N )
            {
                root = c[0];
                // NB! Preserve semantics for negative constants
                if ( data != NULL && strcmp ( data, "-" ) == 0 )
                    *((int *)root->data) *= -1;
            }

            // Binary operations with constant terms can be rewritten
            else if ( n==2 && c[0]->type==INTEGER_N && c[1]->type==INTEGER_N )
            {
                // Copy out the expression data
                char binop = ((char *)candidate->data)[0];
                int a = *((int *)c[0]->data), b = *((int *)c[1]->data);

                // Make a new node, get rid of the old children
                root = create_node ( INTEGER_N, 0, calloc ( 1, sizeof(int) ) );
                destroy_node ( c[0] );
                destroy_node ( c[1] );

                // Evaluate the expression, reducing it to a constant
                switch ( binop )
                {
                    case '+' : *((int *)root->data) = a + b; break;
                    case '-' : *((int *)root->data) = a - b; break;
                    case '*' : *((int *)root->data) = a * b; break;
                    case '/' : *((int *)root->data) = a / b; break;

                    default :   // This should never happen
                        fprintf ( stderr, "Unknown binary operator\n" );
                        break;
                }
            }
            break;

        /* Flatten lists:
         *
         * As per the grammar, the general structure of these lists is
         * that child 0 points to the rest of the list, while child 1
         * holds a value for the present position. The last node has
         * one child, which is the last value.
         * 
         * The only exception to this is declaration-lists. Due to the fact
         * that they may be empty, they end quite abruptly in a NULL list
         * without any 1-child container for the last declaration.
         *
         * We need a simplification proceeding as
         * 1. node(value) --> node (value) --> node (last) --> NULL
         * 2. node(value) --> node (value,last)
         * 3. node(value,value,last)
         */
        case FUNCTION_LIST_N :
        case STATEMENT_LIST_N :
        case PRINT_LIST_N :
        case EXPRESSION_LIST_N :
        case VARIABLE_LIST_N :
        case DECLARATION_LIST_N:
            /*
             * This first condition is to handle declaration lists:
             * if the further list is NULL, we adopt the last element
             * as the final node, bringing our list into the same regular
             * pattern as the other types of lists
             */
            if ( c[0] == NULL )
                root = c[1];

            /*
             * The other case handles the bottom-up flattening of all the
             * (now) regular list types. We will work two levels ahead in
             * the tree, which avoids the need for a special case on the
             * last node, where there is only one child.
             */
            else if ( n>1 )
            {
                // Count the children of my child
                int grandchildren = c[0]->n_children;

                // Make new node with space for both child and grandchildren
                root = allocate_node ( root->type, grandchildren+1, NULL );

                // Adopt all the grandchildren
                for ( int i=0; i<grandchildren; i++ )
                    root->children[i] = c[0]->children[i];

                // Add my own child, and get rid of the extra node
                root->children[grandchildren] = c[1];
                destroy_node ( c[0] );
            }
            break;

        /*
         * Argument and parameter lists are just potentially empty versions
         * of expression_list and variable_list, respectively. They will
         * create at most one node. As the empty case is already
         * covered above, we can just eliminate the singletons.
         */
        case ARGUMENT_LIST_N :
        case PARAMETER_LIST_N :
            root = c[0];
            break;
    }

    // If node has been altered above, the old copy is redundant
    if ( candidate != root )
        destroy_node ( candidate );
    return root;
}
