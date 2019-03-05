%code requires{
  #include "ast.hpp"

  #include <cassert>

  extern const Expression *g_root; // A way of getting the AST out

  //! This is to fix problems when generating C++
  // We are declaring the functions provided by Flex, so
  // that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
}

// Represents the value associated with any kind of
// AST node.
%union{
  const Expression *expr;
  const constant *const; //added 
  const declaration *dec; //added
  const declarator *decr; //added
  const initialiser *init; //added
  const statement *stat; //added
  double number;
  std::string *string;
}

%token T_INT T_DOUBLE T_FLOAT T_VOID T_CHAR T_SHORT T_LONG T_IF T_ELSE T_WHILE T_FOR T_DO T_BREAK T_CONTINUE T_GOTO

%token T_TIMES T_PLUS T_MINUS T_DIVIDE T_MODULUS

%token T_EQUAL T_PLUS_EQUAL T_MINUS_EQUAL T_TIMES_EQUAL T_DIVIDE_EQUAL T_MODULUS_EQUAL

%token T_IS_EQUAL T_IS_NOT_EQUAL T_LESS_THAN T_LESS_EQUAL_THAN T_GREATER_THAN T_GREATER_EQUAL_THAN

%token T_LOGICAL_OR T_LOGICAL_AND T_NOT 

%token T_BITWISE_AND T_BITWISE_OR T_BITWISE_XOR T_BITWISE_COMPLEMENT T_LSHIFT T_RSHIFT

%token T_LBRACKET T_CURLY_LBRACKET T_SQUARE_LBRACKET T_LHEADER T_RBRACKET T_CURLY_RBRACKET T_SQUARE_RBRACKET T_RHEADER

%token T_SEMICOLON T_COLON T_COMMA

%token T_NUMBER T_VARIABLE T_RETURN

%type <expr> EXPR TERM FACTOR EXPONENT
%type <expr> PROGRAM  EX_DECLARATION FUNCTION_DEF TYPE_SPECIFY
%type <expr> SCOPE SCOPE_BODY
%type <expr> C_EXPRESSION C_INCREMENT_DECREMENT
%type <number> T_NUMBER
%type <string> T_VARIABLE T_LOG T_EXP T_SQRT FUNCTION_NAME

%right "then" T_ELSE //solution for dangling if else problem

%start ROOT

%%

/* The TODO notes a are just a guide, and are non-exhaustive.
   The expectation is that you do each one, then compile and test.
   Testing should be done using patterns that target the specific
   feature; the testbench is there to make sure that you haven't
   broken anything while you added it.
*/

/* maths parser
ROOT : PROG_DECL { g_root = $1; }

PROG_DECL : TERM                 { $$ = $1; }
    |EXPR T_PLUS TERM       { $$ = new AddOperator($1, $3); }
    |EXPR T_MINUS TERM      { $$ = new SubOperator($1, $3); }

TERM : EXPONENT                    { $$ = $1; }
      |TERM T_TIMES EXPONENT       { $$ = new MulOperator($1, $3); }
      |TERM T_DIVIDE EXPONENT      { $$ = new DivOperator($1, $3); }

EXPONENT : FACTOR                    { $$ = $1; }
         |FACTOR T_EXPONENT EXPONENT { $$ = new ExpOperator($1, $3); }

FACTOR : T_NUMBER           { $$ = new Number( $1 ); }
       | T_LBRACKET EXPR T_RBRACKET { $$ = $2; }
       | T_VARIABLE { $$ = new Variable( *$1 ); }
       | FUNCTION_NAME T_LBRACKET EXPR T_RBRACKET 
         {
         if (*$1 == "log") { $$ = new LogFunction ($3); }
         
         else if (*$1 == "exp") { $$ = new ExpFunction ($3); }
         
         else if (*$1 == "sqrt") { $$ = new SqrtFunction ($3); }
         }

FUNCTION_NAME : T_LOG { $$ = new std::string("log"); }
              | T_EXP { $$ = new std::string("exp"); }
              | T_SQRT { $$ = new std::string("sqrt"); }
*/
ROOT : PROGRAM {g_root = $1;}

PROGRAM
  : EX_DECLARATION  {$$ = $1;}
  | PROGRAM EX_DECLARATION {$$ = }      //TODO
  ;

EX_DECLARATION
  : FUNCTION_DEF  {$$ = $1;}
  | DECLARATION {$$ = $1;}
  ;

FUNCTION_DEF
  : TYPE_SPECIFY T_VARIABLE T_LBRACKET T_RBRACKET SCOPE {$$ = new FunctionDec($1,$2,NULL,$5);}


TYPE_SPECIFY
  : T_VOID  {$$ = new std::string("void");}
  | T_INT {$$ = new std::string("int");}
  | T_DOUBLE {$$ = new std::string("double");}
  | T_FLOAT {$$ = new std::string("float");}
  ;

SCOPE : T_CURLY_LBRACKET SCOPE_BODY T_CURLY_RBRACKET  {$$ = $2;} ;

SCOPE_BODY
  : STATEMENT {$$ = new Body($1, NULL);}
  | SCOPE_BODY STATEMENT  {$$ = new Body($1, $2);}
  | DECLARE_VAR {$$ = new Body($1,NULL);}
  | SCOPE_BODY DECLARE_VAR {$$ = new Body($1,$2);}
  ;

STATEMENT
  : T_RETURN C_EXPRESSION T_SEMICOLON {$$ = new ReturnStatement($2);}
  | T_VARIABLE T_EQUAL C_EXPRESSION T_SEMICOLON {$$ = new AssignmentStatement($3);}
  | T_IF T_LBRACKET COMPARISONEXPR T_RBRACKET SCOPE {$$ = new IfStatement($3,$5,NULL);}
  | T_IF T_LBRACKET COMPARISONEXPR T_RBRACKET SCOPE T_ELSE SCOPE {$$ = new IfStatement($3,$5,$7);}
  | T_WHILE T_LBRACKET COMPARISONEXPR T_RBRACKET SCOPE  {$$ = new WhileStatment($3,$5);}
  | T_FOR T_LBRACKET DECLARE_VAR COMPARISONEXPR T_SEMICOLON C_INCREMENT_DECREMENT T_RBRACKET SCOPE {$$ = new ForStatement($3,$4,$6,$8);}
  ;

/*TODO
DECLARE_VAR
COMPARISONEXPR
C_EXPRESSION
C_INCREMENT_DECREMENT
*/
%%

const Expression *g_root; // Definition of variable (to match declaration earlier)

const Expression *parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}
