#include <stdlib.h>
#include <search.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include "tree.h"
#include "scope.h"


extern uint32_t scope_depth;

/*
 * Traverse the syntax tree, outputting the current scope/name for each
 * variable and function
 */


void
find_symbols ( node_t *root )
{
	int i;
	if (root!=NULL)
	{
		
			if (strcmp(type_lookup[root->type],"VARIABLE_N")==0)
			{

				push_scope(root->data);
				printf("%s\n", generate_scope_string(scope_depth), scope_depth);
				pop_scope();
			}
			else if (strcmp(type_lookup[root->type],"FUNCTION_N")==0)
			{

				push_scope(root->children[0]->data);
				printf("%s\n", generate_scope_string(scope_depth),scope_depth);

				for (i=1; i<root->n_children; i++)
					find_symbols(root->children[i]);
				
				pop_scope();
			
			}else if (strcmp(type_lookup[root->type],"BLOCK_N")==0)
			{
				push_nameless_scope();
				printf("%s\n", generate_scope_string(scope_depth), scope_depth);
				for (i=0; i<root->n_children; i++)
					find_symbols(root->children[i]);
				
				pop_scope();
			}else{
				
				for (i=0; i<root->n_children; i++)
					find_symbols(root->children[i]);
			}



	}

	
}
