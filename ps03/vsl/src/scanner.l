%{
  #include "parser.tab.h"
%}

%option array
%option noyywrap

WHITESPACE [\ \t\n]
DIGIT [0-9]
LETTER [A-Za-z]

 /* Permit //-style line comments as well... */
COMMENT \/\/[^\n]*\n

%%
FUNC	{ return FUNC; }	
:=	{ return ASSIGN; }
PRINT	{ return PRINT; }
RETURN	{ return RETURN; }
CONTINUE	{ return CONTINUE; }
IF		{ return IF; }
THEN		{ return THEN;}
ELSE		{ return ELSE; }
FI		{ return FI; }
WHILE		{ return WHILE; }
DO		{ return DO; }
DONE		{ return DONE; }
VAR		{ return VAR;}

\} 		{ return '}';}
\{ 		{ return '{'; }
\+ 		{ return '+';}
\- 		{ return '-'; }
\/ 		{ return '/'; }
\* 		{ return '*'; }
\( 		{ return '('; }
\) 		{ return ')'; }
\, 		{ return ','; }

{WHITESPACE}+   {}
{COMMENT}       {}
{DIGIT}+        {return INTEGER; }
\"[^\"]*\"      {return TEXT; }
{LETTER}+	{return IDENTIFIER; }
.           {return yytext[0]; }
%%
