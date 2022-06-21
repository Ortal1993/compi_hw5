%{
#include "tokens.hpp"
#include <stdio.h>
%}

%option yylineno
%option noyywrap

digit ([0-9])
letter ([a-zA-Z])
whitespace ([\t\n\r ])
string_chars [\t\x20-\x7E]
string_hexa_to_string ([0-7][0-9a-fA-F])
string_escape_sequence (\\\\|\\\"|\\n|\\r|\\t|\\0|\\x{string_hexa_to_string})
string_ilegal_sequence (\\.|\\\n|\\\r|\\x|\\x(.|\r\n)|\\x(.|\r\n)(.|\r\n))

%X STRING_RULES

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
[==|!=|<|>|<=|>=]           return RELOP;
[\+|\-|\*|\/]               return BINOP;
{letter}({digit}|{letter})* return ID;
0{digit}+              return ERROR_UNDEFINED_CHAR;
0|[1-9]{digit}*             return NUM;
\/\/[^\n\r]*                 return COMMENT;

[\"]                                      BEGIN(STRING_RULES); return STRING;\\why we also did returm here?
<STRING_RULES>{string_escape_sequence}    return STRING;
<STRING_RULES>{string_ilegal_sequence}    return ERROR_UNDEFINED_ESCAPE_SEQUENCE;
<STRING_RULES>[\"]                        BEGIN(INITIAL); return STRING;\\why we also did returm here? why we did again \" if it is part of string_escape_sequence?
<STRING_RULES>[\n\r]                      return ERROR_UNCLOSED_STRING;
<STRING_RULES>{string_chars}              return STRING;
<STRING_RULES>.                           return ERROR_UNDEFINED_CHAR;

{whitespace}                ;
.                           return ERROR_UNDEFINED_CHAR;

%%
