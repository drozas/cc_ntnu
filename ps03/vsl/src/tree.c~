#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "tree.h"

extern node_t *root;
extern node_t *root2;

/*
 * This is a constant table which reverses the mapping of nodetype_t,
 * finding symbolic names from the node type code.
 */
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
	//printf("llamada a allocate_node de tipo %s con %d hijos \n", type_lookup[type], n_children);
	node_t *result = calloc(1, sizeof(node_t));
	result->type = type;
	result->n_children = n_children;
	result->data = data;

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

	va_list list;
	//list initialization
	va_start(list, data);

	node_t *padre= NULL;

	padre = allocate_node(type,n_children, data);
	padre->children = calloc(1, sizeof(node_t*));
	
	int i = 0;
	for (i=0; i<n_children; i++)
	{

		int p = (int) va_arg(list, void *);

		//Pointing the children from the parent
		padre->children[i] = calloc(1, sizeof(node_t));
		padre->children[i]= (node_t*)p;
		p++;
	}
	
	return padre;
}


/*
 * Destroy a single node, but leave the subtrees below it alone.
 * Again, this is useful when changing the tree without wanting
 * to blow it all away.
 */
void
destroy_node ( node_t *discard )
{
	free(discard);
	//we have to free 'discard->children' also (this does not affect the children themselves, just the list of pointers to them).
	free(discard->children);
}


/*
 * Destroy a subtree, getting rid of all children recursively.
 * This lets us throw away the entire tree at the end of the program.
 */
void
destroy_subtree ( node_t *discard )
{

	if(discard!=NULL)
	{
		int i = 0;
		for (i=0; i<discard->n_children; i++)
			if (discard->children[i] != NULL)
				destroy_subtree(discard->children[i]);

		destroy_node(discard);
	}
}




/*
 *  Recursively print the subtree rooted at a node, showing the
 *  tree structure by indentation. We can use the text labels from
 *  the type_lookup table, mapping nodetype_t enum values to their
 *  grammatical names.
 */
void
print_node ( FILE *output, node_t *root, int nesting )
{
	if (root!=NULL)
	{
		int i = 0;
		
		int j=0;
		for(j=0; j<nesting; j++)
			fprintf(output, " ");

		fprintf(output,"%d:%s",nesting, type_lookup[root->type]);

		if (root->data != NULL)
		{
			//We have to diference the integer print
			if (strcmp(type_lookup[root->type],"INTEGER_N")==0)
				fprintf(output, ": %d", *(int*)(root->data));
			else
				fprintf(output, ": %s", root->data);
		}
		fprintf(output,"\n");

		//Print the children recursively
		for (i=0; i<root->n_children; i++)
			print_node(output, root->children[i], nesting+1);



	}

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

/*	drozas: --------IMPORTANT!!!: THIS FUNCTION DOES NOT WORK PROPERLY --------------- 
	Idea: remove the single child nodes statement, argument_list and parameter_list by point from the parent to the grandson and
		making free the child
*/
node_t *
simplify_tree ( node_t *root )
{
/*    	if (root!=NULL)
	{

		//Simplfiying single child: statement, argument_list and parameter_list	
		if ((strcmp(type_lookup[root->type],"STATEMENT_LIST_N")==0) && (root->n_children==1))
		{
			printf("\n***simplify_tree: we are going to simplify a statement\n");
			root->children = root->children[0]->children;
			free(root->children[0]);
			
		}else if(strcmp(type_lookup[root->type],"FUNCTION_N")==0)
		{
			printf("\n***simplify_tree: we are going to simplify a parameter_list\n");
			root->children = root->children[1]->children;
			free(root->children[1]);
		}else if( 	(strcmp(type_lookup[root->type],"EXPRESSION_N")==0) && 
				(root->n_children==2) &&
				(strcmp(type_lookup[root->children[0]->type],"VARIABLE_N")==0)	
										)
		{
			printf("\n***simplify_tree: we are going to simplify an argument_list\n");
			root->children = root->children[1]->children;
			free(root->children[1]);

		}

		int i = 0;
		for (i=0; i<root->n_children; i++)
			simplify_tree(root->children[i]);
	}else{
		return NULL;
	}
*/
	return NULL;
}
