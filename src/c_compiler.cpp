#include "../include/ast.hpp"
#include <stdio.h>
#include <string>


int main(int argc, char *argv[])
{
  //Lexer debugging
  extern int yy_flex_debug;
  yy_flex_debug = 0;  //set to 1 to debug

	freopen (argv[2],"r",stdin);

  const AST_node *ast=parseAST();
	std::cout<< std::endl;

	freopen (argv[4],"w",stdout);
	std::string test = argv[1];
	if (test == "--translate") {
	  ast->translate(std::cout);
    std::cout<<std::endl;
	}
return 0;
}