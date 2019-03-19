#ifndef ast_dec_def_hpp
#define ast_dec_def_hpp

#include <iostream>
#include <vector>
#include <string>
#include "ast.hpp"

extern std::vector<std::string> global_vars;
extern int var_count;
extern int scopelevel;

static std::vector<std::string> GlobalNames;
static std::vector<ExpressionPtr> GlobalNameExpr;
static std::vector<std::string> LocalNames;
static std::vector<ExpressionPtr> LocalNameExpr;

class Program : public AST_node
{
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
    ExpressionPtr Arguments;

    FunctionDef (std::string _Type, std::string _Identifier, ExpressionPtr _Arguments) : Type(_Type), Identifier(_Identifier), Arguments(_Arguments) {}
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
    ExpressionPtr Arguments;
    NodePtr Scope;

    FunctionDec(std::string _Type, std::string _Identifier, ExpressionPtr _Arguments, NodePtr _Scope) 
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
        for (unsigned int i=0; i< global_vars.size();i++) {
            dst<<std::endl;
            for (int i = 0; i < scopelevel; i++) {dst << "\t";}
            dst<<"\tglobal "<<global_vars[i];
        }
        for (unsigned int i=0; i< GlobalNames.size();i++) {
            dst<<std::endl;
            for (int i = 0; i < scopelevel; i++) {
                dst << "\t";
            }
            dst << "\tglobal " << GlobalNames[i];
        }
        if (Scope != NULL) {
            Scope->translate(dst);
        }
        else if(Scope == NULL){
            dst<<std::endl;
            dst<<"\t";
            dst<<"pass";
            dst<<std::endl;
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

    virtual void compile (std::ostream &dst, Context &contxt) const override 
    {

        //dst<<"#MIPS function:"<<std::endl;
        //creating ABI directives
        dst<<"\t"<<".text"<<std::endl;
        dst<<std::endl;
        dst<<"\t"<<".align"<<"\t"<<"2"<<std::endl;
        dst<<"\t"<<".globl"<<"\t"<<Identifier<<std::endl;
        dst<<std::endl;
        dst<<"\t"<<".ent"<<"\t"<<Identifier<<std::endl;
        dst<<"\t"<<".type"<<"\t"<<Identifier<<", @function"<<std::endl;

        dst<<Identifier<<":"<<std::endl;
        //space allocated in stack
        dst<<"\t"<<"addiu"<<"\t"<<"$sp, $sp,-"<<(var_count*4)+8<<std::endl; //restoring sp
         dst<<"\t"<<"sw"<<"\t"<<"$31,"<<(var_count*4)+8<<"($sp)"<<std::endl;
        dst<<"\t"<<"sw"<<"\t"<<"$fp,"<<(var_count*4)+4<<"($sp)"<<std::endl; //old fp = top of stack address - 4
        dst<<"\t"<<"move"<<"\t"<<"$fp, $sp"<<std::endl;

        if(Arguments != NULL){
            Arguments->compile(dst, contxt);
        }

        contxt.FreeParamRegs();

        if(Scope != NULL){
            Scope->compile(dst, contxt);
        }
        else if(Identifier == "main" && Scope == NULL){
            dst<<"\t"<<"move"<<"\t"<<"$2, $0"<<std::endl; //empty main should return zero in $2
        }
        else{
            dst<<"\t"<<"nop"<<"\t"<<std::endl; //if a function is declared as empty
        }
        
        dst<<"\t"<<"move"<<"\t"<<"$sp, $fp"<<std::endl; //deallocating stack
        
        dst<<"\t"<<"lw"<<"\t"<<"$fp,"<<(var_count*4)+4<<"($sp)"<<std::endl; //old fp = top of stack address - 4
        dst<<"\t"<<"addiu"<<"\t"<<"$sp, $sp,"<<(var_count*4)+8<<std::endl; //restoring sp
        dst<<"\t"<<"j"<<"\t"<<"$31"<<std::endl;
        dst<<"\t"<<"nop"<<std::endl;
        dst<<std::endl;

        dst<<"\t"<<".end"<<"\t" <<Identifier<<std::endl;

        contxt.FreeParamRegs();
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

    virtual void compile (std::ostream &dst, Context &contxt) const override {
        //only generate assembly if the variable is assigned
        if (Expression != NULL) {
            dst<<"\t"<<".globl"<<"\t"<<Name<<std::endl;
            dst<<"\t"<<".data"<<"\t"<<std::endl;
            dst<<"\t"<<".align"<<"\t"<<"2"<<std::endl;
            //dst<<"\t"<<".type"<<"\t"<<Name<<", @object"<<std::endl;
            //dst<<"\t"<<".size"<<"\t"<<Name<<", 4"<<std::endl;
            
            dst<<Name<<":"<<std::endl;
            //store expression
            dst<<"\t"<<".word"<<"\t"<<std::endl;
            Expression->compile(dst, contxt);
            dst<<std::endl;

            dst<<"\t"<<".text"<<std::endl;
            dst<<"\t"<<".align"<<"\t"<<"2"<<std::endl;
        }

        contxt.NewGlobalVar (Name);
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

class MultipleDecs : public Expression {
private:
  std::string Type;
  ExpressionPtr AdditionalNames;
  bool isGlobal;

protected:
    

public:
    ~MultipleDecs() {}
    MultipleDecs (std::string _Type, std::string _Name, ExpressionPtr _AdditionalNames, bool _isGlobal) 
    : Type(_Type), AdditionalNames(_AdditionalNames), isGlobal(_isGlobal) {
        if (isGlobal)
        GlobalNames.push_back(_Name);
        else
        LocalNames.push_back(_Name);
    }
    MultipleDecs () {} //for inheritance

    virtual void print(std::ostream &dst) const override {
        AdditionalNames->print(dst);
        //after recursive call
        if (isGlobal) {
            for (unsigned int i = 0; i < GlobalNames.size(); i++) {
                dst << Type << " " << GlobalNames[i];
                if (GlobalNameExpr[i] != NULL) {
                    dst << " = ";
                    GlobalNameExpr[i]->print(dst);
                }
                dst << ";" << std::endl;
                for (int i = 0; i < scopelevel; i++) {dst << "\t";}
            }
        }
        else {
            for (unsigned int i = 0; i < LocalNames.size(); i++) {
                dst << Type << " " << LocalNames[i];
                if (LocalNameExpr[i] != NULL) {
                    dst << " = ";
                    LocalNameExpr[i]->print(dst);
                }
                dst << ";" << std::endl;
                for (int i = 0; i < scopelevel; i++) {dst << "\t";}

            }
        }
        
    }

    virtual void translate (std::ostream &dst) const override {
        AdditionalNames->translate(dst);
        //after recursive call
        if (isGlobal) {
            for (unsigned int i=0;i<GlobalNames.size();i++) {
                dst << GlobalNames[i];
                if (GlobalNameExpr[i] != NULL) {
                    dst<<" = ";
                    GlobalNameExpr[i]->translate(dst);
                }
                else {
                    dst<<" =0";
                }
                dst<<std::endl;
                for (int i = 0; i < scopelevel; i++) {dst << "\t";}
            }
        }
        else {
           for (unsigned int i=0;i<LocalNames.size();i++) {
                dst<< LocalNames[i];
                if (LocalNameExpr[i] != NULL)
                {
                    dst<<" = ";
                    LocalNameExpr[i]->translate(dst);
                }
                else {
                    dst<<" =0";
                }
                dst<<std::endl;
                for (int i = 0; i < scopelevel; i++) {dst << "\t";}
            } 
        }
    }
};

class AdditionalDecs: public MultipleDecs {
private:
    std::string CurrentVar;
    ExpressionPtr CurrentExpression;
    ExpressionPtr NextVar;
    bool isGlobal;
public:
    ~AdditionalDecs () {}

    AdditionalDecs(std::string _CurrentVar, ExpressionPtr _CurrentExpression, ExpressionPtr _NextVar, bool _isGlobal) 
    : CurrentVar(_CurrentVar), CurrentExpression(_CurrentExpression), NextVar(_NextVar), isGlobal(_isGlobal) {
        if (isGlobal) {
            GlobalNames.push_back(CurrentVar);
            GlobalNameExpr.push_back(CurrentExpression);
        }
            
        else {
            LocalNames.push_back(CurrentVar);
            LocalNameExpr.push_back(CurrentExpression);
        }

    }

    virtual void print (std::ostream &dst) const override {
        if(NextVar != NULL) {
            NextVar->print(dst);
        }
    }
    
    virtual void translate (std::ostream &dst) const override {
        if (NextVar != NULL) {
            NextVar->translate(dst);
        }
    }
};


class PostIncrement: public AST_node {

private:
std::string variable;

public:
    ~PostIncrement() {}

    PostIncrement (std::string _variable) : variable(_variable) {}

    virtual void print (std::ostream &dst) const override {
        dst<<variable<<"++";
    }

    // virtual void translate (std::ostream &dst) const override {
    //     ExDec->translate(dst);
    //     if (Rest_of_program != NULL) {
    //         Rest_of_program->translate(dst);
    //     }
    // }
};

class PreIncrement: public AST_node {

private:
std::string variable;

public:
    ~PreIncrement() {}

    PreIncrement (std::string _variable) : variable(_variable) {}

    virtual void print (std::ostream &dst) const override {
        dst<<"++"<<variable;
    }

    // virtual void translate (std::ostream &dst) const override {
    //     ExDec->translate(dst);
    //     if (Rest_of_program != NULL) {
    //         Rest_of_program->translate(dst);
    //     }
    // }
};

class PostDecrement: public AST_node {

private:
std::string variable;

public:
    ~PostDecrement() {}

    PostDecrement (std::string _variable) : variable(_variable) {}

    virtual void print (std::ostream &dst) const override {
        dst<<variable<<"--";
    }

    // virtual void translate (std::ostream &dst) const override {
    //     ExDec->translate(dst);
    //     if (Rest_of_program != NULL) {
    //         Rest_of_program->translate(dst);
    //     }
    // }
};

class PreDecrement: public AST_node {

private:
std::string variable;

public:
    ~PreDecrement() {}

    PreDecrement (std::string _variable) : variable(_variable) {}

    virtual void print (std::ostream &dst) const override {
        dst<<"--"<<variable;
    }

    // virtual void translate (std::ostream &dst) const override {
    //     ExDec->translate(dst);
    //     if (Rest_of_program != NULL) {
    //         Rest_of_program->translate(dst);
    //     }
    // }
};

#endif