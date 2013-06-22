%{

	#include "pencil.h"
%}
%option noyywrap

%%
[T|t][U|u][R|r][N|n]	{return TURN;}
[M|m][O|o][V|v][E|e]	{return MOVE;}
[D|d][R|r][A|a][W|w]	{return DRAW;}
<<EOF>>	{return END;}
.               { /* This rule matches any single character */ }
%%
