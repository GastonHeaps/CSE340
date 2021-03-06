#ifndef __PARSER__H__
#define __PARSER__H__

#include <string>
#include <vector>

typedef enum {
    GLOBAL_ACCESS, PUBLIC_ACCESS, PRIVATE_ACCESS
} AccessSpecifier;

struct symbolTable {
    std::string variable;
    AccessSpecifier access;
    std::string declaration;
};

struct scopeTable {
    std::string scope;
    scopeTable* previous;
    std::vector<symbolTable> symbols;
    scopeTable()
    {
        previous = NULL;
    }
};

class SymbolTable
{
public:

    SymbolTable();
    std::vector<std::pair<symbolTable, symbolTable>> assignments;
    void addScope(std::string);
    void exitScope();
    void addVariable(std::string, AccessSpecifier);
    void addAssignment(std::string, std::string);
    symbolTable findVariable(std::string);
private:
    scopeTable* current;
    scopeTable* pointer;

};



class Parser
{
public:
    Parser();
    void parse_program();
    void print();

private:
    void parse_global_vars();
    void parse_var_list(AccessSpecifier access);
    void parse_scope();
    void parse_public_vars();
    void parse_private_vars();
    void parse_stmt_list();
    void parse_stmt();
    void syntax_error();
};

#endif  //__PARSER__H__
