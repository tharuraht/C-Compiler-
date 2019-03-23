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

%token   T_MODULUS  T_INCREMENT T_DECREMENT

%nonassoc T_EQUAL T_PLUS_EQUAL T_MINUS_EQUAL T_TIMES_EQUAL T_DIVIDE_EQUAL T_MODULUS_EQUAL

%token T_IS_EQUAL T_IS_NOT_EQUAL T_LESS_THAN T_LESS_EQUAL_THAN T_GREATER_THAN T_GREATER_EQUAL_THAN

%token T_LOGICAL_OR T_LOGICAL_AND T_NOT

%token T_BITWISE_AND T_BITWISE_OR T_BITWISE_XOR T_BITWISE_COMPLEMENT T_LSHIFT T_RSHIFT

%nonassoc T_LBRACKET T_CURLY_LBRACKET T_SQUARE_LBRACKET T_LHEADER T_RBRACKET T_CURLY_RBRACKET T_SQUARE_RBRACKET T_RHEADER

%left T_MINUS T_DIVIDE

%right T_PLUS T_TIMES

%right T_SEMICOLON T_COLON T_COMMA

%right T_NUMBER T_VARIABLE T_RETURN T_STRING

%token T_SIGNED T_GO_TO T_AUTO T_STRUCT 


%type <expr>  TERM FACTOR BINARY_EXPRESSION_TREE STATEMENT
%type <expr> C_EXPRESSION COMPARISON_OP LOGICAL_OP C_ARGS DECLARE_VAR FUNCTION_CALL PASSED_PARAMS G_ADDITIONAL_DECS L_ADDITIONAL_DECS C_INCREMENT_DECREMENT

//C_INCREMENT_DECREMENT, , EXPR, SCOPE_BODY

%type <node> PROGRAM EX_DECLARATION FUNCTION_DEC_DEF  GLOBAL_DECLARATION
%type <node> SCOPE  SCOPE_STATEMENTS STAT_SCOPE PARAMETER
%type <node>  T_IF T_ELSE T_WHILE T_FOR T_RETURN 
//%type <node> BINARY 

%type <number> T_NUMBER 
%type <string> T_VARIABLE T_VOID T_INT T_DOUBLE T_FLOAT TYPE_SPECIFY

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

ROOT: PROGRAM {g_root = $1;};

PROGRAM:
    EX_DECLARATION PROGRAM {$$ = new Program($1,$2);}
  | EX_DECLARATION         {$$ = new Program($1, NULL);} 
  ;   

EX_DECLARATION:
    FUNCTION_DEC_DEF   {$$ = $1;}
  | GLOBAL_DECLARATION {$$ = $1;}
  ;


GLOBAL_DECLARATION:
    TYPE_SPECIFY T_VARIABLE T_SEMICOLON                         {$$ = new GlobalVarDec(*$1,*$2,NULL);}
  | TYPE_SPECIFY T_VARIABLE T_EQUAL C_EXPRESSION T_SEMICOLON    {$$ = new GlobalVarDec(*$1,*$2,$4);}
  | TYPE_SPECIFY T_VARIABLE T_COMMA G_ADDITIONAL_DECS T_SEMICOLON {$$ = new MultipleDecs (*$1,*$2,$4, true);}
  | TYPE_SPECIFY T_VARIABLE T_SQUARE_LBRACKET T_NUMBER T_SQUARE_RBRACKET T_SEMICOLON {$$ = new GlobalArrayDec (*$1,*$2,$4);}
  // | TYPE_SPECIFY T_VARIABLE T_EQUAL G_ADDITIONAL_DECS T_SEMICOLON  {$$ = new MultipleDecs (*$1,*$2,$4, true);}
  ;

G_ADDITIONAL_DECS:
    T_VARIABLE T_EQUAL C_EXPRESSION T_COMMA G_ADDITIONAL_DECS       {$$ = new AdditionalDecs (*$1,$3,$5, true);}
  | T_VARIABLE T_COMMA G_ADDITIONAL_DECS                            {$$ = new AdditionalDecs (*$1,NULL,$3, true);}
  | T_VARIABLE T_EQUAL C_EXPRESSION                                 {$$ = new AdditionalDecs(*$1,$3,NULL,true);}
  | T_VARIABLE                                                      {$$ = new AdditionalDecs (*$1,NULL, NULL, true);}
  ;

