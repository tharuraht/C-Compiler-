#include "ast.hpp"

int main()
{
    //Lexer debugging
    extern int yy_flex_debug;
    yy_flex_debug = 0;
    const AST_node *ast=parseAST();

    ast->print(std::cout);
    std::cout<<std::endl;

    
    return 0;
}
