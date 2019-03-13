#ifndef ast_functions_hpp
#define ast_functions_hpp

#include "ast_expression.hpp"
#include <cmath>

extern std::vector<std::string> function_call_queue;

class FunctionCall : public Expression
{
  private:
    std::string name;
    ExpressionPtr arg;


  public:
    FunctionCall(std::string _name, ExpressionPtr _arg) : name(_name), arg(_arg) {
        function_call_queue.push_back(name);
    }
    ~FunctionCall() {}

    virtual void print(std::ostream &dst) const override {
        dst << name << "(";
        if (arg != NULL) {
            arg->print(dst);
        }
        
        dst << ")";
    }

    virtual void translate(std::ostream &dst) const {
        dst << name << "(";
        if (arg != NULL) {
            arg->translate(dst);
        }

        dst << ")";
    }
};

class Args: public Expression {
private:
    NodePtr SingleParameter;
    NodePtr OtherParameters;
public:
    ~Args() {}

    Args(NodePtr _SingleParameter, NodePtr _OtherParameters) : SingleParameter(_SingleParameter), OtherParameters(_OtherParameters) {}

    virtual void print (std::ostream &dst) const override {
        SingleParameter->print(dst);
        dst<<",";
        if (OtherParameters != NULL) {
            OtherParameters->print(dst);
        }
    }

    virtual void translate (std::ostream &dst) const override {
        SingleParameter->translate(dst);
        dst << ",";
        if (OtherParameters != NULL) {
            OtherParameters->translate(dst);
        }
    }
};

class Parameter: public Expression {
private:
    std::string Type;
    std::string Identifier;

public:
    ~Parameter () {}

    Parameter(std::string _Type, std::string _Identifier) : Type(_Type), Identifier(_Identifier) {}

    virtual void print (std::ostream &dst) const override {
        dst << Type <<" "<< Identifier;
    }

    virtual void translate (std::ostream &dst) const override {
        dst<< Identifier;
    }
};

class Function: public Expression {
private:
    ExpressionPtr arg;
protected:
    Function(ExpressionPtr _arg): arg(_arg) {}
public:
    virtual const char * getFunction() const =0;

    ExpressionPtr getArg() const
    { return arg; }

    virtual void print(std::ostream &dst) const override
    {
        dst<<getFunction()<<"( ";
        arg->print(dst);
        dst<<" )";
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        // NOTE : This should be implemented by the inheriting function nodes, e.g. LogFunction
        throw std::runtime_error("FunctionOperator::evaluate is not implemented.");
    }
};

class LogFunction
    : public Function
{
public:
    LogFunction(ExpressionPtr _arg)
        : Function(_arg)
    {}

    virtual const char *getFunction() const
    { return "log"; }
    
    // TODO-E : Override evaluate, and implement it

    virtual double evaluate(
        const std::map<std::string, double> &bindings
    ) const override 
    {
        double l_arg = getArg()->evaluate(bindings);
        return log(l_arg);
    }


};

class ExpFunction
    : public Function
{
public:
    ExpFunction(ExpressionPtr _arg)
        : Function(_arg)
    {}

    virtual const char *getFunction() const
    { return "exp"; }

    virtual double evaluate(
        const std::map<std::string, double> &bindings
    ) const override {
        double e_arg = getArg()->evaluate(bindings);
        return exp(e_arg);
    }

};

class SqrtFunction
    : public Function
{
public:
    SqrtFunction(ExpressionPtr _arg)
        : Function(_arg)
    {}

    virtual const char *getFunction() const
    { return "sqrt"; }

    virtual double evaluate(
        const std::map<std::string, double> &bindings
    ) const override {
        double s_arg = getArg()->evaluate(bindings);
        return sqrt(s_arg);
    }
};


#endif
