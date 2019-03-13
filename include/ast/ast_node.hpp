#ifndef ast_node_hpp
#define ast_node_hpp

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <memory>

static std::vector<std::string> global_vars;
static int var_count;
static int scopelevel;
static std::vector<std::string> function_call_queue;

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
    { throw std::runtime_error("Evaluate not implemented."); }

    virtual void translate(std::ostream &dst) const {
        throw std::runtime_error("Translate not implemented.");
    }
};


#endif