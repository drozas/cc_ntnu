#include <stdio.h>
#include <stdlib.h>

extern int yyparse ( void );

int
main ( int argc, char **argv )
{
    yyparse ();
    exit ( EXIT_SUCCESS );
}
