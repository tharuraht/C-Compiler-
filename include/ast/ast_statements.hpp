#ifndef ast_statements_hpp
#define ast_statements_hpp

#include "ast.hpp"

extern int scopelevel;
extern int loop_count;
extern int if_level;

class AssignmentStatement: public Expression {
public:
    std::string VarName;
    ExpressionPtr Expression;

    ~AssignmentStatement() {}

    AssignmentStatement (std::string _VarName, ExpressionPtr _Expression)
     : VarName(_VarName), Expression(_Expression) {}

    virtual void print(std::ostream &dst) const override
    {
        dst << VarName << " = ";
        Expression->print(dst);
        dst << ";";
    }

    virtual void translate(std::ostream &dst) const override
    {
        dst << VarName << " = ";
        Expression->translate(dst);         
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        //find a free register
        std::vector<int> freeRegs = contxt.FreeTempRegs();
        contxt.set_used(freeRegs[0]);
        Expression->compile(dst, contxt, freeRegs[0]);
        //store result into variable
        dst<<"\t"<<"sw"<<"\t"<<"$"<<freeRegs[0]<<", "<<contxt.LookupVariable(VarName, scopelevel)<<"($fp)"<<"\t#Assign variable "<<VarName<<std::endl;

        contxt.set_unused(freeRegs[0]);
    }
};

class ReturnStatement : public Expression {
public:
    ExpressionPtr AdditionalExpressions;

    ~ReturnStatement() {}

    ReturnStatement (ExpressionPtr _AdditionalExpressions): AdditionalExpressions(_AdditionalExpressions) {}

    virtual void print(std::ostream &dst) const override {
        dst<<"return";
        if (AdditionalExpressions != NULL) {
            dst<<" ";
            AdditionalExpressions->print(dst);
            dst<<";";
        }
        else {
            dst<<";";
        }
    }

    virtual void translate(std::ostream &dst) const override {
        dst << "return";
        if (AdditionalExpressions != NULL) {
            dst << " ";
            AdditionalExpressions->translate(dst);
        }
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {

        if (AdditionalExpressions != NULL) {
            AdditionalExpressions->compile(dst, contxt, destReg);
            //branch to end of function label
        }
        else{
            // dst<<"\t"<<"move"<<"\t"<<"$sp, $fp"<<std::endl; //deallocating stack
            // dst<<"\t"<<"lw"<<"\t"<<"$31,"<<(var_count*4)+8<<"($sp)"<<std::endl;
            // dst<<"\t"<<"lw"<<"\t"<<"$fp,"<<(var_count*4)+4<<"($sp)"<<std::endl; //old fp = top of stack address - 4
            // dst<<"\t"<<"addiu"<<"\t"<<"$sp, $sp,"<<(var_count*4)+8<<std::endl; //restoring sp
            // dst<<"\t"<<"j"<<"\t"<<"$31"<<std::endl;
            // dst<<"\t"<<"nop"<<std::endl;
            // dst<<std::endl;
            
        }
        dst<<"\t"<<"b "<<function_def_queue.back()<<"_function_end_"<<function_def_num<<"\t#Return statement"<<std::endl;

    }
};

class IfElseStatement : public Expression {
private:
    ExpressionPtr Condition;
    NodePtr IBody;
    NodePtr EBody;

public:
    ~IfElseStatement () {}

    IfElseStatement (ExpressionPtr _Condition, NodePtr _IBody, NodePtr _EBody) : Condition(_Condition), IBody(_IBody), EBody(_EBody) {}

    virtual void print (std::ostream &dst) const override {
        dst << "if (";
        Condition->print(dst);
        dst<<") ";
        IBody->print(dst);
        if (EBody != NULL) {
            dst<<std::endl;
            for (int i = 0; i < scopelevel; i++) {dst<<"\t";}
            dst<<"else";
            EBody->print(dst);
        }
        
    }

