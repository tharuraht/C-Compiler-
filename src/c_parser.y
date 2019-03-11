%code requires{
  #include "ast.hpp"

  #include <cassert>
  #include <string>
  extern const AST_node *g_root; // A way of getting the AST out

  //! This is to fix problems when generating C++
  // We are declaring the functions provided by Flex, so
  // that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
}

// Represents the value associated with any kind of
// AST node.
%union{
  const AST_node* node;
  const Expression* expr;
  /*
  const constant *const; //added 
  const declaration *dec; //added
  const declarator *decr; //added
  const initialiser *init; //added
  const statement *stat; //added
  */
  double number;
  std::string *string;
}

%token T_INT T_DOUBLE T_FLOAT T_VOID T_CHAR T_SHORT T_LONG T_IF T_ELSE T_WHILE T_FOR T_DO T_BREAK T_CONTINUE T_GOTO

%token T_TIMES T_PLUS T_MINUS T_DIVIDE T_MODULUS  T_INCREMENT T_DECREMENT

%token T_EQUAL T_PLUS_EQUAL T_MINUS_EQUAL T_TIMES_EQUAL T_DIVIDE_EQUAL T_MODULUS_EQUAL

%token T_IS_EQUAL T_IS_NOT_EQUAL T_LESS_THAN T_LESS_EQUAL_THAN T_GREATER_THAN T_GREATER_EQUAL_THAN

%token T_LOGICAL_OR T_LOGICAL_AND T_NOT 

%token T_BITWISE_AND T_BITWISE_OR T_BITWISE_XOR T_BITWISE_COMPLEMENT T_LSHIFT T_RSHIFT

%token T_LBRACKET T_CURLY_LBRACKET T_SQUARE_LBRACKET T_LHEADER T_RBRACKET T_CURLY_RBRACKET T_SQUARE_RBRACKET T_RHEADER

%token T_SEMICOLON T_COLON T_COMMA

%token T_NUMBER T_VARIABLE T_RETURN T_STRING

%token T_SIGNED T_GO_TO T_AUTO T_STRUCT 

%type <expr> EXPR TERM FACTOR BINARY_EXPRESSION_TREE
%type <expr> C_EXPRESSION C_INCREMENT_DECREMENT C_ARGS COMPARISONEXPR DECLARE_VAR

%type <node> PROGRAM EX_DECLARATION FUNCTION_DEF FUNCTION_CALL GLOBAL_DECLARATION
%type <node> SCOPE SCOPE_BODY
%type <node> STATEMENT T_IF T_ELSE T_WHILE T_FOR T_RETURN
%type <node> BINARY 

%type <number> T_NUMBER 
%type <string> T_VARIABLE FUNCTION_NAME T_VOID T_INT T_DOUBLE T_FLOAT TYPE_SPECIFY


//%right "then" T_ELSE solution for dangling if else problem
%nonassoc "then" //solution for dangling if else problem
%nonassoc T_ELSE //solution for dangling if else problem

%start ROOT

%%

/* The TODO notes a are just a guide, and are non-exhaustive.
   The expectation is that you do each one, then compile and test.
   Testing should be done using patterns that target the specific
   feature; the testbench is there to make sure that you haven't
   broken anything while you added it.
*/

ROOT : DECLARE_VAR {g_root = $1;}

