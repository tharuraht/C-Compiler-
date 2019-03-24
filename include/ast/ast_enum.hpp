#ifndef ast_enum_hpp
#define ast_enum_hpp

#include "ast.hpp"

class EnumDeclaration : public Expression {
    private:
    std::string listname;
    ExpressionPtr varlist;

    public:
    ~EnumDeclaration () {}
    EnumDeclaration(std::string _listname, ExpressionPtr _varlist) : listname(_listname), varlist(_varlist) {}

    virtual void print (std::ostream &dst) const override {
        dst<<"enum "<<listname<<"{";
        varlist->print(dst);
        dst<<"};";
    }
};

class EnumElement: public Expression {
    private:
    std::string current_element;
    int el_value;
    ExpressionPtr next_el;
    bool value_assigned;

    public:
    ~EnumElement() {}
    EnumElement(std::string _current_element, int _el_value, ExpressionPtr _next_el, bool _value_assigned)
     : current_element(_current_element), el_value(_el_value), next_el(_next_el), value_assigned(_value_assigned) {}

    virtual void print (std::ostream &dst) const override {
        dst<<current_element;
        if (value_assigned) {
            dst<<" = "<<el_value;
            
        }
        if (next_el != NULL) {
            dst<<", ";
            next_el->print(dst);
        }
    }
};

#endif