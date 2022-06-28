%{
#include "RulesTypes.hpp"
#include "hw3_output.hpp"
#include "parser.tab.hpp"
%}

%option yylineno
%option noyywrap

whitespace ([\t\n\r ])

%%
void                        return VOID;
int                         return INT;
byte                        return BYTE;
b                           return B;
bool                        return BOOL;
auto                        return AUTO;
and                         return AND;
or                          return OR;
not                         return NOT;
true                        return TRUE;
false                       return FALSE;
return                      return RETURN;
if                          return IF;
else                        return ELSE;
while                       return WHILE;
break                       return BREAK;
continue                    return CONTINUE;
;                           return SC;
,                           return COMMA;
\(                          return LPAREN;
\)                          return RPAREN;
\{                          return LBRACE;
\}                          return RBRACE;
=                           return ASSIGN;
==|!=                       yylval = new OpClass(yytext); return RELOP_EQUAL;
\<|\>|\<\=|\>\=             yylval = new OpClass(yytext); return RELOP_GLT;
[\+|\-]                     yylval = new OpClass(yytext); return BINOP_ADD;
[\*|\/]                     yylval = new OpClass(yytext); return BINOP_MUL;
[a-zA-Z][a-zA-Z0-9]*        yylval = new IDClass(yytext); return ID;
0|[1-9][0-9]*               yylval = new NUMClass(yytext); return NUM;
\"([^\n\r\"\\]|\\[rnt"\\])+\" yylval = new StringClass(yytext); return STRING;

\/\/[^\r\n]*[\r|\n|\r\n]?   ;
{whitespace}                ;

.                           output::errorLex(yylineno); exit(0);

%%
