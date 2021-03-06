#include <stdlib.h>
#include <search.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include "tree.h"
#include "scope.h"


extern unsigned int scope_depth;
static const char *separator = "_";
static int STR_INITIAL_SIZE = 256;


//void find_symbols ( node_t *root );


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

char *valid_scopes[1000];
int n_valid_scopes = 0;



/*This function look in the values of the stack trying to look for the closed nested scope */
char * 
get_nested_scope(node_t *node)
{

	int i;
	int size = 1; //One, because of the "/0"  

	for(i=n_valid_scopes-1; i>=0;i--)
	{
		char *str;
	
		//We allocate for a string of STR_INITIAL_SIZE
		str = calloc(STR_INITIAL_SIZE, sizeof(char));
		
		size = size + strlen(valid_scopes[i]) + strlen(node->data) + 1; //One because of the separator
		//if the current size is bigger or equal, we have to reallocate
		if (size>=STR_INITIAL_SIZE)
			str = realloc(str, (sizeof(char))*(size));
		
		strcat(str, valid_scopes[i]);
		strcat(str, separator);
		strcat(str, node->data);
		
		
		int j;
		for(j=n_valid_scopes-1; j>=0;j--)
		{
			if (strcmp(str,valid_scopes[j])==0)
				return str;
		}
		
	}
	
	return NULL;
}


//Create the hash table
void
create_symtab ( void )
{
	int i = hcreate(TABLE_SIZE);

	if(i==0)
	{
		printf("The allocation for the hash table has failed\n");
	}
		
}


//Search and free the entry
void
destroy_symtab_entry ( symtab_t *discard )
{
	//Look up for the entry
	symtab_t* entry = lookup_symbol(discard->label);

	if(entry!=NULL)
		free(entry);
	
}

//Destroy the hash table and all its entries
void
destroy_symtab ( void )
{
	//Destroy the entries:
/*	symtab_t **current_entry, **next_entry = entries;
	char **current_key, **next_key = keys;


	while(current_entry!=last_entry && current_entry!=NULL)
	{
		next_entry = current_entry;
		next_entry++;
		
		free(current_entry);
		current_entry = next_entry;
	}

	while(current_key!=last_key && current_key!=NULL)
	{
		next_key = current_key;
		next_key++;
		
		free(current_key);
		current_key = next_key;
	}
*/	

	hdestroy();
	
}

//Add an element to the hash table
void
enter_symbol ( char *key, symtab_t *value )
{
	ENTRY aux, *aux_p;
	
	aux.key = key;
	aux.data = value;
	
	//printf("We are gonna add %s to the hash table\n", aux.key);
	aux_p = hsearch(aux, ENTER);
	
	if(aux_p==NULL)
	{
		printf("The hash table is full, it was imposible to add the new entry\n");
	}else{
		//if it is added properly, add in our log tables
		*last_entry = aux.data;
		*last_key = aux.key;
		
		last_entry++;
		last_key++;
	}
	
	
}


//Returns the pointer to the element if this exists, or NULL in any other case
symtab_t *
lookup_symbol ( char * key )
{
	//We have to create an auxiliary entry
	ENTRY aux;
	aux.key = key;
		
	return (symtab_t*) hsearch(aux, FIND);
}


void
find_symbols ( node_t *root )
{
	int i;
	if (root!=NULL)
	{
			
			if (strcmp(type_lookup[root->type],"VARIABLE_LIST_N")==0)
			{
				//printf("Scope in VARIABLE_list (n_children= %d) %s detected\n",root->n_children, generate_scope_string(scope_depth));

				int i;

				for(i=0;i<root->n_children;i++)
				{
					printf("->Adding entry key=%s, value=%s to the symbols table\n", augmented_scope_string(scope_depth,root->children[i]->data), root->children[i]->data);
					symtab_t *symtab_entry = calloc ( 1, sizeof(symtab_t) );
					symtab_entry->type = VARIABLE_S;
					symtab_entry->stack_offset = 0;
					symtab_entry->label = root->data;
					enter_symbol(augmented_scope_string(scope_depth,root->children[i]->data),symtab_entry);
				}

			}
			else if (strcmp(type_lookup[root->type],"VARIABLE_N")==0)
			{
				
				push_scope(root->data);
				//printf("%s\n", generate_scope_string(scope_depth));
				
				//Saving the scope in an auxiliar array
				valid_scopes[n_valid_scopes]= strdup(generate_scope_string(scope_depth));
				n_valid_scopes++;
				
				//Pointing the ocurrence to the symbol to the hash table
				char *nested_scope = get_nested_scope(root);
				
				if (nested_scope != NULL)
				{
					printf("-->Pointing ocurrence of :%s to symbol entry with key(scope):%s\n", root->data, nested_scope);
					root->entry = lookup_symbol(nested_scope);
				}
				
				pop_scope();
			}
			else if (strcmp(type_lookup[root->type],"FUNCTION_N")==0)
			{

				push_scope(root->children[0]->data);
				//printf("%s\n", generate_scope_string(scope_depth));

				char *auxiliar = strdup(root->children[0]->data);
				//Saving the scope in an auxiliar array
				valid_scopes[n_valid_scopes]= strdup(generate_scope_string(scope_depth));
				n_valid_scopes++;				
				
				for (i=1; i<root->n_children; i++)
					find_symbols(root->children[i]);
				
				pop_scope();
			
			}else if (strcmp(type_lookup[root->type],"BLOCK_N")==0)
			{
				push_nameless_scope();
				//printf("%s\n", generate_scope_string(scope_depth));
				
				//Saving the scope in an auxiliar array
				valid_scopes[n_valid_scopes]= strdup(generate_scope_string(scope_depth));
				n_valid_scopes++;
				
				
				for (i=0; i<root->n_children; i++)
					find_symbols(root->children[i]);
				
				pop_scope();
			}else{
				
				for (i=0; i<root->n_children; i++)
					find_symbols(root->children[i]);
			}



	}


}

void 
print_valid_scopes()
{
	int i;
	
	for(i=0; i<n_valid_scopes;i++)
		printf("%s - ", valid_scopes[i]);
	
	printf("\n");
}

void 
print_valid_scopes_reverse()
{
	int i;
	
	for(i=n_valid_scopes-1; i>=0;i--)
		printf("%s - ", valid_scopes[i]);
	
	printf("\n");
}
