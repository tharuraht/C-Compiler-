%option noyywrap

%{
/* Now in a section of C that will be embedded
   into the auto-generated code. Flex will not
   try to interpret code surrounded by %{ ... %} */

/* Bring in our declarations for token types and
   the yylval variable. */

#include "c_parser.tab.hpp"
#include <iostream>

// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

/* End the embedded code section. */
%}


%%

("//").* || ^"/*"[^*]*|[*]*"*/"    {} /* Ignore comments */

"int" 			    { yylval.string = new std::string(yytext); return T_INT; }
"double"        { yylval.string = new std::string(yytext); return T_DOUBLE; }
"float"         { yylval.string = new std::string(yytext); return T_FLOAT; }
"long"          { yylval.string = new std::string(yytext); return T_LONG; }
"void"          { yylval.string = new std::string(yytext); return T_VOID; }
"char"          { yylval.string = new std::string(yytext); return T_CHAR; }
"short"         { yylval.string = new std::string(yytext); return T_SHORT; }
"signed"        { yylval.string = new std::string(yytext); return T_SIGNED; }
"if"            { yylval.string = new std::string(yytext); return T_IF; }
"else"          { yylval.string = new std::string(yytext); return T_ELSE; }
"while"         { yylval.string = new std::string(yytext); return T_WHILE; }
"for"           { yylval.string = new std::string(yytext); return T_FOR; }
"do"            { yylval.string = new std::string(yytext); return T_DO; }
"break"			    { yylval.string = new std::string(yytext); return T_BREAK; }
"continue"		  { yylval.string = new std::string(yytext); return T_CONTINUE; }
"go to"			    { yylval.string = new std::string(yytext); return T_GO_TO; }
"auto"          { yylval.string = new std::string(yytext); return T_AUTO; }
"struct"        { yylval.string = new std::string(yytext); return T_STRUCT; }
"return"        { yylval.string = new std::string(yytext); return T_RETURN; }

[0-9]+([.][0-9]*) { yylval.number=strtod(yytext, 0); return T_NUMBER; }

[a-z]+[a-z0-9]*        { yylval.string=new std::string(yytext); return T_VARIABLE; }

[0-9]+          { ECHO; yylval.number=strtod(yytext, 0); return T_INT; }




[*]             { return T_TIMES; }
[+]             { return T_PLUS; }
[-]             { return T_MINUS; }
[/]             { return T_DIVIDE; }
[%]             { return T_MODULUS; }



[=]             { return T_EQUAL; }
[+][=]            { return T_PLUS_EQUAL; }
[-][=]            { return T_MINUS_EQUAL; }
[*][=]            { return T_TIMES_EQUAL; }
[/][=]            { return T_DIVIDE_EQUAL; }
[%][=]            { return T_MODULUS_EQUAL; }



[=][=]            { return T_IS_EQUAL; }
[!][=]            { return T_IS_NOT_EQUAL; }
[<]             { return T_LESS_THAN; }
[<][=]            { return T_LESS_EQUAL_THAN; }
[=][<]            { return T_LESS_EQUAL_THAN; }
[>]             { return T_GREATER_THAN; }
[>][=]            { return T_GREATER_EQUAL_THAN; }
[=][>]            { return T_GREATER_EQUAL_THAN; }


[|][|]            { return T_LOGICAL_OR; }
[&][&]            { return T_LOGICAL_AND; }
[!]             { return T_NOT; }



[&]             { return T_BITWISE_AND; }
[|]             { return T_BITWISE_OR; }
["^"]             { return T_BITWISE_XOR; }
[~]             { return T_BITWISE_COMPLEMENT; }
[<][<]            { return T_LSHIFT; }
[>][>]            { return T_RSHIFT; }



[(]             { return T_LBRACKET; }
[{]             { return T_CURLY_LBRACKET; }
"["             { return T_SQUARE_LBRACKET; }
[<]             { return T_LHEADER; }
[)]             { return T_RBRACKET; }
[}]             { return T_CURLY_RBRACKET; }
"]"             { return T_SQUARE_RBRACKET; }
[>]             { return T_RHEADER; }



[;]				      { return T_SEMICOLON; }
[:]				      { return T_COLON; }
[,]				      { return T_COMMA; }


[+][+]            { return T_INCREMENT; }
[-][-]            { return T_DECREMENT; }

[ \n\t]         {;}
.                 { fprintf(stderr, "Invalid token\n"); exit(1); }

%%

/* Error handler. This will get called if none of the rules match. */
void yyerror (char const *s)
{
  fprintf (stderr, "Flex Error: %s\n", s); /* s is the text that wasn't matched */
  exit(1);
}

// //main to check what is being lexed
// int main()
// {
//     int token;
//     while ((token = yylex()) != 0)
//         printf("Token: %d (%s)\n", token, yytext);
//     return 0;
// }
