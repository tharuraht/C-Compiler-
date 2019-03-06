#ifndef ast_dec_def_hpp
#define ast_dec_def_hpp

#include <iostream>
#include "ast.hpp"

extern std::vector<std::string> global_vars;
extern int var_count;

class FunctionDef: public AST_node {
    public:
    std::string Type;
    std::string Identifier;

    FunctionDef (std::string _Type, std::string _Identifier) : Type(_Type), Identifier(_Identifier) {}
    ~FunctionDef () {}

    virtual void print(std::ostream &dst) const override {
        dst <<Type<<" "<<Identifier<<" ();"<<std::endl;
    }
};

class FunctionDec: public AST_node {
    public:
    std::string Type;
    std::string Identifier;
    NodePtr Arguments;
    NodePtr Scope;

    FunctionDec(std::string _Type, std::string _Identifier, NodePtr _Arguments, NodePtr _Scope) 
    : Type(_Type), Identifier(_Identifier), Arguments(_Arguments), Scope(_Scope) {}
    ~FunctionDec() {}

    virtual void print(std::ostream &dst) const override {
        dst << Type << " " << Identifier << " (";
        if (Arguments != NULL) {
            Arguments->print(dst);
        }
        dst<<") {" <<std::endl;
        if (Scope != NULL) {
            Scope->print(dst);
            dst<<std::endl;
        }
        dst<<" }" << std::endl;
    }
};

class GlobalVarDec: public AST_node {
    public:
    std::string Type;
    std::string Name;
    ExpressionPtr Expression;

    GlobalVarDec (std::string _Type, std::string _Name, ExpressionPtr _Expression) : Type(_Type),Name(_Name),Expression(_Expression) {
        global_vars.push_back(Name)
        var_count++;
    }
    ~GlobalVarDec () {}

    virtual void print(std::ostream &dst) const override {
        dst << Type << " " << Name;
        if (Expression != NULL)
        {
            dst << " = ";
            Expression->print(dst);
        }
        dst<<";";
    }

    virtual void translate(std::ostream &dst) const override {
        dst<< Name;
        if (Expression != NULL)
        {
            dst<<"=";
            Expression->print(dst);
        }
        else {
            dst<<"=0";
        }
    }
};

class LocalVarDec : public AST_node
{
  public:
    std::string Type;
    std::string Name;
    ExpressionPtr Expression;

    GlobalVarDec(std::string _Type, std::string _Name, ExpressionPtr _Expression) : Type(_Type), Name(_Name), Expression(_Expression) {}
    ~GlobalVarDec() {}

    virtual void print(std::ostream &dst) const override
    {
        dst << Type << " " << Name;
        if (Expression != NULL)
        {
            dst << " = ";
            Expression->print(dst);
        }
        dst << ";";
    }

    virtual void translate(std::ostream &dst) const override
    {
        dst << Name;
        if (Expression != NULL)
        {
            dst << "=";
            Expression->print(dst);
        }
        else
        {
            dst << "=0";
        }
    }
};

#endif