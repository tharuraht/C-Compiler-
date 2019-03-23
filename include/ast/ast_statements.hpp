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
        
        int stack_end = (var_count*4) +parameter_count+12+50;
        
        //find a free register
        std::vector<int> freeRegs = contxt.FindFreeTempRegs();
        contxt.set_used(freeRegs[0]);
        Expression->compile(dst, contxt, freeRegs[0]);
        //store result into variable
        if (contxt.LookupVariable(VarName, scopelevel) != 0) 
            dst<<"\t"<<"sw"<<"\t"<<"$"<<freeRegs[0]<<", "<<contxt.LookupVariable(VarName, scopelevel)<<"($fp)"<<"\t#Assign variable "<<VarName<<std::endl;

        else if (contxt.find_parameter(VarName) > -1) {
            dst<<"\t"<<"sw"<<"\t"<<"$"<<freeRegs[0]<<", "<<(contxt.find_parameter(VarName))*4+stack_end<<"($fp)"<<"\t#Assign passed variable "<<VarName<<std::endl;
        }
        // if(varGlobal == false){
        //     dst<<"\t"<<"sw"<<"\t"<<"$"<<freeRegs[0]<<", "<<stack_end<<"($fp)"<<"\t#Assign function result"<<VarName<<std::endl;
        // }
        // else{
        //     dst<<"\t"<<"sw"<<"\t"<<"$"<<freeRegs[0]<<", "<<contxt.LookupVariable(VarName, scopelevel)<<"($fp)"<<"\t#Assign variable "<<VarName<<std::endl;
        // }
        contxt.set_unused(freeRegs[0]);
    }
};

class ArrayAssignment : public Expression {
private:
    std::string Identifier;
    // int index_no;
    ExpressionPtr Expression;
    ExpressionPtr IndexExpr;
public:
    ~ArrayAssignment() {}
    // ArrayAssignment(std::string _Identifier, int _index_no, ExpressionPtr _Expression)
    //  : Identifier(_Identifier), index_no(_index_no), Expression(_Expression), IndexExpr(NULL) {}
    ArrayAssignment(std::string _Identifier, ExpressionPtr _IndexExpr, ExpressionPtr _Expression)
     : Identifier(_Identifier), Expression(_Expression), IndexExpr(_IndexExpr) {}


    virtual void print (std::ostream &dst) const override {
        dst<<Identifier<<"[";
        IndexExpr->print(dst);
        dst<<"] = ";
        Expression->print(dst);
        dst<<";";
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {

        std::vector<int> freeRegs = contxt.FindFreeTempRegs();
        contxt.set_used(freeRegs[0]);
        int index_no = IndexExpr->evaluate();
        Expression->compile(dst, contxt, freeRegs[0]);

        int var_offset = contxt.LookupVariable(Identifier + std::to_string(index_no), scopelevel);
        if (var_offset !=0) {
            dst<<"\t"<<"sw"<<"\t"<<"$"<<freeRegs[0]<<", "<<var_offset<<"($fp)"<<"\t#Assign element "<<index_no<<" of array "<<Identifier<<std::endl;
        }
        else {
            //global array
            dst<<"\t"<<"lui"<<"\t"<<"$"<<destReg<<", "<<"%hi("<<Identifier<<")"<<"\t#Loading in array: "<<Identifier<<std::endl;
            dst<<"\t"<<"addiu"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", %lo("<<Identifier<<")"<<std::endl;
            dst<<"\t"<<"nop"<<std::endl;
            dst<<"\t"<<"addiu"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", "<<index_no*4<<"\t#Add offset for element "<<index_no<<std::endl;
            dst<<"\t"<<"sw"<<"\t"<<"$"<<freeRegs[0]<<", ($"<<destReg<<")"<<"\t#Store into global array "<<Identifier<<std::endl;
        }
        dst<<"\t"<<"nop"<<std::endl;
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
        dst<<"\t"<<"b "<<function_def_queue.back()<<"_function_end_"<<function_def_num<<"\t#Return statement"<<std::endl;
        if (function_def_queue.back()=="main") {
            main_returned = true;
        }
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
        dst<<"\t"<<"nop"<<std::endl;

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
        std::vector<int> freeRegs = contxt.FindFreeConstantRegs();
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
        dst<<"\t"<<"nop"<<std::endl;
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
        std::vector<int> freeRegs = contxt.FindFreeConstantRegs();
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
        dst<<"\t"<<"nop"<<std::endl;
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

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        Expression->compile(dst, contxt, destReg);
    }
};

class BreakStatement : public Expression {
private:
    
public:
    ~BreakStatement() {}
    BreakStatement() {}

    virtual void print (std::ostream &dst) const override {
        dst<<"break;"<<std::endl;
    }

    virtual void translate (std::ostream &dst) const override {
        dst<<"("<<"break"<<")"<<std::endl;
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        dst<<"\t"<<"b"<<"\t"<<"end_break_"<<loop_count<<std::endl;
    }
};


#endif