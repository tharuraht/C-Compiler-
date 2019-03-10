#ifndef ast_node_hpp
#define ast_node_hpp

#include <string>
#include <iostream>
#include <map>

#include <memory>

class AST_node;

typedef const AST_node *NodePtr;

class AST_node
{
public:
    virtual ~AST_node()
    {}

    //! Tell a node to print itself to the given stream
    virtual void print(std::ostream &dst) const =0;

    //! Evaluate the tree using the given mapping of variables to numbers
    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const
    { throw std::runtime_error("Not implemented."); }

    virtual void translate(std::ostream &dst) const {
        throw std::runtime_error("Translate not implemented.");
    }
};


#endif
