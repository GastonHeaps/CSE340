#ifndef __PARSER__H__
#define __PARSER__H__

#include <string>
#include <vector>

#include "inputbuf.h"

typedef enum {
    END_OF_FILE = 0,
    PUBLIC, PRIVATE,
    EQUAL, COLON, COMMA, SEMICOLON,
    LBRACE, RBRACE, ID, ERROR
} TokenType;

class Token {
public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

class LexicalAnalyzer {
public:
    Token GetToken();
    TokenType UngetToken(Token);
    LexicalAnalyzer();

private:
    std::vector<Token> tokens;
    int line_no;
    Token tmp;
    InputBuffer input;

    bool SkipSpace();
    bool SkipComment();
    bool IsKeyword(std::string);
    TokenType FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();
};


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
    std::vector<std::pair<symbolTable, symbolTable> > assignments;
    SymbolTable();
    void addScope(std::string);
    void exitScope();
    void addVariable(std::string, AccessSpecifier);
    symbolTable findVariable(std::string);
    void addAssignment(std::string, std::string);

private:
    scopeTable* current;
    scopeTable* pointer;

};



class Parser
{
public:


    void parse_program();
    void print();

private:
    void parse_global_vars();
    void parse_var_list(AccessSpecifier);
    void parse_scope();
    void parse_public_vars();
    void parse_private_vars();
    void parse_stmt_list();
    void parse_stmt();
    void syntax_error();

    Token token;
    LexicalAnalyzer lexer;
    SymbolTable table;
};

#endif  //__PARSER__H__
