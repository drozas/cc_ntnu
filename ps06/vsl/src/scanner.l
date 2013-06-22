%{
#include "parser.tab.h"
long line_count = 1;
%}

%option array

WHITESPACE [\ \t]
NEWLINE [\n]
DIGIT [0-9]
LETTER [A-Za-z_]
COMMENT \/\/[^\n]*\n

%%
{WHITESPACE}+   {}
{COMMENT}   {}
{NEWLINE}   { line_count++; }
FUNC        { return FUNC; }
PRINT       { return PRINT; }
RETURN      { return RETURN; }
CONTINUE    { return CONTINUE; }
IF          { return IF; }
THEN        { return THEN; }
ELSE        { return ELSE; }
FI          { return FI; }
WHILE       { return WHILE; }
DO          { return DO; }
DONE        { return DONE; }
VAR         { return VAR; }
:=          { return ASSIGN; }
{DIGIT}+    { return INTEGER; }
\"[^\"]*\"  { return TEXT; }
{LETTER}({LETTER}|{DIGIT})* { return VARIABLE; }
.           { return yytext[0]; }
%%

int
yywrap ( void )
{
    return 1;
}
