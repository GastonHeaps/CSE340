#ifndef __PARSER__H__
#define __PARSER__H__

#include <string>
#include "lexer.h"


typedef enum {
    GLOBAL_ACCESS, PUBLIC_ACCESS, PRIVATE_ACCESS
} AccessSpecifier;

struct symbolTableItem {
    std::string name;
    std::string scope;
    AccessSpecifier access;
    symbolTableItem()
    {
    }
    symbolTableItem(std::string n, std::string s, AccessSpecifier a) : name(n), scope(s), access(a)
    {
    }
};

struct symbolTable {
    symbolTableItem* item;
    symbolTable* next;
    symbolTable* previous;
};

struct printOutput {
    std::string line;
    printOutput* next;
};

class SymbolTable {
    private:
        symbolTable* head;
        symbolTable* temp;
        symbolTable* n;
        symbolTable* pointer;
    public:
        std::string currentScope;
        AccessSpecifier currentAccess;

        SymbolTable();
        void addItem(std::string name, std::string scope, AccessSpecifier access);
        std::string searchList(std::string);
};


class Parser
{
public:
    Parser();
    void parse_program();
    void print();

    

private:
    void parse_global_vars();
    void parse_var_list();
    void parse_scope();
    void parse_public_vars();
    void parse_private_vars();
    void parse_stmt_list();
    void parse_stmt();
    void syntax_error();
    void addString(std::string);
    //void searchList(std::string);

    LexicalAnalyzer lexer; 
    Token token;
    SymbolTable table;
    printOutput* stringHead;
    printOutput* stringTemp;
    printOutput* stringNew;

};

#endif  //__PARSER__H__
