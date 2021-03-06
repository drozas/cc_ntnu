#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "tree.h"

int yyparse ( void );
node_t *root;


int
main ( int argc, char **argv )
{

	printf("main: creating the tree structure...");
   	yyparse ();
	//root_aux = root;
	printf("ok\n");
	
	FILE *pre = fopen ( "pre.tree", "w" ),
        *post = fopen ( "post.tree", "w" );

	printf("main: printing pre.tree...");	
	print_node ( pre, root, 0 );
	printf("ok\n");

	printf("main: simplifying the tree...");
    	root = simplify_tree ( root );
	printf("This function does not work properly: please, see the source code\n");

	printf("main: printing post.tree...");	
	print_node ( post, root, 0 );
	printf("ok\n");

    	fclose ( pre );
    	fclose ( post );

	printf("main: destroying the tree structure...");
    	destroy_subtree ( root);
	printf("ok\n");

    	exit ( EXIT_SUCCESS );
}
