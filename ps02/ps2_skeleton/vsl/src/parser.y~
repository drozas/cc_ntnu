 /*
  * This Yacc specification is structured into definition, rules and program
  * sections, just like Lex specifications. The difference is that the 
  * rules section now specifies productions instead of regular expressions,
  * and the resultant function (called yyparse()) implements a bottom-up
  * parser, instead of a DFA. This function is called from 'main.c', and
  * attempts to parse text from standard input.
  */
%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <inttypes.h>

    // External variables - these refer to state in the scanner
    extern char yytext[];
    extern int line_count;
%}

// Union type for left-hand-side values - we want integers and strings
%union {
    int32_t ival;
    char *sval;
};

/*
 * These are the tokens we want the scanner to return.
 * When the generator runs, these are put into an enumerated type inside
 * work/parser.tab.h, which the scanner can include, to return the token
 * values we expect.
 */
%token INTEGER TEXT PRINT

// Handle operator precedences
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

/*
 * Here we declare the types of some production left-hand sides such that
 * the type of anything composed from these can be derived.
 */ 
%type <sval> text TEXT
%type <ival> integer INTEGER
%type <ival> expression 

%%
program:

	print_statement
		{
		};



print_statement:
		PRINT print_list
		{
		};
			
			

print_list:
	print_item
		{
		}
	|print_list ',' print_item
		{
		};


print_item:
	expression
		{
			printf("print_item->expression:%d\n",$1);

		}
	|text
		{
			printf("print_item->text:%s\n", $1);
			free($1);
		};			



expression:
	expression '+' expression
		{
			$$ = $1 + $3;
		}

	|expression '-' expression
		{
			$$ = $1 - $3;
		}

	|expression '*' expression
		{
			$$ = $1 * $3;
		}

	|expression '/' expression
		{
			$$ = $1 / $3;
		}

	|'-'expression
		{
			$$ = $2 * -1;
		}
	|'(' expression ')'
		{
			$$ = ( $2 );
		}

	|integer{
		};

integer:
    INTEGER
    {
        char *temp = strdup ( yytext );
        $$ = strtol ( temp, NULL, 10 );
        free ( temp );
    };
text:
    TEXT
    {
        uint16_t len;

        $$ = strdup ( yytext + 1 );     // Get heap-allocated copy of string
        len = strlen ( $$ );            // Find out how long it is
        $$[len-1] = '\0';               // Delete the trailing " character

    };
%%


/*
 * This function is mandatory for a Yacc specification, and it is called if
 * there is a syntax error in the input. We don't need to handle that
 * gracefully here.
 */
int
yyerror ( void )
{
    return 0;
}
