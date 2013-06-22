#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "scope.h"


static const char *separator = "_";
static uint32_t counter = 0;
static uint32_t scope_limit = 0;

uint32_t scope_depth = 0;
char **scope_stack;


/*
 * Allocate space for a stack of at most s dynamically allocated strings
 */
void
init_scopes ( uint32_t s )
{
}


/*
 * Free both the memory of the contents and of the stack itself
 */
void
destroy_scopes ( void )
{
}


/*
 * Add a string to the stack.
 * If the stack overruns, expand it by reallocation.
 */
void
push_scope ( char *text )
{
}


/*
 * Push a uniquely numbered string
 */
void
push_nameless_scope ( void )
{
}


/*
 * Remove the top string of the stack, freeing its memory
 */
void
pop_scope ( void )
{
}


/*
 * Retrieve a dynamically allocated copy of the stack contents,
 * concatenated and separated with the 'separator' constant.
 */
char *
generate_scope_string ( uint32_t depth )
{
    return NULL;
}


/*
 * Retrieve a dynamic copy as for generate_scope_string, with an
 * additional item added from the parameter 'text'.
 */
char *
augmented_scope_string ( uint32_t depth, char *text )
{
    return NULL;
}