    virtual void translate(std::ostream &dst) const override {
        dst<< "if(";
        Condition->translate(dst);
        dst<<"):";
        IBody->translate(dst);

        if(EBody != NULL){
            dst<<std::endl;
            for(int i = 0; i < scopelevel; i++){dst<<"\t";}
            dst<<"else:";
            EBody->translate(dst);
        }
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> FreeReg = contxt.FindFreeRegs(16, 24);
        contxt.set_used(FreeReg[0]);

        Condition->compile(dst, contxt, FreeReg[0]);

        dst << "\t"<<"beq"<<"\t" << "$0, $" << FreeReg[0] << ", else_"<<if_level << std::endl; //$else condition yet to be filled
		dst << "\t"<<"nop"<<"\t" << std::endl;
		contxt.set_unused(FreeReg[0]);

        if(IBody != NULL){
            if_level++;
            IBody->compile(dst, contxt, destReg);
            if_level--;
        }

        dst<<"\t"<<"b"<<"\t"<<"ifelse_end_"<<if_level<<std::endl;

        dst<<"else_"<<if_level<<":"<<std::endl;
        if(EBody != NULL){
            if_level++;
            EBody->compile(dst, contxt, destReg);
            if_level--;
        }

        dst<<"ifelse_end_"<<if_level<<":"<<std::endl;
    }
};

class WhileStatement: public Expression {
private:
    ExpressionPtr Condition;
    NodePtr Body;

public:
    ~WhileStatement() {}

    WhileStatement (ExpressionPtr _Condition, NodePtr _Body) : Condition(_Condition) , Body(_Body) {}

    virtual void print (std::ostream &dst) const override {
        dst << "while(";
        Condition->print(dst);
        dst <<") ";
        Body->print(dst);
    }

    virtual void translate (std::ostream &dst) const override {
        dst<< "while(";
        Condition->translate(dst);
        dst<<"): ";
        Body->translate(dst);
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override {
        //use a free register for condition check
        std::vector<int> freeRegs = contxt.FreeTempRegs();
        contxt.set_used(freeRegs[0]);

        dst<<"while_loop_"<<loop_count<<"_begin:"<<"\t#Begin while loop"<<std::endl;
        
        //evalute the condition into the free register
        Condition->compile(dst, contxt, freeRegs[0]);
        //branch to end if condition evaluates false (0)
        dst<<"\t"<<"beq"<<"\t"<<"$0, $"<<freeRegs[0]<<", end_loop_"<<loop_count<<std::endl;
        dst<<"\t"<<"nop"<<std::endl;

        loop_count++;
        Body->compile(dst, contxt, destReg);
        loop_count--;

        //branch back to top
        dst<<"\t"<<"b"<<"\t"<<"while_loop_"<<loop_count<<"_begin"<<std::endl;
        //end of loop
        dst<<"end_loop_"<<loop_count<<":"<<"\t#End while loop"<<std::endl;
        contxt.set_unused(freeRegs[0]);
    }
};

class DoWhileStatement: public Expression {
private:
    ExpressionPtr Condition;
    NodePtr Body;

public:
    ~DoWhileStatement() {}

    DoWhileStatement (ExpressionPtr _Condition, NodePtr _Body) : Condition(_Condition) , Body(_Body) {}

    virtual void print (std::ostream &dst) const override {
        dst << "do" <<std::endl;
        dst<<"\t";
        Body->print(dst);
        dst<<"while(";
        Condition->print(dst);
        dst <<");"<<std::endl;
        
    }
};

class ForStatement: public Expression {
private:
    NodePtr Init;
    ExpressionPtr Condition;
    NodePtr Increment;
    NodePtr Body;

public:
    ~ForStatement() {}

    ForStatement (NodePtr _Init, ExpressionPtr _Condition, NodePtr _Increment, NodePtr _Body) : Init(_Init), Condition(_Condition) , Increment(_Increment), Body(_Body) {}

    virtual void print (std::ostream &dst) const override {
        dst << "for(";
        Init->print(dst);
        Condition->print(dst);
        dst<<";";
        Increment->print(dst);
        dst<<")"<<std::endl;
        dst<<"\t";
        Body->print(dst);        
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override {
        //use a free register for condition check
        std::vector<int> freeRegs = contxt.FreeTempRegs();
        contxt.set_used(freeRegs[0]);

        dst<<"for_loop_"<<loop_count<<"_begin:"<<"\t#Begin for loop"<<std::endl;

        Init->compile(dst, contxt, freeRegs[0]);
        
        //evalute the condition into the free register
        Condition->compile(dst, contxt, destReg);
        //branch to end if condition evaluates false (0)
        dst<<"\t"<<"beq"<<"\t"<<"$0, $"<<freeRegs[0]<<", end_loop_"<<loop_count<<std::endl;
        dst<<"\t"<<"nop"<<std::endl;

        loop_count++;
        Body->compile(dst, contxt, destReg);
        loop_count--;

        Increment->compile(dst, contxt, destReg);

        //branch back to top
        dst<<"\t"<<"b"<<"\t"<<"for_loop_"<<loop_count<<"_begin"<<std::endl;
        //end of loop
        dst<<"end_loop_"<<loop_count<<":"<<"\t#End while loop"<<std::endl;
        contxt.set_unused(freeRegs[0]);
    }
};


class ScopeBody : public Expression {
private:
    NodePtr Body;
public:
    ~ScopeBody () {}

