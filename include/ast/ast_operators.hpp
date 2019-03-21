#ifndef ast_operators_hpp
#define ast_operators_hpp

#include <string>
#include <iostream>
#include <cmath>

class Operator
    : public Expression
{
protected:
    ExpressionPtr left;
    ExpressionPtr right;

    Operator(ExpressionPtr _left, ExpressionPtr _right)
        : left(_left)
        , right(_right)
    {}
public:
    virtual const char *getOpcode() const =0;

    ExpressionPtr getLeft() const
    { return left; }

    ExpressionPtr getRight() const
    { return right; }

    virtual void print(std::ostream &dst) const override
    {
        // dst<<"( ";
        left->print(dst);
        
        dst<<getOpcode();
        
        right->print(dst);
    //    dst<<" )";
    }

    virtual void translate(std::ostream &dst) const {
        left ->translate(dst);
        dst << " ";
        dst << getOpcode();
        dst << " ";
        right->translate(dst);
    }

};

    class AddOperator
        : public Operator
    {
      protected:
        virtual const char *getOpcode() const override
        {
            return "+";
        }

      public:
        AddOperator(ExpressionPtr _left, ExpressionPtr _right)
            : Operator(_left, _right)
        { }

        // virtual double evaluate(
        //     const std::map<std::string, double> &bindings) const override
        // {
        //     // TODO-C : Run bin/eval_expr with something like 5+a, where a=10, to make sure you understand how this works
        //     double vl = left->evaluate(bindings);
        //     double vr = right->evaluate(bindings);
        //     return vl + vr;

        //     //throw std::runtime_error("AddOperator::evaluate is not implemented.");
        // }

        virtual int evaluate () const override
        {
            int vl = left->evaluate();
            int vr = right->evaluate();
            return vl + vr;
        }

        virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
        {
            if (varGlobal)
            {
                int vl = left->evaluate();
                int vr = right->evaluate();
                dst << vl + vr;
            }
            else
            {
                std::vector<int> freeregs = contxt.FreeTempRegs(); //finds available registers
                contxt.set_used(freeregs[0]);                      //locks the registers for use of the function
                contxt.set_used(freeregs[1]);
                int current_function_depth = function_call_num;
                std::cout<<"#function call depth: "<<current_function_depth<<std::endl;
                left->compile(dst, contxt, freeregs[0]);
                if (current_function_depth != function_call_num) {
                    dst<<"\t"<<"move"<<"\t"<<"$"<<freeregs[0]<<", $"<<destReg<<"\t#Function call result"<<std::endl;
                }
                current_function_depth = function_call_num;

                right->compile(dst, contxt, freeregs[1]);
                if (current_function_depth != function_call_num) {
                    dst<<"\t"<<"move"<<"\t"<<"$"<<freeregs[1]<<", $"<<destReg<<"\t#Function call result"<<std::endl;
                }
                dst << "\t"<< "addu"<< "\t"<< "$" << destReg << ", $" << freeregs[0] << ", $" << freeregs[1] << "\t#Add operator" << std::endl;
                contxt.set_unused(freeregs[0]);
                contxt.set_unused(freeregs[1]);
            }
    }

};

class SubOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "-"; }
public:
    SubOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}
    
    // virtual double evaluate(
    //     const std::map<std::string,double> &bindings
    // ) const override 
    // {
    //     // TODO-D : Implement this, based on AddOperator::evaluate

    //     double vl=left->evaluate(bindings);
    //     double vr=right->evaluate(bindings);
    //     return vl-vr;

    //     //throw std::runtime_error("SubOperator::evaluate is not implemented.");
    // }
    virtual int evaluate() const override
    {
        int vl = left->evaluate();
        int vr = right->evaluate();
        return vl * vr;
        
    }
    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        if (varGlobal) {
            int vl = left->evaluate();
            int vr = right->evaluate();
            dst<<vl - vr;
        }
        else {
            std::vector<int> freeregs = contxt.FreeTempRegs(); //finds available registers
            contxt.set_used(freeregs[0]);                      //locks the registers for use of the function
            
            left->compile(dst, contxt, destReg);
            right->compile(dst, contxt, freeregs[0]);
            dst << "\t"<< "sub"<< "\t"<< "$" << destReg << ", $" << destReg << ", $" << freeregs[0] <<"\t#Sub Operator"<< std::endl;
            contxt.set_unused(freeregs[0]);
        }
    }
};


class MulOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "*"; }
public:
    MulOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    // virtual double evaluate(
    //     const std::map<std::string,double> &bindings
    // ) const override
    // {
    //     //throw std::runtime_error("MulOperator::evaluate is not implemented.");

    //     double vl=left->evaluate(bindings);
    //     double vr=right->evaluate(bindings);
    //     return vl*vr;
    // }
    virtual int evaluate () const override
    {
        int vl = left->evaluate();
        int vr = right->evaluate();
        return vl * vr;
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        if (varGlobal) {
            int vl = left->evaluate();
            int vr = right->evaluate();
            dst<<vl * vr;
        }
        else {
            std::vector<int> freeregs = contxt.FreeTempRegs(); //finds available registers
            contxt.set_used(freeregs[0]);                      //locks the registers for use of the function
            contxt.set_used(freeregs[1]);
            int current_function_depth = function_call_num;
            std::cout << "#function call depth: " << current_function_depth << std::endl;

            left->compile(dst, contxt, freeregs[0]);
            if (current_function_depth != function_call_num) {
                    dst<<"\t"<<"move"<<"\t"<<"$"<<freeregs[0]<<", $"<<destReg<<"\t#Function call result"<<std::endl;
                }
            current_function_depth = function_call_num;

            right->compile(dst, contxt, freeregs[1]);
            if (current_function_depth != function_call_num) {
                    dst<<"\t"<<"move"<<"\t"<<"$"<<freeregs[1]<<", $"<<destReg<<"\t#Function call result"<<std::endl;
                }

            dst << "\t"<<"mult"<<"\t"<< "$" << freeregs[0] << ", $" << freeregs[1] <<"\t#Multiply Operator"<< std::endl;
            dst << "\t"<<"mflo"<<"\t"<<"$"<<destReg<<"\t#Store result of multiply"<<std::endl;
            contxt.set_unused(freeregs[0]);
            contxt.set_unused(freeregs[1]);

        }
    }
};

class DivOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "/"; }
public:
    DivOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        //throw std::runtime_error("DivOperator::evaluate is not implemented.");

        double vl=left->evaluate(bindings);
        double vr=right->evaluate(bindings);
        return vl/vr;
    }
};

class ModOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "%"; }
public:
    ModOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        //throw std::runtime_error("ModOperator::evaluate is not implemented.");

        int vl=left->evaluate(bindings);
        int vr=right->evaluate(bindings);
        return vl%vr;
    }
};

// class ExpOperator
//     : public Operator
// {
// protected:
//     virtual const char *getOpcode() const override
//     { return "^"; }
// public:
//     ExpOperator(ExpressionPtr _left, ExpressionPtr _right)
//         : Operator(_left, _right)
//     {}

//     virtual double evaluate(
//         const std::map<std::string,double> &bindings
//     ) const override
//     {
//         //throw std::runtime_error("ExpOperator::evaluate is not implemented.");

//         double vl=left->evaluate(bindings);
//         double vr=right->evaluate(bindings);
//         return pow(vl, vr);
//     }
// };

class LessThanOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "<"; }
public:
    LessThanOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl=left->evaluate(bindings);
        double vr=right->evaluate(bindings);
        return (vl<vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeregs = contxt.FreeTempRegs(); //finds available registers
        contxt.set_used(freeregs[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeregs[0]);
        dst << "\t"<< "slt"<< "\t"<< "$" << destReg << ", $" << destReg << ", $" << freeregs[0] << "\t#< operator" << std::endl;
        contxt.set_unused(freeregs[0]);
    }
};

class LessThanEqualOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "<="; }
public:
    LessThanEqualOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl=left->evaluate(bindings);
        double vr=right->evaluate(bindings);
        return (vl<=vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeregs = contxt.FreeTempRegs(); //finds available registers
        contxt.set_used(freeregs[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeregs[0]);
        dst << "\t"<< "slt"<< "\t"<< "$" << destReg << ", $" << destReg << ", $" <<freeregs[0]<< "\t#<= operator" << std::endl;
        dst<<"\t"<<"xori"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", 1"<<"\t#>= operator" << std::endl;

        contxt.set_unused(freeregs[0]);
    }
};


class GreaterThanOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return ">"; }
public:
    GreaterThanOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl=left->evaluate(bindings);
        double vr=right->evaluate(bindings);
        return (vl>vr);
    }
    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeregs = contxt.FreeTempRegs(); //finds available registers
        contxt.set_used(freeregs[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeregs[0]);
        dst << "\t"<< "slt"<< "\t"<< "$" << destReg << ", $" << freeregs[0] << ", $" << destReg << "\t#> operator" << std::endl;
        contxt.set_unused(freeregs[0]);
    }
};

class GreaterThanEqualOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return ">="; }
public:
    GreaterThanEqualOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl=left->evaluate(bindings);
        double vr=right->evaluate(bindings);
        return (vl>=vr);
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeregs = contxt.FreeTempRegs(); //finds available registers
        contxt.set_used(freeregs[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeregs[0]);
        dst<<"\t"<< "slt"<<"\t"<< "$" << destReg << ", $" << destReg << ", $" << freeregs[0] << "\t#>= operator" << std::endl;
        dst<<"\t"<<"xori"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", 1"<<"\t#>= operator" << std::endl;
        contxt.set_unused(freeregs[0]);
    }
};

class IsEqualOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "=="; }
public:
    IsEqualOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl=left->evaluate(bindings);
        double vr=right->evaluate(bindings);
        return (vl==vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeregs = contxt.FreeTempRegs(); //finds available registers
        contxt.set_used(freeregs[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeregs[0]);
     
        //checks equivalence, if they are the same will result in zero
        dst<<"\t"<<"xor"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", $"<<freeregs[0]<<"\t#== operator" << std::endl;
        //hence if if zero, set destreg to 1 (since they are equal) but 0 otherwise
        dst<<"\t"<<"sltiu"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", 1"<<std::endl;
        contxt.set_unused(freeregs[0]);
    }
};

class IsNotEqualOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "!="; }
public:
    IsNotEqualOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl=left->evaluate(bindings);
        double vr=right->evaluate(bindings);
        return (vl!=vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeregs = contxt.FreeTempRegs(); //finds available registers
        contxt.set_used(freeregs[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeregs[0]);
     
        //checks equivalence, if they are the same will result in zero
        dst<<"\t"<<"xor"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", $"<<freeregs[0]<<"\t#!= operator" << std::endl;
        //esnures that if the result is non zero it becomes 1, hence compare with zero register (a non zero will be greater)
        dst<<"\t"<<"sltu"<<"\t"<<"$"<<destReg<<", $0"<<", $"<<destReg<<std::endl;
        contxt.set_unused(freeregs[0]);
    }
};

class LogicalAndOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "&&"; }
public:
    LogicalAndOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl=left->evaluate(bindings);
        double vr=right->evaluate(bindings);
        return (vl&&vr);
    }

    virtual void translate (std::ostream &dst) const override {
        left->translate(dst);
        dst << " and ";
        right->translate(dst);
    }
};

class LogicalOrOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "||"; }
public:
    LogicalOrOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual double evaluate(const std::map<std::string,double> &bindings) const override
    {
        double vl=left->evaluate(bindings);
        double vr=right->evaluate(bindings);
        return (vl||vr);
    }

    virtual void translate (std::ostream &dst) const override {
        left->translate(dst);
        dst << " or ";
        right->translate(dst);
    }
};

#endif
