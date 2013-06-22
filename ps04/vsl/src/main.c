#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "symtab.h"
#include "scope.h"
#include "tree.h"

int yyparse ( void );
node_t *root;


void
read_args ( int argc, char **argv )
{
    int option;
    while (  (option = getopt ( argc, argv, "f:" )) != -1 )
        switch ( option )
        {
            case 'f':
                if ( freopen ( optarg, "r", stdin ) == 0 )
                {
                    fprintf ( stderr, "Could not open file '%s'\n", optarg );
                    exit ( EXIT_FAILURE );
                }
                break;
        }
}

int
main ( int argc, char **argv )
{
    read_args ( argc, argv );
    yyparse ();

#ifdef DUMPTREES
    FILE *pre = fopen ( "pre.tree", "w" ),
        *post = fopen ( "post.tree", "w" );
    print_node ( pre, root, 0 );
#endif

    root = simplify_tree ( root );

#ifdef DUMPTREES
    print_node ( post, root, 0 );
    fclose ( pre );
    fclose ( post );
#endif

    init_scopes ( 256 );

    find_symbols ( root );

    destroy_scopes ();
    destroy_subtree ( root );
    exit ( EXIT_SUCCESS );
}