    ScopeBody (NodePtr _Body) : Body(_Body) {}

    virtual void print(std::ostream &dst) const override {
        dst << "{ " << std::endl;
        // std::cout<<"scope level: "<<scopelevel<<std::endl;
        scopelevel++;
        Body->print(dst);
        scopelevel--;
        for (int i = 0; i < scopelevel; i++) {
            dst << "\t";
        }
        dst << "}";
    }

    virtual void translate(std::ostream &dst) const override {
        dst << std::endl;
        scopelevel++;
        Body->translate(dst);
        scopelevel--;
        for (int i = 0; i < scopelevel; i++) {
            dst << "\t";
        }
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        //dst << std::endl;
        // scopelevel++;
        dst<<"#FUNCTION BODY"<<std::endl;
        Body->compile(dst, contxt, destReg);
        // scopelevel--;
        // for (int i = 0; i < scopelevel; i++) {
        //     dst << "\t";
        // }
    }
};

class NoBraces : public Expression
{
  private:
    NodePtr Body;

  public:
    ~NoBraces() {}

    NoBraces(NodePtr _Body) : Body(_Body) {}

    virtual void print(std::ostream &dst) const override
    {
        // std::cout<<"scope level: "<<scopelevel<<std::endl;
        scopelevel++;
        for (int i = 0; i < scopelevel; i++)  { dst << "\t"; }
        Body->print(dst);
        scopelevel--;
                for (int i = 0; i < scopelevel; i++)  { dst << "\t"; }

    }

    virtual void translate(std::ostream &dst) const override
    {
        dst << std::endl;
        scopelevel++;
        for (int i = 0; i < scopelevel; i++)  { dst << "\t"; }
        Body->translate(dst);
        scopelevel--;
        for (int i = 0; i < scopelevel; i++)  { dst << "\t"; }

    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override {
        Body->compile(dst,contxt,destReg);
    }
};

class ScopeStatements: public AST_node {
private:
    NodePtr Singular_statement;
    NodePtr Rest_of_statements;
public:
    ~ScopeStatements () {}

    ScopeStatements (NodePtr _Singular_statement, NodePtr _Rest_of_statements)
     : Singular_statement(_Singular_statement), Rest_of_statements(_Rest_of_statements) {}

    virtual void print(std::ostream &dst) const override {
        for (int i = 0; i < scopelevel; i++) {
            dst << "\t";
        }
        Singular_statement->print(dst);
        dst<<std::endl;
        if (Rest_of_statements != NULL) {
            Rest_of_statements->print(dst);
        }
    }

    virtual void translate(std::ostream &dst) const override {
        for (int i = 0; i < scopelevel; i++) {
            dst << "\t";
        }
        Singular_statement->translate(dst);
        dst<<std::endl;
        if (Rest_of_statements != NULL) {
            Rest_of_statements->translate(dst);
        }
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        // for (int i = 0; i < scopelevel; i++) {
        //     dst << "\t";
        // }
        Singular_statement->compile(dst, contxt, destReg);
        if (Rest_of_statements != NULL) {
            Rest_of_statements->compile(dst, contxt, destReg);
        }
    }


};

class BracketedExpr : public Expression {
private:
    ExpressionPtr Expression;
public:
    ~BracketedExpr() {}
    BracketedExpr(ExpressionPtr _Expression) : Expression(_Expression) {}

    virtual void print (std::ostream &dst) const override {
        dst<<"(";
        Expression->print(dst);
        dst<<")";
    }

    virtual void translate (std::ostream &dst) const override {
        dst<<"(";
        Expression->translate(dst);
        dst<<")";
    }
};

#endif