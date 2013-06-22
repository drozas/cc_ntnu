#include <stdlib.h>
#include <search.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include "tree.h"
#include "scope.h"


extern unsigned int scope_depth;


/*
 * These variables are there to log everything entered into the
 * hashtable, because as soon as the entries go in, we can only
 * retrieve them by their index. Keeping an independent table makes
 * it easy to free all allocated memory without remembering all
 * the indices. The table is initialized to NULL, because that can
 * safely be passed to free() even if no value has been set. 
 */
static symtab_t *entries[ TABLE_SIZE ];
static symtab_t **last_entry = entries;
static char *keys[ TABLE_SIZE ];
static char **last_key = keys;

void
create_symtab ( void )
{
    hcreate ( TABLE_SIZE );             // Create the hashtable
}


static void
destroy_symtab_entry ( symtab_t *discard )
{
    free ( discard->label );
    free ( discard );
}


void
destroy_symtab ( void )
{
    hdestroy ();                    // Destroy the hashtable
    while ( last_entry > entries  ) // Destroy all logged entries also
    {
        last_entry--;
        last_key--;
        destroy_symtab_entry ( *last_entry );
        free ( *last_key );
    }
}


void
enter_symbol ( char *key, symtab_t *value )
{
    ENTRY item;
    *last_key = strdup ( key ); //  Log the key and location
    *last_entry = value;
    item.key = *last_key;        // Create an item for the hashtable and
    item.data = *last_entry;     // enter it
    hsearch ( item, ENTER );
    last_entry++;               // Advance the table pointers
    last_key++;
}


symtab_t *
lookup_symbol ( char * key )
{
    ENTRY item, *found;                 // Get the entry from the hashtable
    item.key = key;
    found = hsearch ( item, FIND );
    if ( found != NULL )                // Return what was found
        return (symtab_t *)(found->data);
    else
        return NULL;
}


static void
declare_variables ( node_t *varlist, symtype_t symbol_type )
{
    char *enclosing_scope_name = generate_scope_string ( scope_depth );
    symtab_t *enclosing_scope = lookup_symbol ( enclosing_scope_name );
#ifdef DEBUG
    printf ( "Working on %s\n", enclosing_scope_name );
#endif
    free ( enclosing_scope_name );
    switch ( symbol_type )
    {
        case VARIABLE_S: case PARAMETER_S:
            for ( uint8_t i=0; i<varlist->n_children; i++ )
            {
                node_t *child = varlist->children[i];
                push_scope ( child->data );
                char *scopes = generate_scope_string ( scope_depth );
                if ( lookup_symbol ( scopes ) == NULL )
                {
                    symtab_t *entry = calloc ( 1, sizeof(symtab_t) );
                    entry->type = symbol_type;
                    entry->stack_offset =
                        enclosing_scope->stack_offset;
                    enclosing_scope->stack_offset -= 4;
                    entry->label = strdup ( scopes );
                    enter_symbol ( scopes, entry );
                    child->entry = entry;
#ifdef DEBUG
                    printf (
                        "Decl %s as %d at %d\n", 
                        scopes, symbol_type, entry->stack_offset
                    );
#endif
                }
                else
                    fprintf (
                        stderr, "Warning: '%s' multiple declaration\n", scopes
                    );
                pop_scope ();
                free ( scopes );
            }
            break;
        default:
            fprintf ( stderr, "Error: unfamiliar symbol type\n" );
            break;
    }
}


/*
 * Recursively look up variable name in nested scopes
 * node_t variable: the variable we are seeking to bind to a symtab entry
 * uint8_t unnest: how many inner scopes we have already been through
 */
static symtab_t *
search_closure ( node_t *variable, uint8_t unnest )
{
    symtab_t *result;
    if ( unnest > scope_depth )
        result = NULL;  // Basic case: no more scopes to search
    else
    {
        // Check for the variable in the present scope
        char *scopes =
            augmented_scope_string ( scope_depth-unnest, variable->data );
        result = lookup_symbol ( scopes );
        free ( scopes );

        // Recursive step: if it was not here, search on the next level
        if ( result == NULL )
            result = search_closure ( variable, unnest+1 );
    }
    return result;
}


/*
 * Link a variable to the symtab entry which describes it
 */
