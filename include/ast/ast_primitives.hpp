#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>

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

    virtual void compile(std::ostream &dst, Context &contxt) const override
    {
        int var_offset = contxt.LookupVariable(id, scopelevel);

        if(var_offset){  //if (var_offset == 0)
            
            dst<<"\t"<<"lui"<<"\t"<<"$2, "<<"%hi("<<id<<")"<<std::endl;
            dst<<"\t"<<"lw"<<"\t"<<"$2, "<<"%lo("<<id<<")($2)"<<std::endl;
        }
        else{
            //load whole word into register two
            dst<<"\t"<<"lw"<<"\t"<<"$2, "<<var_offset<<"($fp)"<<std::endl;
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


    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        // TODO-A : Run bin/eval_expr with a numeric expression to make sure you understand how this works.
        return value;
    }

    virtual void compile(std::ostream &dst, Context &contxt) const override {
		dst <<"\t"<< "li" << "\t" << "$2, " << value<<std::endl;
	}
};


#endif