FUNCTION_DEC_DEF: 
    TYPE_SPECIFY T_VARIABLE T_LBRACKET T_RBRACKET SCOPE         {$$ = new FunctionDec(*$1,*$2,NULL,$5);}
  | TYPE_SPECIFY T_VARIABLE T_LBRACKET C_ARGS T_RBRACKET SCOPE  {$$ = new FunctionDec(*$1,*$2,$4,$6);}
  | TYPE_SPECIFY T_VARIABLE T_LBRACKET T_RBRACKET T_CURLY_LBRACKET T_CURLY_RBRACKET {$$ = new FunctionDec(*$1,*$2,NULL,NULL);}

  | TYPE_SPECIFY T_VARIABLE T_LBRACKET T_RBRACKET T_SEMICOLON        {$$ = new FunctionDef(*$1,*$2, NULL);}
  | TYPE_SPECIFY T_VARIABLE T_LBRACKET C_ARGS T_RBRACKET T_SEMICOLON {$$ = new FunctionDef(*$1,*$2,$4);}
  ;


C_ARGS:
    PARAMETER T_COMMA C_ARGS {$$ = new Args($1,$3);}
  | PARAMETER                {$$ = new Args($1,NULL); }
  ;

PARAMETER: T_INT T_VARIABLE {$$= new Parameter(*$1,*$2);};

SCOPE: T_CURLY_LBRACKET SCOPE_STATEMENTS T_CURLY_RBRACKET  {$$ = new ScopeBody($2);} ;

SCOPE_STATEMENTS:
    STATEMENT SCOPE_STATEMENTS  { $$ = new ScopeStatements($1,$2);}
  | STATEMENT                   { $$ = new ScopeStatements($1,NULL);}
/*
  : STATEMENT                {$$ = new ScopeBody($1, NULL);}
  | STATEMENT SCOPE_BODY     {$$ = new ScopeBody($1, $2);}
  | DECLARE_VAR              {$$ = new ScopeBody($1,NULL);}
  | DECLARE_VAR SCOPE_BODY   {$$ = new ScopeBody($1,$2);}
*/
;

STATEMENT:
    T_RETURN LOGICAL_OP T_SEMICOLON                                                                    {$$ = new ReturnStatement($2);}
  | T_RETURN T_SEMICOLON                                                                               {$$ = new ReturnStatement(NULL);}
  | T_VARIABLE T_EQUAL C_EXPRESSION T_SEMICOLON                                                        {$$ = new AssignmentStatement(*$1,$3);}
  | T_VARIABLE T_SQUARE_LBRACKET C_EXPRESSION T_SQUARE_RBRACKET T_EQUAL C_EXPRESSION T_SEMICOLON       {$$ = new ArrayAssignment(*$1,$3,$6);}
  | T_IF T_LBRACKET LOGICAL_OP T_RBRACKET STAT_SCOPE                                                   {$$ = new IfElseStatement($3,$5,NULL);}
  | T_IF T_LBRACKET LOGICAL_OP T_RBRACKET STAT_SCOPE T_ELSE STAT_SCOPE                                 {$$ = new IfElseStatement($3, $5, $7);}
  | T_WHILE T_LBRACKET LOGICAL_OP T_RBRACKET STAT_SCOPE                                                {$$ = new WhileStatement($3,$5);}
  | T_DO STAT_SCOPE T_WHILE T_LBRACKET LOGICAL_OP T_RBRACKET T_SEMICOLON                               {$$ = new DoWhileStatement($5, $2);}
  | T_FOR T_LBRACKET STATEMENT LOGICAL_OP T_SEMICOLON C_INCREMENT_DECREMENT T_RBRACKET STAT_SCOPE      {$$ = new ForStatement($3, $4, $6, $8);}
  | T_BREAK T_SEMICOLON                                                                                {$$ = new BreakStatement();}
  | DECLARE_VAR  T_SEMICOLON                                                                           {$$ = $1;}
  | C_EXPRESSION T_SEMICOLON                                                                           {$$ = $1;}
  ;

STAT_SCOPE:
    STATEMENT {$$ = new NoBraces($1);}
  | SCOPE {$$=$1;}
  ;

  FUNCTION_CALL:
    T_VARIABLE T_LBRACKET PASSED_PARAMS T_RBRACKET  {$$ = new FunctionCall(*$1, $3);}
  | T_VARIABLE T_LBRACKET T_RBRACKET {$$ = new FunctionCall(*$1, NULL);}
  ;

PASSED_PARAMS:
    BINARY_EXPRESSION_TREE T_COMMA PASSED_PARAMS {$$ = new PassedParams($1, $3);}
  | BINARY_EXPRESSION_TREE                       {$$ = new PassedParams($1,NULL);}
  ;

