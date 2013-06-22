#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "generator.h"
#include "tree.h"
#include "symtab.h"
#include "asm_macros.h"
#include <string.h>

/*
 * This file contains a skeleton of how the suggested solution generator
 * is structured, in case you find it helpful. You are, however, free to write
 * whatever suits your way of thinking about code, the way you simplify trees,
 * etc. - the only interface to external code is the void generate(node_t *)
 * function, which must have its function signature intact.
 *
 * As you will notice, this skeleton contains a few constructs which are
 * not part of PS6 - you don't have to implement anything for those, but
 * you are free to continue on your own. (The remaining constructs will make
 * it into PS7 anyway, short of the CONTINUE statment, which I humbly submit
 * that we ignore).
 */


#define LABEL_STACK_SIZE 1024


static void generate_expression ( node_t *expression );
static void generate_declaration ( node_t *varlist );

/*
 * To save the trouble of building another stack, here is a simple one
 * to use for generating labels in IF and WHILE statements. A stack is
 * necessary because these constructs can nest.
 * Since we are writing assembler straight on standard output, we don't
 * need to worry about making strings out of these labels - printf conversion
 * will do. All this does is push (and pop) strictly increasing integers:
 * the first call to push_label will push a 0, the second a 1, etc...
 */
uint16_t label_stack[LABEL_STACK_SIZE], label_index = 0, label_counter = 0;

static void
push_label ( void )
{
    label_index++;
    label_stack[label_index] = label_counter;
    label_counter++;
}

static void
pop_label ( void )
{
    label_index--;
}


/*
 * All strings must be labelled uniquely as well, but since they don't
 * nest, a simple counter will suffice.
 */
uint32_t text_counter = 0;


static void
generate_data_segment ( node_t *root )
{
	if (root==NULL)
		return;

	if (strcmp(type_lookup[root->type],"TEXT_N")==0)
	{
		printf("STRING_%d:\n", text_counter);
		printf(".string %s\n", root->data);
		text_counter++;
	}
	
    for (int i=0; i<root->n_children; i++ )
        generate_data_segment(root->children[i]);
	
}


static void
generate_function ( node_t *function )
{
    // Stack frame handling, parameters, etc.
	printf("_%s: \t\n", function->children[0]->data);


	// Go through the list of parameters
	if (function->children[1] != NULL)
	for(int i=0; i<function->children[1]->n_children; i++)
		printf("\t pushl %d(%%ebp) \t /*Parameter %d*/\n", function->children[1]->children[i]->entry->stack_offset, i+1); 
	
}


static void
generate_declaration ( node_t *varlist )
{

    // Convenience function, making stack space for all vars in the list.
	for (int i=0; i<varlist->children[0]->n_children; i++ )
       	printf("\t pushl %d(%%ebp) \t/* Var declaration*/\n", varlist->children[0]->children[i]->entry->stack_offset);
}


static void
generate_bound_name ( node_t *var )
{
    // Locate a variable on stack, and push a copy of its value as the
    // result of an expression.
}


static void
assign_bound_name ( node_t *var )
{
	//Calculating the expression with children 1
	generate_expression(var->children[1]);
	
	//We make a pop for the final value on the stack and  copy it to the space in the memory allocated for this variable
	printf("\t popl %%eax \t/*Pop with the final result*/\n");
	printf("\t movl %%eax,%d(%%ebp) \t /*Moving the result to the var*/\n",var->children[0]->entry->stack_offset);


}


static void
generate_binary_operation ( node_t *binop )
{
    // Evaluate two subexpressions, handle a binary operation on the results
	
	//Push the first operand by type
	if (strcmp(type_lookup[binop->children[0]->type],"VARIABLE_N")==0)
	{
		//Making a push of a variable
		printf("\t pushl %d(%%ebp) \t/*Push of a variable in the first operand*/\n",binop->children[0]->entry->stack_offset);
	}else if (strcmp(type_lookup[binop->children[0]->type],"INTEGER_N")==0)
	{
		//Making a push of an integer value
		printf("\t pushl $%d \t/*Push of an integer value  in the first operand*/\n", *(int*)binop->children[0]->data);
	}
	
	//Push the second operand by type
	if (strcmp(type_lookup[binop->children[1]->type],"VARIABLE_N")==0)
	{
		//Making a push of a variable
		printf("\t pushl %d(%%ebp) \t/*Push of a variable in the second operand*/\n",binop->children[1]->entry->stack_offset);
	}else if (strcmp(type_lookup[binop->children[1]->type],"INTEGER_N")==0)
	{
		//Making a push of an integer value
		printf("\t pushl $%d \t/*Push of an integer value in the second operand*/\n", *(int*)binop->children[1]->data);
	}	
	
	
    // Copy out the expression data
    char binop2 = *(char *)binop->data;
	switch (binop2)
    {
		case '+':
			// Poping the two values in temporal registers
			printf("\t popl %%eax \t /*Pop of the first expression value*/\n");
			printf("\t popl %%ecx \t /*Pop of the second expression value*/\n");
		
			printf("\t addl %%ecx,%%eax \t /*Add, result saved in %%eax*/\n");
			printf("\t pushl %%eax \t /*Push the result at the top of the stack*/\n");
		break;
		
		case '-':
			// Poping the two values in temporal registers
			printf("\t popl %%eax \t /*Pop of the first expression value*/\n");
			printf("\t popl %%ecx \t /*Pop of the second expression value*/\n");
		
			printf("\t subl %%ecx,%%eax \t /*Substraction, result saved in %%eax*/\n");
			printf("\t pushl %%eax \t /*Push the result at the top of the stack*/\n");
		break;
		
		case '*':
			//We have to be initialize edx
			printf("\t movl $0,%%edx \t/*Initialize %%edx*/ \n");
			printf("\t popl %%eax \t/*Pop of the first expression value*/\n");
			printf("\t popl %%ecx \t/*Pop of the second expression value*/\n");
		
			printf("\t imull %%ecx \t/*Multiplication, result saved in %%eax*/\n");
			printf("\t pushl %%eax \t/*Push the result at the top of the stack*/\n");
		break;
		
		case '/':
			//We have to be initialize edx
			printf("\t movl $0,%%edx \t/*Initialize %%edx*/ \n");
			printf("\t popl %%eax \t/*Pop of the first expression value*/\n");
			printf("\t popl %%ecx \t/*Pop of the second expression value*/\n");
		
			printf("\t idivl %%ecx \t/*Division, result saved in %%eax*/\n");
			printf("\t pushl %%eax \t/*Push the result at the top of the stack*/\n");
		break;
	}
	
}

