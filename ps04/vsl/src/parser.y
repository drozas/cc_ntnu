%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"
#define YYSTYPE node_t *

extern char yytext[];
extern int line_count;
extern node_t *root;
%}

%token INTEGER TEXT VARIABLE ASSIGN
%token FUNC PRINT RETURN CONTINUE IF THEN ELSE FI WHILE DO DONE VAR
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%%


/* Top level: a program is a list of functions */
program:
      function_list
        { root = create_node ( PROGRAM_N, 1, NULL, $1 ); }
    ;


/*
 * Lists of various kinds:
 * All of these follow the general structure of 'item | item_list item',
 * potentially with an empty list.
 */
function_list:
      function { $$ = create_node ( FUNCTION_LIST_N, 1, NULL, $1 ); }
    | function_list function
        { $$ = create_node ( FUNCTION_LIST_N, 2, NULL, $1, $2 ); }
    ;
statement_list:
      statement { $$ = create_node ( STATEMENT_LIST_N, 1, NULL, $1 ); }
    | statement_list statement
        { $$ = create_node ( STATEMENT_LIST_N, 2, NULL, $1, $2 ); }
    ;
print_list:
      print_item { $$ = create_node ( PRINT_LIST_N, 1, NULL, $1 ); }
    | print_list ',' print_item
        { $$ = create_node ( PRINT_LIST_N, 2, NULL, $1, $3 ); }
    ;
expression_list:
      expression { $$ = create_node ( EXPRESSION_LIST_N, 1, NULL,  $1 ); }
    | expression_list ',' expression
        { $$ = create_node ( EXPRESSION_LIST_N, 2, NULL, $1, $3 ); }
    ;
variable_list:
      variable { $$ = create_node ( VARIABLE_LIST_N, 1, NULL, $1 ); }
    | variable_list ',' variable
        { $$ = create_node ( VARIABLE_LIST_N, 2, NULL, $1, $3 ); }
    ;
argument_list:
      expression_list { $$ = create_node ( ARGUMENT_LIST_N, 1, NULL, $1 ); }
    | /* e */ { $$ = NULL; }
    ;
parameter_list:
      variable_list { $$ = create_node ( PARAMETER_LIST_N, 1, NULL, $1 ); }
    | /* e */ { $$ = NULL; }
    ;
declaration_list:
      declaration_list declaration
        { $$ = create_node ( DECLARATION_LIST_N, 2, NULL, $1, $2 ); }
    | /* e */ { $$ = NULL; }
    ;


/* Function declarations: name, parameter list and a (block) statement */
function:
      FUNC variable '(' parameter_list ')' statement
        { $$ = create_node ( FUNCTION_N, 3, NULL, $2, $4, $6 ); };


/*
 * The various kinds of statement: basic statements, or blocks of them.
 * This is a purely syntactic construct, it adds no information to the tree.
 */
statement:
      assignment_statement { $$ = create_node ( STATEMENT_N, 1, NULL, $1 ); }
    | return_statement     { $$ = create_node ( STATEMENT_N, 1, NULL, $1 ); }
    | print_statement      { $$ = create_node ( STATEMENT_N, 1, NULL, $1 ); }
    | null_statement       { $$ = create_node ( STATEMENT_N, 1, NULL, $1 ); }
    | if_statement         { $$ = create_node ( STATEMENT_N, 1, NULL, $1 ); }
    | while_statement      { $$ = create_node ( STATEMENT_N, 1, NULL, $1 ); }
    | block                { $$ = create_node ( STATEMENT_N, 1, NULL, $1 ); }
    ;
block:
      '{' declaration_list statement_list '}'
        { $$ = create_node ( BLOCK_N, 2, NULL, $2, $3 ); }
    ;
assignment_statement:
      variable ASSIGN expression
        { $$ = create_node ( ASSIGNMENT_STATEMENT_N, 2, NULL, $1, $3 ); }
    ;
return_statement:
      RETURN expression
        { $$ = create_node ( RETURN_STATEMENT_N, 1, NULL, $2 ); }
    ;
print_statement:
      PRINT print_list
        { $$ = create_node ( PRINT_STATEMENT_N, 1, NULL, $2 ); }
    ;
null_statement:
      CONTINUE
        { $$ = create_node ( NULL_STATEMENT_N, 0, NULL ); };
if_statement:
      IF expression THEN statement FI
        { $$ = create_node ( IF_STATEMENT_N, 2, NULL, $2, $4 ); }
    | IF expression THEN statement ELSE statement FI
        { $$ = create_node ( IF_STATEMENT_N, 3, NULL, $2, $4, $6 ); }
    ;
while_statement:
      WHILE expression DO statement DONE
        { $$ = create_node ( WHILE_STATEMENT_N, 2, NULL, $2, $4 ); }
    ;


/*
 * Various building blocks for statements: expressions, and the
 * terminals they are composed from. Note that attribute values
 * are always dynamically allocated.
 */
print_item:
      expression { $$ = create_node ( PRINT_ITEM_N, 1, NULL, $1 ); }
    | text       { $$ = create_node ( PRINT_ITEM_N, 1, NULL, $1 ); }
    ;
expression:
      expression '+' expression
        { $$ = create_node ( EXPRESSION_N, 2, strdup ("+"), $1, $3 ); }
    | expression '-' expression
        { $$ = create_node ( EXPRESSION_N, 2, strdup ("-"), $1, $3 ); }
    | expression '*' expression
        { $$ = create_node ( EXPRESSION_N, 2, strdup ("*"), $1, $3 ); }
    | expression '/' expression
        { $$ = create_node ( EXPRESSION_N, 2, strdup ("/"), $1, $3 ); }
    | '-' expression %prec UMINUS
        { $$ = create_node ( EXPRESSION_N, 1, strdup ("-"), $2 ); }
    | '(' expression ')'
        { $$ = create_node ( EXPRESSION_N, 1, NULL, $2 ); }
    | integer
        { $$ = create_node ( EXPRESSION_N, 1, NULL, $1 ); }
    | variable
        { $$ = create_node ( EXPRESSION_N, 1, NULL, $1 ); }
    | variable '(' argument_list ')'
        { $$ = create_node ( EXPRESSION_N, 2, NULL, $1, $3 ); }
    ;
declaration:
      VAR variable_list
        { $$ = create_node ( DECLARATION_N, 1, NULL, $2 ); }
    ;
variable:
      VARIABLE { $$ = create_node ( VARIABLE_N, 0, strdup ( yytext ) ); }
    ;
integer:
      INTEGER
      {
        $$ = create_node ( INTEGER_N, 0, calloc ( 1, sizeof(int) ) );
        *((int *)$$->data) = strtol ( yytext, NULL, 10 );
      }
    ;
text:
      TEXT
        { $$ = create_node ( TEXT_N, 0, strdup ( yytext ) ); }
    ;
%%

int
yyerror ( void )
{
    printf ( "Syntax error\n" );
    return 0;
}
