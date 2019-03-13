#ifndef ast_dec_def_hpp
#define ast_dec_def_hpp

#include <iostream>
#include <vector>
#include <string>
#include "ast.hpp"

extern std::vector<std::string> global_vars;
extern int var_count;
extern int scopelevel;

class Program: public AST_node {
private:
    NodePtr ExDec;
    NodePtr Rest_of_program;

public:
    ~Program() {}

    Program (NodePtr _ExDec, NodePtr _Rest_of_program) : ExDec(_ExDec), Rest_of_program(_Rest_of_program) {}

    virtual void print (std::ostream &dst) const override {
        ExDec->print(dst);
        if (Rest_of_program != NULL) {
            Rest_of_program->print(dst);
        }
    }

    virtual void translate (std::ostream &dst) const override {
        ExDec->translate(dst);
        if (Rest_of_program != NULL) {
            Rest_of_program->translate(dst);
        }
    }
};

class FunctionDef: public AST_node {
public:
    std::string Type;
    std::string Identifier;
    NodePtr Arguments;

    FunctionDef (std::string _Type, std::string _Identifier, NodePtr _Arguments) : Type(_Type), Identifier(_Identifier), Arguments(_Arguments) {}
    ~FunctionDef () {}

    virtual void print(std::ostream &dst) const override {
        dst <<Type<<" "<<Identifier<<"(";
        if (Arguments != NULL) {
            Arguments->print(dst);
        }
        dst<<");"<<std::endl;
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
        dst << Type << " " << Identifier << "(";
        if (Arguments != NULL) {
            Arguments->print(dst);
        }
        dst<<") " <<std::endl;
        if (Scope != NULL) {
            Scope->print(dst);
        }
    }

    virtual void translate (std::ostream &dst) const override {
        dst << "def " << Identifier << "(";
        if (Arguments != NULL) {
            Arguments->translate(dst);
        }
        dst << "): ";
        for (int i=0; i< global_vars.size();i++) {
            dst<<std::endl;
            for (int i = 0; i < scopelevel; i++) {
                dst << "\t";
            }
            dst<<"\tglobal "<<global_vars[i];
        }
        if (Scope != NULL) {
            Scope->translate(dst);
        }

        //tells python to invoke the function if it is the main function
        if (Identifier == "main") {
            dst<<std::endl;
            dst<< "if __name__ == \"__main__\": "<<std::endl;
            dst<<"\timport sys"<<std::endl;
            dst<<"\tret=main()"<<std::endl;
            dst<<"\tsys.exit(ret)"<<std::endl;
        }
    }
};

class GlobalVarDec: public Expression {
    public:
    std::string Type;
    std::string Name;
    ExpressionPtr Expression;

    GlobalVarDec (std::string _Type, std::string _Name, ExpressionPtr _Expression) : Type(_Type),Name(_Name),Expression(_Expression) {
        global_vars.push_back(Name);
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
        dst<<";"<<std::endl;
    }

    virtual void translate(std::ostream &dst) const override {
        dst<< Name;
        if (Expression != NULL)
        {
            dst<<"=";
            Expression->translate(dst);
        }
        else {
            dst<<"=0";
        }
        dst<<std::endl;
    }
};

class LocalVarDec : public Expression
{
  public:
    std::string Type;
    std::string Name;
    ExpressionPtr Expression;

    LocalVarDec(std::string _Type, std::string _Name, ExpressionPtr _Expression)
        : Type(_Type), Name(_Name), Expression(_Expression) 
        { var_count++; }

    ~LocalVarDec() {}

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
            dst << " = ";
            Expression->translate(dst);
        }
        else
        {
            dst << "=0";
        }
    }
};



#endif