DECLARE_VAR:
    TYPE_SPECIFY T_VARIABLE                        {$$ = new LocalVarDec (*$1,*$2,NULL);}
  | TYPE_SPECIFY T_VARIABLE T_EQUAL C_EXPRESSION   {$$ = new LocalVarDec (*$1,*$2,$4);}
  | TYPE_SPECIFY T_VARIABLE T_COMMA L_ADDITIONAL_DECS  {$$ = new MultipleDecs (*$1,*$2,$4,false);}
  | TYPE_SPECIFY T_VARIABLE T_SQUARE_LBRACKET T_NUMBER T_SQUARE_RBRACKET {$$ = new LocalArrayDec (*$1,*$2,$4);}
  // | TYPE_SPECIFY T_VARIABLE T_EQUAL L_ADDITIONAL_DECS  {$$ = new MultipleDecs (*$1,*$2,$4,false);}
  ;

L_ADDITIONAL_DECS:
  T_VARIABLE T_EQUAL C_EXPRESSION T_COMMA L_ADDITIONAL_DECS       {$$ = new AdditionalDecs (*$1,$3,$5, false);}
  | T_VARIABLE T_COMMA L_ADDITIONAL_DECS                            {$$ = new AdditionalDecs (*$1,NULL,$3, false);}
  | T_VARIABLE T_EQUAL C_EXPRESSION                                 {$$ = new AdditionalDecs(*$1,$3,NULL,false);}
  | T_VARIABLE                                                      {$$ = new AdditionalDecs (*$1,NULL, NULL, false);}
  ;

TYPE_SPECIFY:
    T_VOID    {$$ = $1;}
  | T_INT     {$$ = $1;}
  | T_DOUBLE  {$$ = $1;}
  | T_FLOAT   {$$ = $1;}
  ;

C_EXPRESSION:  BINARY_EXPRESSION_TREE {$$ = $1;};


BINARY_EXPRESSION_TREE:
    TERM T_PLUS C_EXPRESSION     { $$ = new AddOperator($1, $3);}
  | TERM T_MINUS C_EXPRESSION    { $$ = new SubOperator($1, $3);}
  | TERM                    { $$ = $1; }
  ;

TERM: 
  FACTOR T_TIMES TERM  { $$ = new MulOperator($1, $3);}
  | FACTOR T_DIVIDE TERM { $$ = new DivOperator($1, $3);}
  | FACTOR T_MODULUS TERM { $$ = new ModOperator($1, $3);}
  | FACTOR  { $$ = $1;}
  ;

C_INCREMENT_DECREMENT: 
    T_VARIABLE T_INCREMENT  {$$ = new PostIncrement(*$1);}
  | T_VARIABLE T_DECREMENT  {$$ = new PostDecrement(*$1);}
  | T_INCREMENT T_VARIABLE  {$$ = new PreIncrement(*$2);}
  | T_DECREMENT T_VARIABLE  {$$ = new PreDecrement(*$2);}
  ;

COMPARISON_OP: 
    BINARY_EXPRESSION_TREE T_LESS_THAN COMPARISON_OP { $$ = new LessThanOperator($1, $3);}
  | BINARY_EXPRESSION_TREE T_LESS_EQUAL_THAN COMPARISON_OP { $$ = new LessThanEqualOperator($1, $3);}
  | BINARY_EXPRESSION_TREE T_GREATER_THAN COMPARISON_OP { $$ = new GreaterThanOperator($1, $3);}
  | BINARY_EXPRESSION_TREE T_GREATER_EQUAL_THAN COMPARISON_OP { $$ = new GreaterThanEqualOperator($1, $3);}
  | BINARY_EXPRESSION_TREE T_IS_EQUAL COMPARISON_OP { $$ = new IsEqualOperator($1, $3);}
  | BINARY_EXPRESSION_TREE T_IS_NOT_EQUAL COMPARISON_OP { $$ = new IsNotEqualOperator($1, $3);}
  | BINARY_EXPRESSION_TREE { $$ = $1;}
  ;

LOGICAL_OP: 
  COMPARISON_OP T_LOGICAL_AND LOGICAL_OP { $$ = new LogicalAndOperator($1, $3);}
  | COMPARISON_OP T_LOGICAL_OR LOGICAL_OP { $$ = new LogicalOrOperator($1, $3);}
  | COMPARISON_OP               { $$ = $1; }
  ;

FACTOR: 
    T_VARIABLE         {$$ = new Variable(*$1);}
  | T_VARIABLE T_SQUARE_LBRACKET T_NUMBER T_SQUARE_RBRACKET {$$ = new ArrayElement(*$1,$3); }
  | T_NUMBER           {$$ = new Number( $1 );}
  | T_MINUS T_NUMBER   {$$ = new Number(-$2);}
  | FUNCTION_CALL      {$$ = $1;}
  | C_INCREMENT_DECREMENT {$$ = $1;}
  | T_LBRACKET C_EXPRESSION T_RBRACKET {$$ = new BracketedExpr($2);}
  ;


/*TODO

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