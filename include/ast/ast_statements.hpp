#ifndef ast_statements_hpp
#define ast_statements_hpp

#include "ast.hpp"

extern int scopelevel;

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
};

class ReturnStatement : public Expression {
public:
    ExpressionPtr AdditionalExpressions;

    ~ReturnStatement() {}

    ReturnStatement (ExpressionPtr _AdditionalExpressions): AdditionalExpressions(_AdditionalExpressions) {}

    virtual void print(std::ostream &dst) const override {
        for (int i = 0; i < scopelevel; i++) {
            dst << "\t";
        }
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
        for (int i = 0; i < scopelevel; i++) {
            dst << "\t";
        }
        dst << "return";
        if (AdditionalExpressions != NULL) {
            dst << " ";
            AdditionalExpressions->print(dst);
            
        }
    }
};

class IfStatement : public Expression {
private:
    ExpressionPtr Condition;
    NodePtr Body;
public:
    ~IfStatement () {}

    IfStatement (ExpressionPtr _Condition, NodePtr _Body) : Condition(_Condition), Body(_Body) {}

    virtual void print (std::ostream &dst) const override {
        for (int i = 0; i < scopelevel; i++) {
            dst << "\t";
        }
        dst << "if (";
        Condition->print(dst);
        dst<<") ";
        Body->print(dst);

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
};

#endif