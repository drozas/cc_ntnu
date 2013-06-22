#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scope.h"


static const char *separator = "_";
static unsigned int counter = 0;
static unsigned int scope_limit = 0;

unsigned int scope_depth = 0;
char **scope_stack;


void
init_scopes ( int s )
{
    scope_stack = malloc ( s *sizeof(char**) );
    scope_limit = s;
}


void
destroy_scopes ( void )
{
    for ( int i=0; i<scope_depth; i++ )
        free ( scope_stack[i] );
    free ( scope_stack );
}


void
push_scope ( char *text )
{
    if ( scope_depth == scope_limit )
    {
        scope_limit += 16;
        scope_stack = realloc ( scope_stack, scope_limit * sizeof(char **) );
    }
    scope_stack[scope_depth] = strdup ( text );
    scope_depth++;
}


void
push_nameless_scope ( void )
{
    char unique[11];  // There are at most 10 digits in 32-bit uint
    counter++;
    sprintf ( unique, "s%d", counter );
    push_scope ( unique );
}


void
pop_scope ( void )
{
    if ( scope_depth > 0 )
    {
        scope_depth--;
        free ( scope_stack[scope_depth] );
    }
}


char *
generate_scope_string ( int depth )
{
    int len = 0;
    for ( int i=0; i<depth; i++ )
        len += strlen ( scope_stack[i] ) + 1;

    char *result = calloc ( (len+1), sizeof(char) );
    for ( int i=0; i<depth; i++ )
    {
        strcat ( result, separator );
        strcat ( result, scope_stack[i] );
    }
    return result;
}


char *
augmented_scope_string ( int depth, char *text )
{
    char *result = generate_scope_string ( depth );
    int slen = strlen(result), tlen = strlen(text);
    result = realloc ( result, slen+tlen+2 * sizeof(char) );
    strcat ( result, separator );
    strcat ( result, text );
    return result;
}
