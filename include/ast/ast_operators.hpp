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
        {
        }

        virtual double evaluate(
            const std::map<std::string, double> &bindings) const override
        {
            // TODO-C : Run bin/eval_expr with something like 5+a, where a=10, to make sure you understand how this works
            double vl = left->evaluate(bindings);
            double vr = right->evaluate(bindings);
            return vl + vr;

            //throw std::runtime_error("AddOperator::evaluate is not implemented.");
        }
    
    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        std::vector<int> freeregs = contxt.FreeTempRegs(); //finds available registers
        contxt.set_used(freeregs[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeregs[0]);
        dst << "\t"<< "addu"<< "\t"<< "$" << destReg << ", $" << destReg << ", $" << freeregs[0] <<"\t#Add operator"<< std::endl;
        contxt.set_unused(freeregs[0]);
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
    
    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override 
    {
        // TODO-D : Implement this, based on AddOperator::evaluate

        double vl=left->evaluate(bindings);
        double vr=right->evaluate(bindings);
        return vl-vr;

        //throw std::runtime_error("SubOperator::evaluate is not implemented.");
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        std::vector<int> freeregs = contxt.FreeTempRegs(); //finds available registers
        contxt.set_used(freeregs[0]);                      //locks the registers for use of the function
        
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeregs[0]);
        dst << "\t"<< "sub"<< "\t"<< "$" << destReg << ", $" << destReg << ", $" << freeregs[0] <<"\t#Sub Operator"<< std::endl;
        contxt.set_unused(freeregs[0]);
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

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        std::vector<int> freeregs = contxt.FreeTempRegs(); //finds available registers
        contxt.set_used(freeregs[0]);                      //locks the registers for use of the function
        
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeregs[0]);
        dst << "\t"<<"mult"<<"\t"<< "$" << destReg << ", $" << freeregs[0] <<"\t#Multiply Operator"<< std::endl;
        dst << "\t"<<"mflo"<<"\t"<<"$"<<destReg<<"\t#Store result of multiply"<<std::endl;
        contxt.set_unused(freeregs[0]);
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
