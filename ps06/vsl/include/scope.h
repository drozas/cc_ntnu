#ifndef SCOPE_H
#define SCOPE_H

void init_scopes ( int size );
void destroy_scopes ( void );

void push_scope ( char *text );
void push_nameless_scope ( void );
void pop_scope ( void );

char *generate_scope_string ( int depth );
char *augmented_scope_string ( int depth, char *text );

#endif
