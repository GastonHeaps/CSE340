#ifndef __PARSER__H__
#define __PARSER__H__

#include <vector>
#include <string>

#include "inputbuf.h"
#include "lexer.h"

typedef enum {
    C1, C2, C3, C4, C5
} ConstraintType;

typedef enum {
    INT_TYPE = 1, REAL_TYPE, BOOL_TYPE
} Type;

struct sTableEntry
{
    std::string name;
    int lineNo;
    int type;
    int printed;
};

struct sTable
{
    sTableEntry* item;
    sTable* prev;
    sTable* next;
};


class Parser
{
public:

    int parse_program();
    void printList();

private:
    int parse_global_vars();
    int parse_var_decl_list();
    int parse_var_decl();
    int parse_var_list();
    int parse_type_name();
    int parse_body();
    int parse_stmt_list();
    int parse_stmt();
    int parse_assignment_stmt();
    int parse_expression();
    void compare_L(int line_No, int token_Type);
    int parse_unary_operator();
    int parse_binary_operator();
    int parse_primary();
    int parse_if_stmt();
    int parse_while_stmt();
    int parse_switch_stmt();
    int parse_case_list();
    int parse_case();
    void syntax_error();
    void type_mismatch(ConstraintType c);
    void update_Types(int LHS, int RHS);
    Token peek();
    Token expect(TokenType type);
    void addList(std::string n, int lN, int t);
    int Search_List(std::string n);
};

#endif  //__PARSER__H__
