#ifndef SCOPE_H
#define SCOPE_H
#include <inttypes.h>

void init_scopes ( uint32_t size );
void destroy_scopes ( void );

void push_scope ( char *text );
void push_nameless_scope ( void );
void pop_scope ( void );

char *generate_scope_string ( uint32_t depth );
char *augmented_scope_string ( uint32_t depth, char *text );

#endif