/*
PROGRAM
  : EX_DECLARATION  {$$ = $1;}
  | PROGRAM EX_DECLARATION {$$ = $1;}      //TODO
  ;

EX_DECLARATION :
    FUNCTION_DEF  {$$ = $1;}
  | GLOBAL_DECLARATION {$$ = $1;}
  ;

GLOBAL_DECLARATION :
    TYPE_SPECIFY T_VARIABLE T_SEMICOLON {$$ = new GlobalVarDec($1,$2,NULL);}
  | TYPE_SPECIFY T_VARIABLE T_EQUAL C_EXPRESSION T_SEMICOLON  {$$ = new GlobalVarDec($1,$2,*$4);}

FUNCTION_DEF : 
    TYPE_SPECIFY T_VARIABLE T_LBRACKET T_RBRACKET SCOPE {$$ = new FunctionDef($1,$2,NULL,$5);}
  | TYPE_SPECIFY T_VARIABLE T_LBRACKET C_ARGS T_RBRACKET SCOPE {$$ = new FunctionDef($1,$2,$4,$5);}



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


FUNCTION_CALL
  : T_VARIABLE T_LBRACKET T_RBRACKET  {$$ = new FunctionCall($1, NULL);}
  | T_VARIABLE T_LBRACKET C_ARGS T_RBRACKET {$$ = new FunctionCall($1, $3);}
  
//-----------------------------------------------------------------------------
*/
DECLARE_VAR
  : TYPE_SPECIFY T_VARIABLE T_SEMICOLON {$$ = new LocalVarDec (*$1,*$2,NULL);}
  | TYPE_SPECIFY T_VARIABLE T_EQUAL C_EXPRESSION T_SEMICOLON {$$ = new LocalVarDec (*$1,*$2,$4);}

TYPE_SPECIFY
  : T_VOID    {$$ = $1;}
  | T_INT     {$$ = $1;}
  | T_DOUBLE  {$$ = $1;}
  | T_FLOAT   {$$ = $1;}
  ;

C_EXPRESSION
  : BINARY_EXPRESSION_TREE {$$ = $1;}
  /*
  | C_INCREMENT_DECREMENT {$$ = $1;}
  | FUNCTION_CALL {$$ = $1;}
  */

BINARY_EXPRESSION_TREE
  : BINARY_EXPRESSION_TREE T_PLUS BINARY_EXPRESSION_TREE     { $$ = new AddOperator($1, $3);}
  | BINARY_EXPRESSION_TREE T_MINUS BINARY_EXPRESSION_TREE    { $$ = new SubOperator($1, $3);}
  | TERM                 { $$ = $1; }
  
TERM : TERM T_TIMES TERM  { $$ = new MulOperator($1, $3);}
     | TERM T_DIVIDE TERM { $$ = new DivOperator($1, $3);}
     | FACTOR               { $$ = $1; }

FACTOR : T_VARIABLE         {$$ = new Variable(*$1);}
       | T_NUMBER           {$$ = new Number( $1 );}

/*
C_ARGS :
    C_EXPRESSION {$$ = new Args($1,NULL);}
  | C_EXPRESSION T_COMMA C_ARGS {$$ = new Args($1,$3);}


COMPARISONEXPR : 
    C_EXPRESSION COMPARISON_OP C_EXPRESSION     {$$ = new ComparisonExpr($1,$2,$3);}
  | COMPARISONEXPR T_LOGICAL_AND COMPARISONEXPR {$$ = new ComparisonExpr($1,new std::string("&&"));}
  | COMPARISONEXPR T_LOGICAL_OR COMPARISONEXPR  {$$ = new ComparisonExpr($1,new std::string("||"));}
  | COMPARISONEXPR                              {$$ = $1;}


COMPARISON_OP :   
    T_IS_EQUAL            {$$ = new std::string("==");}
  | T_IS_NOT_EQUAL        {$$ = new std::string("!=");}
  | T_LESS_THAN           {$$ = new std::string("<");}
  | T_GREATER_THAN        {$$ = new std::string(">");}
  | T_LESS_EQUAL_THAN     {$$ = new std::string("<=");}
  | T_GREATER_EQUAL_THAN  {$$ = new std::string(">=");}  


C_INCREMENT_DECREMENT : 
    T_VARIABLE T_INCREMENT  {$$ = new PostIncrement($1);}
  | T_VARIABLE T_DECREMENT  {$$ = new PostDecrement($1);}
  | T_INCREMENT T_VARIABLE  {$$ = new PreIncrement($2);}
  | T_DECREMENT T_VARIABLE  {$$ = new PreDecrement($2);}
*/

/*TODO
COMPARISONEXPR
BINARY_EXPRESSION_TREE
C_INCREMENT_DECREMENT
C_ARGS
DECLARATION
Factor can also have function call
*/
%%

const AST_node *g_root; // Definition of variable (to match declaration earlier)

const AST_node *parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}
