#ifndef ast_dec_def_hpp
#define ast_dec_def_hpp

#include <iostream>
#include <vector>
#include <string>
#include "ast.hpp"

extern std::vector<std::string> global_vars;
extern int var_count;
extern int scopelevel;
extern int function_call_num;
extern  std::vector<std::string> function_call_queue;
static std::vector<std::string> function_def_queue;
static int function_def_num;


class Program : public AST_node
{
  private:
    NodePtr ExDec;
    NodePtr Rest_of_program;

public:
    ~Program() {}

    Program (NodePtr _ExDec, NodePtr _Rest_of_program) : ExDec(_ExDec), Rest_of_program(_Rest_of_program) {
        function_call_num = 0;
        
    }

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

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        ExDec->compile(dst, contxt, destReg);
        if (Rest_of_program != NULL) {
            Rest_of_program->compile(dst, contxt, destReg);
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
    : Type(_Type), Identifier(_Identifier), Arguments(_Arguments), Scope(_Scope) {
    }

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

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        function_def_num++;
        function_def_queue.push_back(Identifier);
        if (Identifier == "main") {
            function_call_queue.insert(function_call_queue.begin(),"main");
        }
        //dst<<"#MIPS function:"<<std::endl;
        //creating ABI directives
        dst<<"#----------FUNCTION "<<Identifier<<"----------"<<std::endl;
        dst<<"\t"<<".text"<<std::endl;
        dst<<std::endl;
        dst<<"\t"<<".align"<<"\t"<<"2"<<std::endl;
        dst<<"\t"<<".globl"<<"\t"<<Identifier<<std::endl;
        dst<<std::endl;
        dst<<"\t"<<".ent"<<"\t"<<Identifier<<std::endl;
        dst<<"\t"<<".type"<<"\t"<<Identifier<<", @function"<<std::endl;

        dst<<Identifier<<":"<<std::endl;
        //space allocated in stack
        dst<<"#allocating stack"<<std::endl;
        dst<<"\t"<<"addiu"<<"\t"<<"$sp, $sp,-"<<(var_count*4) +parameter_count+12<<std::endl; //restoring sp
        dst<<"\t"<<"sw"<<"\t"<<"$ra,"<<(var_count*4)+parameter_count+8<<"($sp)"<<std::endl; //store return address at end of stack frame
        dst<<"\t"<<"sw"<<"\t"<<"$fp,"<<(var_count*4)+parameter_count+4<<"($sp)"<<std::endl; //old fp = top of stack address - 4
        dst<<"\t"<<"move"<<"\t"<<"$fp, $sp"<<std::endl;

        if(Arguments != NULL){
            Arguments->compile(dst, contxt, destReg);
        }

        contxt.FreeParamRegs();

        if(Scope != NULL){
            Scope->compile(dst, contxt, destReg);
        }
        else if(Identifier == "main" && Scope == NULL){
            dst<<"\t"<<"move"<<"\t"<<"$2, $0"<<std::endl; //empty main should return zero in $2
        }
        else{
            dst<<"\t"<<"nop"<<"\t"<<std::endl; //if a function is declared as empty
        }
        
        dst<<Identifier<<"_function_end_"<<function_def_num<<":"<<std::endl;
        dst<<"#deallocating stack"<<std::endl;
        dst<<"\t"<<"move"<<"\t"<<"$sp, $fp"<<std::endl; //deallocating stack
        dst<<"\t"<<"lw"<<"\t"<<"$ra,"<<(var_count*4)+parameter_count+8<<"($sp)"<<std::endl;
        dst<<"\t"<<"lw"<<"\t"<<"$fp,"<<(var_count*4)+parameter_count+4<<"($sp)"<<std::endl; //old fp = top of stack address - 4
        dst<<"\t"<<"addiu"<<"\t"<<"$sp, $sp,"<<(var_count*4)+parameter_count+12<<std::endl; //restoring sp
        dst<<"\t"<<"j"<<"\t"<<"$ra"<<std::endl;
        dst<<"\t"<<"nop"<<std::endl;
        dst<<std::endl;

        dst<<"\t"<<".end"<<"\t" <<Identifier<<std::endl;
        function_def_queue.pop_back();

        contxt.FreeParamRegs();
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