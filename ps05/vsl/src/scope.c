#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "scope.h"


static const char *separator = "_";
static int STR_INITIAL_SIZE = 256;
static uint32_t counter = 0;
static uint32_t scope_limit = 0;

uint32_t scope_depth = 0;
char **scope_stack;


/*
 * Allocate space for a stack of at mosts dynamically allocated strings
 */
void
init_scopes ( uint32_t s )
{
	//We have an initial max size, but then the stack can grow
	scope_stack = calloc(s, sizeof(char*));
	scope_limit = s;
	scope_depth = 0;
	
}


/*
 * Free both the memory of the contents and of the stack itself
 */
void
destroy_scopes ( void )
{
	free(scope_stack);
}


/*
 * Add a string to the stack.
 * If the stack overruns, expand it by reallocation.
 */
void
push_scope ( char *text )
{
	if (scope_depth<scope_limit)
	{	
		scope_stack[scope_depth] = strdup(text);
	}else{
		//If the stack is gonna overrun, expand the allocation
		scope_limit++;

		scope_stack = realloc(scope_stack, (sizeof(char*))*(scope_limit));
		scope_stack[scope_depth] = strdup(text);
	}
	
	scope_depth++;

}


/*
 * Push a uniquely numbered string
 */
void
push_nameless_scope ( void )
{
	//Counter needs integer conversion
	char str_counter[20];
	sprintf(str_counter,"s%d",counter);
		
	if (scope_depth<scope_limit)
	{	
		scope_stack[scope_depth] = strdup(str_counter);

	}else{
		//If the stack is gonna overrun, expand the allocation
		scope_limit++;
		scope_stack = realloc(scope_stack, (sizeof(char*))*(scope_limit));
		
		scope_stack[scope_depth] = strdup(str_counter);

	}
	
	scope_depth++;
	counter ++;
}


/*
 * Remove the top string of the stack, freeing its memory
 */
void
pop_scope ( void )
{

	free(scope_stack[scope_depth]);
	scope_depth--;
}


/*
 * Retrieve a dynamically allocated copy of the stack contents,
 * concatenated and separated with the 'separator' constant.
 */
char *
generate_scope_string ( uint32_t depth )
{
	
	int i;
	int size = 1; //One, because of the "/0"  
	char *str;
	
	//We allocate for a string of STR_INITIAL_SIZE
	str = calloc(STR_INITIAL_SIZE, sizeof(char));
	
	
	for(i=0; i<depth; i++)
	{

		size = size + strlen(scope_stack[i])+1; //One because of the separator
		//if the current size is bigger or equal, we have to reallocate
		if (size>=STR_INITIAL_SIZE)
			str = realloc(str, (sizeof(char))*(size));
		
		strcat(str,separator);
		strcat(str,scope_stack[i]);

	}
	
    return str;
}


/*
 * Retrieve a dynamic copy as for generate_scope_string, with an
 * additional item added from the parameter 'text'.
 */
char *
augmented_scope_string ( uint32_t depth, char *text )
{
	
	int i;
	int size = 1; //One, because of the "/0"  
	char *str;
	
	//We allocate for a string of STR_INITIAL_SIZE
	str = calloc(STR_INITIAL_SIZE, sizeof(char));
	
	
	for(i=0; i<depth; i++)
	{

		size = size + strlen(scope_stack[i])+1; //One because of the separator
		//if the current size is bigger or equal, we have to reallocate
		if (size>STR_INITIAL_SIZE)
			str = realloc(str, (sizeof(char))*(size));
		
		strcat(str,separator);
		strcat(str,scope_stack[i]);

	}
	
	size = size + strlen(text);
	if (size>STR_INITIAL_SIZE)
		str = realloc(str, (sizeof(char))*(size));

	strcat(str,separator);
	strcat(str,text);

	
    return str;
}

void
print_stack()
{
	int top = scope_depth - 1;
	int i;
	printf("top=%d\n",top);
	for(i=top; i<0; i--)
	{
		printf("%s\n", scope_stack[i]);
	}
	
}
