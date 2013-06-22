%{
#include <inttypes.h>

// This header gets us the (enumerated) tokens defined in the parser
#include "parser.tab.h"
%}

 /* A couple of directives on how flex should generate its C code... */
%array
%option noyywrap

 /*
  * An extremely minimalistic scanner implementation, the rule set below
  * recognizes "foobar" as the only valid string, and 42 as the only valid
  * integer. This has to be replaced: it is only there to make sure that
  * the skeleton code compiles and does something you can test.
  */

/* drozas:	- NOT_DQ defines a class which contains everything except double-quote.
		- In the rules section, we have to define rules for all special characters.
*/

DIGIT   [0-9]
ALPHA	[a-zA-Z]
NOT_DQ	[^\"]
%%
{DIGIT}+	{ return INTEGER;}
\"{NOT_DQ}+\"	{ return TEXT; }
PRINT		{ return PRINT; }	

\+ 		{ return '+'; }
\- 		{ return '-'; }
\/ 		{ return '/'; }
\* 		{ return '*'; }
\( 		{ return '('; }
\) 		{ return ')'; }
\, 		{ return ','; }
%%
