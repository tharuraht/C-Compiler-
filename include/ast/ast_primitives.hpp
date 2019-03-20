#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>
extern bool varGlobal;

class Variable: public Expression {
private:
    std::string id;
public:
    Variable(const std::string &_id)
        : id(_id)
    {}

    const std::string getId() const
    { return id; }

    virtual void print(std::ostream &dst) const override
    {
        dst<<id;
    }

    virtual void translate(std::ostream &dst) const {
        dst<<id;
    }


    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        // TODO-B : Run bin/eval_expr with a variable binding to make sure you understand how this works.
        // If the binding does not exist, this will throw an error
        return bindings.at(id);
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        int var_offset = contxt.LookupVariable(id, scopelevel);
        // std::cout<<"VAR OFFSET: "<<var_offset<<std::endl;

        if(var_offset != 0){  //if (var_offset == 0)
            //looks for local variables in the scopes
            dst<<"\t"<<"lw"<<"\t"<<"$"<<destReg<<", "<<var_offset<<"($fp)"<<std::endl;
        }
        else{
            //loads global variables
            dst<<"\t"<<"lui"<<"\t"<<"$"<<destReg<<", "<<"%hi("<<id<<")"<<"\t#Loading in global variable"<<std::endl;
            dst<<"\t"<<"lw"<<"\t"<<"$"<<destReg<<", "<<"%lo("<<id<<")($2)"<<std::endl;
        }
    }   
};

class Number: public Expression
{
private:
    double value;
public:
    Number(double _value)
        : value(_value)
    {}

    double getValue() const
    { return value; }

    virtual void print(std::ostream &dst) const override
    {
        dst<<value;
    }

    virtual void translate(std::ostream &dst) const {
        dst<<value;
    }


    // virtual double evaluate(
    //     const std::map<std::string,double> &bindings
    // ) const override
    // {
    //     // TODO-A : Run bin/eval_expr with a numeric expression to make sure you understand how this works.
    //     return value;
    // }
    virtual int evaluate () const override {
        return value;
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        if (varGlobal) {
            dst<<value<<"\t#Global variable"<<std::endl;
        }
        else if (!varGlobal) {
            dst <<"\t"<< "li" << "\t" << "$"<<destReg<<", " << value<<std::endl;
        }
	}
};


#endif
