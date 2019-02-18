%option noyywrap

%{
/* Now in a section of C that will be embedded
   into the auto-generated code. Flex will not
   try to interpret code surrounded by %{ ... %} */

/* Bring in our declarations for token types and
   the yylval variable. */

#include "parser.tab.hpp"

// This is to work around an irritating bug in Flex
// https://stackoverflow.com/questions/46213840/get-rid-of-warning-implicit-declaration-of-function-fileno-in-flex
extern "C" int fileno(FILE *stream);

/* End the embedded code section. */
%}
%%

("//").* || ("/*").*("*/")    {} /* Ignore comments */

"int" 			    { yylval.string = new std::string(yytext); return T_INT; }
"double"        { yylval.string = new std::string(yytext); return T_DOUBLE; }
"float"         { yylval.string = new std::string(yytext); return T_FLOAT; }
"void"          { yylval.string = new std::string(yytext); return T_VOID; }
"char"          { yylval.string = new std::string(yytext); return T_CHAR; }
"short"         { yylval.string = new std::string(yytext); return T_SHORT; }
"long"          { yylval.string = new std::string(yytext); return T_LONG; }
"signed"        { yylval.string = new std::string(yytext); return T_SIGNED; }
"if"            { yylval.string = new std::string(yytext); return T_IF; }
"else"          { yylval.string = new std::string(yytext); return T_ELSE; }
"while"         { yylval.string = new std::string(yytext); return T_WHILE; }
"for"           { yylval.string = new std::string(yytext); return T_FOR; }
"do"            { yylval.string = new std::string(yytext); return T_DO; }
"break"			    { yylval.string = new std::string(yytext); return T_BREAK; }
"continue"		  { yylval.string = new std::string(yytext); return T_CONTINUE; }
"go to"			    { yylval.string = new std::string(yytext); return T_GO_TO; }

/* Operators below: */

[*]             { return T_TIMES; }
[+]             { return T_PLUS; }
[-]             { return T_MINUS; }
[/]             { return T_DIVIDE; }
[%]             { return T_MODULUS; }

/* Assignment operators below: */

[=]             { return T_EQUAL; }
[+=]            { return T_PLUS_EQUAL; }
[-=]            { return T_MINUS_EQUAL; }
[*=]            { return T_TIMES_EQUAL; }
[/=]            { return T_DIVIDE_EQUAL; }
[%=]            { return T_MODULUS_EQUAL; }

/* Relational operators below: */

[==]            { return T_IS_EQUAL; }
[!=]            { return T_IS_NOT_EQUAL; }
[<]             { return T_LESS_THAN; }
[<=]            { return T_LESS_EQUAL_THAN; }
[=<]            { return T_LESS_EQUAL_THAN; }
[>]             { return T_GREATER_THAN; }
[>=]            { return T_GREATER_EQUAL_THAN; }
[=>]            { return T_GREATER_EQUAL_THAN; }

/* Logical operators below: */

[||]            { return T_LOGICAL_OR; }
[&&]            { return T_LOGICAL_AND; }
[!]             { return T_NOT; }

/* Bitwise operators below: */

[&]             { return T_BITWISE_AND; }
[|]             { return T_BITWISE_OR; }
[^]             { return T_BITWISE_XOR; }
[~]             { return T_BITWISE_COMPLEMENT; }
[<<]            { return T_LSHIFT; }
[>>]            { return T_RSHIFT; }

/* Bracket operators below: */

[(]             { return T_LBRACKET; }
[{]             { return T_CURLY_LBRACKET; }
"["             { return T_SQUARE_LBRACKET; }
[<]             { return T_LHEADER; }
[)]             { return T_RBRACKET; }
[}]             { return T_CURLY_RBRACKET; }
"]"             { return T_SQUARE_RBRACKET; }
[>]             { return T_RHEADER; }

/* Punctuation operators below: */

[;]				      { return T_SEMI_COLON; }
[:]				      { return T_COLON; }
[,]				      { return T_COMMA; }

/* Whitespaces are ignored: */
[ \n\t]

[-]?[0-9]+([.][0-9]*)? { yylval.number=strtod(yytext, 0); return T_NUMBER; }

[a-z]+[a-z0-9]*        { yylval.string=new std::string(yytext); return T_VARIABLE; }

%%

/* Error handler. This will get called if none of the rules match. */
void yyerror (char const *s)
{
  fprintf (stderr, "Flex Error: %s\n", s); /* s is the text that wasn't matched */
  exit(1);
}
