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

        int stack_end = (var_count*4) +parameter_count+12+50; 

        std::vector<int> freeArgReg = contxt.FindFreeRegs(4, 7);

        if(var_offset != 0){  //if (var_offset == 0)
            //looks for local variables in the scopes
            dst<<"\t"<<"lw"<<"\t"<<"$"<<destReg<<", "<<var_offset<<"($fp)"<<std::endl;
            dst<<"\t"<<"nop"<<std::endl;
        }
        else if(contxt.find_parameter(id) > -1){
            // for(int i =0; i<4; i++){
            //     contxt.set_used(destReg+i);
            //     dst<<"\t"<<"lw"<<"\t"<<"$"<<(destReg+i)<<", "<<stack_end+(4*i)<<"($fp)"<<"\t"<<"#loading param argument register"<<std::endl;
            // }
            dst<<"\t"<<"lw"<<"\t"<<"$"<<destReg<<", "<<(contxt.find_parameter(id))*4+stack_end<<"($fp)";
            dst<<"\t #found variable "<<id<<" in parameters"<<std::endl;
            dst<<"\t"<<"nop"<<std::endl;
        }
        else{
            //loads global variables
            dst<<"\t"<<"lui"<<"\t"<<"$"<<destReg<<", "<<"%hi("<<id<<")"<<"\t#Loading in global variable: "<<id<<std::endl;
            dst<<"\t"<<"lw"<<"\t"<<"$"<<destReg<<", "<<"%lo("<<id<<")($"<<destReg<<")"<<std::endl;
            dst<<"\t"<<"nop"<<"\t#Global variable load nop"<<std::endl;
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
