%{
#include <string>
#include "interpreter.h"
#include "parser.tab.hpp"

extern int yyparse();
%}

%option noyywrap
%option yylineno

%%

[ \t\n\r]+              { /* skip whitespace */ }
;.*                     { /* skip comments */ }

"("                     { return LPAREN; }
")"                     { return RPAREN; }

[0-9]+                  { yylval.num = atoi(yytext); return INTEGER; }
[a-zA-Z_+\-*/%=<>][a-zA-Z0-9_]*  { yylval.str = new std::string(yytext); return SYMBOL; }

.                       { return yytext[0]; }

%%