static void
generate_unary_minus ( node_t *uminus )
{
    // Just like a variable lookup, but there is another node to take care
    // of, as well as making sure the value on stack is negated.
    // (The NEG instruction "negl (address)" does negative values...)
	printf("popl %%eax /*Pop of the first expression value*/\n");
	printf("negl %%eax /*Add, result saved in %%eax*/\n");
	printf("pushl %%eax /*Push the result at the top of the stack*/\n");
}


static void
generate_call ( node_t *call )
{
    /* Push parameters, generate a call, remove parameters and push result */
}


static void
generate_expression ( node_t *expression )
{
	
	if (expression->n_children==2)
	{
		generate_binary_operation(expression);
	}
	else if (expression->n_children==1)
	{
		generate_unary_minus(expression);
		
	}else if (expression->n_children==0)
	{
		if (strcmp(type_lookup[expression->type],"VARIABLE_N")==0)
		{
			//Making a push of a variable
			printf("\t pushl %d(%%ebp) \t/*Push of a variable*/\n",expression->entry->stack_offset);
		}else if (strcmp(type_lookup[expression->type],"INTEGER_N")==0)
		{
			//Making a push of an integer value
			printf("\t pushl $%d \t/*Push of an integer value*/\n", *(int*)expression->data);
		}
		
		
	}

		
}


void
generate_print_list ( node_t *list )
{
    // Make a sequence of printf calls from a PRINT statement
}


void
generate_if_statement ( node_t *root )
{
    // Generate an IF statement. Mind that there may be further IF
    // statements inside the conditional code when you make jump labels.
    // (You can use the label stack at the top of this file)
}


void
generate_while_statement ( node_t *root )
{
    // Generate a while statement. Same nesting comments apply as for IF.
}


void
generate ( node_t *root )
{
    if ( root == NULL )
        return;
    switch ( root->type )
    {
        case PROGRAM_N:
			//Generate data head
			printf(DATA_HEAD);
			//Generate unique labels for strings
			generate_data_segment(root);
			//Create an entry point
			printf(TEXT_HEAD, root->children[0]->children[0]->entry->label );
		
			for (int i=0; i<root->n_children; i++ )
			{
       			generate(root->children[i]);
			}
			
            break;
        case FUNCTION_LIST_N:
;
			for (int i=0; i<root->n_children; i++ )
			{
       			generate(root->children[i]);
			}
            break;
        case FUNCTION_N:
			generate_function(root);
			generate(root->children[2]);

            break;
        case BLOCK_N:
			for (int i=0; i<root->n_children; i++ )
			{
       			generate(root->children[i]);
			}
            break;
        case DECLARATION_LIST_N:
			for (int i=0; i<root->n_children; i++ )
			{
       			generate(root->children[i]);
			}
            break;
        case DECLARATION_N:
			generate_declaration(root);
            break;
        case STATEMENT_LIST_N:
			for (int i=0; i<root->n_children; i++ )
			{
       			generate(root->children[i]);
			}
            break;
        case ASSIGNMENT_STATEMENT_N:
			assign_bound_name(root);
            break;
        case RETURN_STATEMENT_N:
			//The result is also an expression
			generate_expression(root->children[0]);
			printf("\t pushl %%eax \t /*Push of the final result*/\n");
			printf("\t ret \t /*Return control*/\n");
            break;
        case PRINT_STATEMENT_N:
            break;
        case NULL_STATEMENT_N:
            /* A proper implementation of the CONTINUE statement is more
             * trouble than it is worth at this point; since it is not the
             * most central of features, we skip it.
             * (Of course, if you really want to make one...)
             */
            printf ( "\t/* CONTINUE */\n" );
            break;
        case IF_STATEMENT_N:
            break;
        case WHILE_STATEMENT_N:
            break;
							  
        case EXPRESSION_N:
        case INTEGER_N:
        case VARIABLE_N:
			generate_expression(root);
            break;
							  
        default:
            fprintf ( stderr, "Warning: did not generate assembly for '%s'\n", root->type);
            break;

    }
	
}