static void
bind_occurrence ( node_t *variable )
{
    push_scope ( variable->data );
    symtab_t *var_entry = search_closure ( variable, 1 );
    char *scopes = generate_scope_string ( scope_depth );
    if ( var_entry != NULL )
    {
#ifdef DEBUG
        printf ( " Bound %s to %s\n", variable->data, var_entry->label );
#endif
        variable->entry = var_entry;
    }
    else
    {
        fprintf ( stderr, "Error: %s not declared in scope %s\n",
            variable->data, scopes
        );
        exit ( EXIT_FAILURE );
    }
    free ( scopes );
    pop_scope ();
}


static void
declare_functions ( node_t *function_list )
{
    for ( uint8_t f=0; f<function_list->n_children; f++ )
    {
        node_t *function = function_list->children[f];
        symtab_t *entry = calloc ( 1, sizeof(symtab_t) );
        char *name = function->children[0]->data, *scopes;

        push_scope ( name );
        scopes = generate_scope_string ( scope_depth );

        entry->type = FUNCTION_S;

        /*
         * Functions with parameters need to bind their names inside
         * the caller's stack frame, i.e. at positive offsets.
         * Functions with no parameters can start their declarations
         * inside their own stack frame.
         * Functions with both parameters and declarations must skip
         * across the return base pointer just before their own stack
         * frame - this is handled in the setup for generating a block,
         * as a function which is only a statement cannot contain
         * any declarations.
         */
        if ( function->children[1] != NULL )
            entry->stack_offset = 
                4 * function->children[1]->n_children + 4;
        else
            entry->stack_offset = -4;

        entry->label = strdup ( scopes );
        enter_symbol ( scopes, entry );
        function->entry = entry;
#ifdef DEBUG
        printf ( "Declared %s\n", entry->label );
#endif

        free ( scopes );
        pop_scope ();
    }
    for ( uint8_t f=0; f<function_list->n_children; f++ )
        find_symbols ( function_list->children[f] );
}


void
find_symbols ( void *r )
{
    node_t *root = (node_t *)r;
    if ( root == NULL )
        return;


    switch ( root->type )
    {
        char *scopes, *enclosing_scope_name;
        symtab_t *enclosing_scope;

        case BLOCK_N:
            enclosing_scope_name = generate_scope_string ( scope_depth );
            enclosing_scope = lookup_symbol ( enclosing_scope_name );
            free ( enclosing_scope_name );
            push_nameless_scope ();
            scopes = generate_scope_string ( scope_depth );
            symtab_t *entry = calloc ( 1, sizeof(symtab_t) );
            entry->type = FUNCTION_S;
            entry->stack_offset = enclosing_scope->stack_offset;

            /*
             * The stack offset will be nonnegative iff this block forms
             * the body of a function (as the possibly empty parameter list
             * is down the stack from the base pointer). In this particular
             * case, we need to jump past the (return) base pointer for the
             * caller's stack frame, and start at -4 (upstack from our own
             * base pointer).
             */
            if ( entry->stack_offset >= 0 )
                entry->stack_offset = -4;

            entry->label = NULL;
            enter_symbol ( scopes, entry );
            for ( uint8_t i=0; i<root->n_children; i++ )
                find_symbols ( root->children[i] );
            pop_scope ();
            free ( scopes );
            break;

        case DECLARATION_LIST_N:
            declare_variables ( root->children[0], VARIABLE_S );
            for ( uint8_t i=1; i<root->n_children; i++ )
                find_symbols ( root->children[i] );
            break;
        case DECLARATION_N:
            declare_variables ( root->children[0], VARIABLE_S );
            break;

        case FUNCTION_LIST_N:
            declare_functions ( root );
            break;

        case FUNCTION_N:
            push_scope ( root->children[0]->data );
            if ( root->children[1] != NULL )
                declare_variables ( root->children[1], PARAMETER_S );
            find_symbols ( root->children[2] );
            pop_scope ();
            break;

        case ASSIGNMENT_STATEMENT_N:
            bind_occurrence ( root->children[0] );
            find_symbols ( root->children[1] );
            break;

        case VARIABLE_N:
            bind_occurrence ( root );
            break;

        default :
            for ( uint8_t i=0; i<root->n_children; i++ )
                find_symbols ( root->children[i] );
            break;
    }
}
