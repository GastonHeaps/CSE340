#ifndef __PARSER__H__
#define __PARSER__H__

#include <string>
#include <vector>
#include <stack>

#include "inputbuf.h"

typedef enum {
    END_OF_FILE = 0,
    INT, REAL, BOOL, TRUE, FALSE,
    IF, WHILE, SWITCH, CASE, NOT,
    PLUS, MINUS, MULT, DIV, GREATER, 
    LESS, GTEQ, LTEQ, NOTEQUAL, LPAREN, 
    RPAREN, NUM, REALNUM, PUBLIC, PRIVATE, 
    EQUAL, COLON, COMMA, SEMICOLON, LBRACE, 
    RBRACE, ID, ERROR
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
    Token ScanNumber();
};

struct symbolTableItem {
    std::string name;
    std::string type;
};

struct symbolTable {
    symbolTableItem* item;
    symbolTable* previous;
    symbolTable* next;
};

class SymbolTable
{
public:

    SymbolTable();
    void addItem(std::string name, std::string type);
    void editType(std::string type);
    std::string searchItem(std::string name);


private:
    symbolTable* head;
};

typedef enum {
    C1, C2, C3, C4, C5
} Constraint;

class Parser
{
public:

    Token peek();
    Token expect(TokenType);
    void parse_program();
    void print();

private:
    void parse_global_vars();
    void parse_var_decl_list();
    void parse_var_decl();
    void parse_var_list();
    std::string parse_type_name();
    void parse_body();
    void parse_stmt_list();
    void parse_stmt();
    void parse_assignment_stmt();
    void parse_expression();
    void parse_unary_operator();
    void parse_binary_operator();
    void parse_primary();
    void parse_if_stmt();
    void parse_while_stmt();
    void parse_switch_stmt();
    void parse_case_list();
    void parse_case();
    void syntax_error();
    void type_mismatch(Constraint constraint);

    Token token;
    LexicalAnalyzer lexer;
    SymbolTable table;
    std::vector<Token> expression;
    std::stack<symbolTableItem*> result;
};

#endif  //__PARSER__H__
