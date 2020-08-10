#include <iostream>
#include <fstream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "inputbuf.h"
#include "lexer.h"
#include "parser.h"



using namespace std;

string reserved[] = { "END_OF_FILE", "INT", "REAL", "BOOL", "TR", "FA", "IF", "WHILE", "SWITCH", "CASE", "PUBLIC", "PRIVATE", "NUM", "REALNUM", "NOT", "PLUS", "MINUS", "MULT", "DIV", "GTEQ", "GREATER", "LTEQ", "NOTEQUAL", "LESS", "LPAREN", "RPAREN", "EQUAL", "COLON", "COMMA", "SEMICOLON", "LBRACE", "RBRACE", "ID", "ERROR" // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 11
string keyword[] = { "int", "real", "bool", "true", "false", "if", "while", "switch", "case", "public", "private" };


int line = 0;

string constraint[] = { "C1", "C2", "C3", "C4", "C5" };

LexicalAnalyzer lexer;
Token token;
TokenType tempTokenType;
int enumType;
int enumCount = 4;


sTable* symbolTable;

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
        << reserved[(int)this->token_type] << " , "
        << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    line = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');
    line = line_no;

    while (!input.EndOfInput() && isspace(c))
    {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
        line = line_no;
    }

    if (!input.EndOfInput())
    {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::SkipComments()
{
    bool comments = false;
    char c;
    if (input.EndOfInput())
    {
        input.UngetChar(c);
        return comments;
    }

    input.GetChar(c);

    if (c == '/')
    {
        input.GetChar(c);
        if (c == '/')
        {
            comments = true;
            while (c != '\n')
            {
                comments = true;
                input.GetChar(c);
            }
            line_no++;
            line = line_no;
            SkipComments();
        }
        else
        {
            comments = false;
            cout << "Syntax Error\n";
            exit(0);
        }
    }
    else
    {
        input.UngetChar(c);
        return comments;
    }
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
        {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
        {
            return (TokenType)(i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c;
    bool realNUM = false;
    input.GetChar(c);
    if (isdigit(c))
    {
        if (c == '0')
        {
            tmp.lexeme = "0";
            input.GetChar(c);
            if (c == '.')
            {
                input.GetChar(c);

                if (!isdigit(c))
                {
                    input.UngetChar(c);
                }
                else
                {
                    while (!input.EndOfInput() && isdigit(c))
                    {
                        tmp.lexeme += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                    input.UngetChar(c);
                }
            }
            else
            {
                input.UngetChar(c);
            }
        }
        else
        {
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(c))
            {
                tmp.lexeme += c;
                input.GetChar(c);
            }
            if (c == '.')
            {
                input.GetChar(c);

                if (!isdigit(c))
                {
                    input.UngetChar(c);
                }
                else
                {
                    while (!input.EndOfInput() && isdigit(c))
                    {
                        tmp.lexeme += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                }
            }
            if (!input.EndOfInput())
            {
                input.UngetChar(c);
            }
        }

        if (realNUM)
        {
            tmp.token_type = REALNUM;
        }
        else
        {
            tmp.token_type = NUM;
        }
        tmp.line_no = line_no;
        return tmp;
    }
    else
    {
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c))
    {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c))
        {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;

        if (IsKeyword(tmp.lexeme))
        {
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        }
        else
        {
            tmp.token_type = ID;
        }
    }
    else
    {
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//

TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty())
    {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    SkipComments();
    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c)
    {
    case '!':
        tmp.token_type = NOT;
        return tmp;
    case '+':
        tmp.token_type = PLUS;
        return tmp;
    case '-':
        tmp.token_type = MINUS;
        return tmp;
    case '*':
        tmp.token_type = MULT;
        return tmp;
    case '/':
        tmp.token_type = DIV;
        return tmp;
    case '>':
        input.GetChar(c);
        if (c == '=')
        {
            tmp.token_type = GTEQ;
        }
        else
        {
            input.UngetChar(c);
            tmp.token_type = GREATER;
        }
        return tmp;
    case '<':
        input.GetChar(c);
        if (c == '=')
        {
            tmp.token_type = LTEQ;
        }
        else if (c == '>')
        {
            tmp.token_type = NOTEQUAL;
        }
        else
        {
            input.UngetChar(c);
            tmp.token_type = LESS;
        }
        return tmp;
    case '(':
        tmp.token_type = LPAREN;
        return tmp;
    case ')':
        tmp.token_type = RPAREN;
        return tmp;
    case '=':
        tmp.token_type = EQUAL;
        return tmp;
    case ':':
        tmp.token_type = COLON;
        return tmp;
    case ',':
        tmp.token_type = COMMA;
        return tmp;
    case ';':
        tmp.token_type = SEMICOLON;
        return tmp;
    case '{':
        tmp.token_type = LBRACE;
        return tmp;
    case '}':
        tmp.token_type = RBRACE;
        return tmp;
    default:
        if (isdigit(c))
        {
            input.UngetChar(c);
            return ScanNumber();
        }
        else if (isalpha(c))
        {
            input.UngetChar(c);
            return ScanIdOrKeyword();
        }
        else if (input.EndOfInput())
        {
            tmp.token_type = END_OF_FILE;
        }
        else
        {
            tmp.token_type = ERROR;
        }
        return tmp;
    }
}


Token Parser::peek()
{
    Token token = lexer.GetToken();
    lexer.UngetToken(token);
    return token;
}

Token Parser::expect(TokenType type)
{
    token = lexer.GetToken();
    if (token.token_type != type)
    {
        syntax_error();
    }
    return token;
}

void Parser::syntax_error()
{
    cout << "\n Syntax Error \n";
    exit(EXIT_FAILURE);
}

void Parser::type_mismatch(ConstraintType c)
{
    cout << "TYPE MISMATCH " << token.line_no << " " << constraint[c] << endl;
    exit(1);
}

void Parser::addList(std::string name, int line, int type)
{
    if (symbolTable == NULL)
    {
        sTable* newEntry = new sTable();
        sTableEntry* newItem = new sTableEntry();

        newItem->name = name;
        newItem->lineNo = token.line_no;
        newItem->type = type;
        newItem->printed = 0;

        newEntry->item = newItem;
        newEntry->next = NULL;
        newEntry->prev = NULL;

        symbolTable = newEntry;
    }
    else
    {
        sTable* temp = symbolTable;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }

        sTable* newEntry = new sTable();
        sTableEntry* newItem = new sTableEntry();

        newItem->name = name;
        newItem->lineNo = token.line_no;
        newItem->type = type;
        newItem->printed = 0;

        newEntry->item = newItem;
        newEntry->next = NULL;
        newEntry->prev = temp;
        temp->next = newEntry;
    }
}

int Parser::Search_List(std::string n)
{
    sTable* temp = symbolTable;
    bool found = false;
    if (symbolTable == NULL)
    {
        addList(n, token.line_no, enumCount);
        enumCount++;
        return (4);
    }
    else
    {
        while (temp->next != NULL)
        {
            if (strcmp(temp->item->name.c_str(), n.c_str()) == 0)
            {
                found = true;
                return(temp->item->type);
            }
            else
            {
                temp = temp->next;
            }
        }
        if (strcmp(temp->item->name.c_str(), n.c_str()) == 0)
        {
            found = true;
            return(temp->item->type);
        }
        if (!found)
        {
            addList(n, token.line_no, enumCount);
            enumCount++;
            int t = enumCount - 1;
            return(t);
        }
    }
}


int Parser::parse_var_list()
{
    expect(ID);
    int tempI;
    char* lexeme = (char*)malloc(sizeof(token.lexeme) + 1);
    memcpy(lexeme, (token.lexeme).c_str(), (token.lexeme).size() + 1);
    addList(lexeme, token.line_no, 0);

    Token t2 = peek();
    if (t2.token_type == COMMA)
    {
        expect(COMMA);
        cerr << "\n Rule Parsed: var_list -> ID COMMA var_list \n";
        tempI = parse_var_list();
    }
    else if (t2.token_type == COLON)
    {
        cerr << "\n Rule Parsed: var_list -> ID \n";
    }
    else
    {
        syntax_error();
    }

    return(0);
}

// return 0 for error and return 1 for NOT
int Parser::parse_unary_operator()
{
    expect(NOT);
    cerr << "\n Rule parsed: unary_operator -> NOT";
    return(1);
}

// this returns 0 if lType cant be bool, 1 if lType can be anything, -1 if type error
int Parser::parse_binary_operator()
{
    token = lexer.GetToken();
    switch (token.token_type) {
    case PLUS:
        cerr << "\n Rule parsed: binary_operator -> PLUS\n";
        return(PLUS);
    case MINUS:
        cerr << "\n Rule parsed: binary_operator -> MINUS\n";
        return(MINUS);
    case MULT:
        cerr << "\n Rule parsed: binary_operator -> MULT\n";
        return(MULT);
    case DIV:
        cerr << "\n Rule parsed: binary_operator -> DIV\n";
        return(DIV);
    case GREATER:
        cerr << "\n Rule parsed: binary_operator -> GREATER\n";
        return(GREATER);
    case LESS:
        cerr << "\n Rule parsed: binary_operator -> LESS\n";
        return(LESS);
    case GTEQ:
        cerr << "\n Rule parsed: binary_operator -> GTEQ\n";
        return(GTEQ);
    case LTEQ:
        cerr << "\n Rule parsed: binary_operator -> LTEQ\n";
        return(LTEQ);
    case EQUAL:
        cerr << "\n Rule parsed: binary_operator -> EQUAL\n";
        return(EQUAL);
    case NOTEQUAL:
        cerr << "\n Rule parsed: binary_operator -> NOTEQUAL\n";
        return(NOTEQUAL);
    default:
        syntax_error();
        return(-1);
    }
}

int Parser::parse_primary()
{
    token = lexer.GetToken();
    switch (token.token_type) {
    case ID:
        cerr << "\n Rule parsed: primary -> ID\n";
        return(Search_List(token.lexeme));
    case NUM:
        cerr << "\n Rule parsed: primary -> NUM\n";
        return(INT_TYPE);
    case REALNUM:
        cerr << "\n Rule parsed: primary -> REALNUM\n";
        return(REAL_TYPE);
    case TR:
        cerr << "\n Rule parsed: primary -> TRUE\n";
        return(BOOL_TYPE);
    case FA:
        cerr << "\n Rule parsed: primary -> FALSE\n";
        return(BOOL_TYPE);
    default:
        syntax_error();
        return(0);
    }
}

int Parser::parse_expression()
{
    int tempI;
    token = peek();
    if (token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TR || token.token_type == FA)
    {
        cerr << "\n Rule parsed: expression -> primary \n";
        tempI = parse_primary();
    }
    else if (token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || 
        token.token_type == GREATER || token.token_type == LESS || token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL)
    {
        cerr << "\n Rule parsed: expression -> binary_operator expression expression \n";
        int leftExp;
        int rightExp;
        tempI = parse_binary_operator(); 
        leftExp = parse_expression();
        rightExp = parse_expression();
        if ((leftExp != rightExp) || (tempI != PLUS && tempI != MINUS && tempI != MULT && tempI != DIV && tempI != GTEQ && tempI != GREATER && tempI != LTEQ && tempI != NOTEQUAL && tempI != LESS && tempI != EQUAL))
        {

            if (tempI == PLUS || tempI == MINUS || tempI == MULT || tempI == DIV)
            {
                if (leftExp <= 2 && rightExp > 3)
                {
                    update_Types(rightExp, leftExp);
                    rightExp = leftExp;
                }
                else if (leftExp > 3 && rightExp <= 2)
                {
                    update_Types(rightExp, leftExp);
                    leftExp = rightExp;
                }
                else if (leftExp > 3 && rightExp > 3)
                {
                    update_Types(rightExp, leftExp);
                    rightExp = leftExp;
                }
                else
                {
                    type_mismatch(C2);
                }
            }
            else if (tempI == GREATER || tempI == LESS || tempI == GTEQ || tempI == LTEQ || tempI == EQUAL || tempI == NOTEQUAL)
            {
                if (leftExp <= 2 && rightExp > 3)
                {
                    update_Types(rightExp, leftExp);
                    rightExp = leftExp;
                    return(3);
                }
                else if (leftExp > 3 && rightExp <= 2)
                {
                    update_Types(rightExp, leftExp);
                    leftExp = rightExp;
                    return(3);
                }
                else if (rightExp > 3 && leftExp > 3)
                {
                    update_Types(rightExp, leftExp);
                    rightExp = leftExp;
                    return(3);
                }
                else
                {
                    type_mismatch(C2);
                }
            }
            else
            {
                type_mismatch(C2);
            }
        }
        if (tempI == GTEQ || tempI == GREATER || tempI == LTEQ || tempI == LESS || tempI == EQUAL || tempI == NOTEQUAL)
        {
            tempI = 3;
        }
        else
        {
            tempI = rightExp;
        }
    }
    else if (token.token_type == NOT)
    {
        cerr << "\n Rule parsed: expression -> unary_operator expression \n";
        tempI = parse_unary_operator(); 
        tempI = parse_expression();
        if (tempI != 3)
        {
            type_mismatch(C3);
        }
    }
    else
    {
        syntax_error();
    }
    return tempI;
}

void Parser::compare_L(int line_No, int token_Type)
{
    sTable* temp = symbolTable;
    while (temp->next != NULL)
    {
        if (temp->item->lineNo == line_No)
        {
            temp->item->type = token_Type;
        }
        temp = temp->next;
    }
    if (temp->item->lineNo == line_No)
    {
        temp->item->type = token_Type;
    }
}

void Parser::update_Types(int currentType, int newType)
{
    sTable* temp = symbolTable;

    while (temp->next != NULL)
    {
        if (temp->item->type == currentType)
        {
            temp->item->type = newType;
        }
        temp = temp->next;
    }
    if (temp->item->type == currentType)
    {
        temp->item->type = newType;
    }
}

int Parser::parse_assignment_stmt()
{
    int tempI;
    string name;
    int LHS;
    int RHS;
    expect(ID);

    LHS = Search_List(token.lexeme);
    expect(EQUAL);


    token = peek();
    if (token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TR || token.token_type == FA ||
        token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV ||
        token.token_type == LESS || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL ||
        token.token_type == NOT)
    {
        RHS = parse_expression();
        if (LHS == INT_TYPE || LHS == REAL_TYPE || LHS == BOOL_TYPE)
        {
            if (LHS != RHS)
            {
                if (LHS < 3)
                {
                    type_mismatch(C1);
                }
                else
                {
                    update_Types(RHS, LHS);
                    RHS = LHS;
                }
            }
        }
        else
        {
            update_Types(LHS, RHS);
            LHS = RHS;
        }
        expect(SEMICOLON);
        cerr << "\n Rule parsed: assignment_stmt -> ID EQUAL expression SEMICOLON"<<endl;
    }
    else
    {
        syntax_error();
    }
    return(0);
}

int Parser::parse_case()
{
    int tempI;
    expect(CASE);
    expect(NUM);
    expect(COLON);
    cerr << "\n Rule parsed: case -> CASE NUM COLON body";
    tempI = parse_body();
}

int Parser::parse_case_list()
{
    int tempI;
    token = peek();
    if (token.token_type == CASE)
    {
        tempI = parse_case();
        token = peek();
        if (token.token_type == CASE)
        {
            cerr << "\n Rule parsed: case_list -> case case_list \n";
            tempI = parse_case_list();
        }
        else if (token.token_type == RBRACE)
        {
            cerr << "\n Rule parsed: case_list -> case  \n";
        }
    }
    return(0);
}

int Parser::parse_switch_stmt()
{
    int tempI;
    expect(SWITCH);
    expect(LPAREN);

    tempI = parse_expression();
    if (tempI <= 3 && tempI != 1)
    {
        type_mismatch(C5);
    }
    expect(RPAREN);
    expect(LBRACE);
    tempI = parse_case_list();
    expect(RBRACE);
    cerr << "\n Rule parsed: switch_stmt -> SWITCH LPAREN expression RPAREN LBRACE case_list RBRACE \n";
    return(0);
}

int Parser::parse_while_stmt()
{
    int tempI;
    expect(WHILE);
    expect(LPAREN);
    tempI = parse_expression();
    if (tempI != BOOL_TYPE)
    {
        type_mismatch(C4);
    }
    expect(RPAREN);
    cerr << "\n Rule parsed: whilestmt -> WHILE LPAREN expression RPAREN body \n";
    tempI = parse_body();
    return(0);
}

int Parser::parse_if_stmt()
{
    int tempI;
    expect(IF);
    expect(LPAREN);
    tempI = parse_expression();
    if (tempI != BOOL_TYPE)
    {
        type_mismatch(C4);
    }
    expect(RPAREN);
    cerr << "\n Rule parsed: ifstmt -> IF LPAREN expression RPAREN body \n";
    tempI = parse_body();
    return(0);
}

int Parser::parse_stmt()
{
    int tempI;
    token = peek();
    switch (token.token_type) {
    case ID:
        cerr << "\n Rule parsed: stmt -> assignment_stmt \n";
        tempI = parse_assignment_stmt();
        break;
    case IF:
        cerr << "\n Rule parsed: stmt -> if_stmt";
        tempI = parse_if_stmt();
        break;
    case WHILE:
        cerr << "\n Rule parsed: stmt -> while_stmt";
        tempI = parse_while_stmt();
        break;
    case SWITCH:
        cerr << "\n Rule parsed: stmt -> switch_stmt";
        tempI = parse_switch_stmt();
        break;
    default:
        syntax_error();
    }
    return(0);
}

int Parser::parse_stmt_list()
{
    token = peek();
    int tempI;
    if (token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH)
    {
        tempI = parse_stmt();
        token = peek();
        if (token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH)
        {
            cerr << "\n Rule Parsed: stmt_list -> stmt stmt_list \n";
            tempI = parse_stmt_list();

        }
        else if (token.token_type == RBRACE)
        {
            cerr << "\n Rule parsed: stmt_list -> stmt \n";
        }
    }
    else
    {
        syntax_error();
    }
    return(0);
}

int Parser::parse_body()
{
    token = lexer.GetToken();
    int tempI;
    if (token.token_type == LBRACE)
    {
        cerr << "\n Rule Parsed: scope -> ID LBRACE public_vars private_vars stmt_list RBRACE \n";
        tempI = parse_stmt_list();
        expect(RBRACE);
        cerr << "\n Rule parsed: body -> LBRACE stmt_list RBRACE \n";
    }
    else if (token.token_type == END_OF_FILE)
    {
        tempTokenType = lexer.UngetToken(token);
    }
    else
    {
        syntax_error();
    }
    return(0);
}

int Parser::parse_type_name()
{
    token = lexer.GetToken();
    if (token.token_type == INT || token.token_type == REAL || token.token_type == BOOL)
    {
        compare_L(token.line_no, token.token_type);
    }
    else
    {
        syntax_error();
    }
    return(0);
}

int Parser::parse_var_decl()
{
    int tempI;
    token = peek();
    if (token.token_type == ID)
    {
        tempI = parse_var_list();
        expect(COLON);
        tempI = parse_type_name();
        expect(SEMICOLON);
        cerr << "\n Rule parsed: var_decl -> var_list COLON type_name SEMICOLON" << endl;
    }
    return(0);
}

int Parser::parse_var_decl_list()
{
    int tempI;
    token = peek();
    while (token.token_type == ID)
    {
        tempI = parse_var_decl();
        token = peek();
        if (token.token_type != ID)
        {
            cerr << "\n Rule Parsed: var_decl_list -> var_decl \n";
        }
        else
        {
            cerr << "\n Rule Parsed: var_decl_list -> var_decl var_decl_list \n";
        }
    }
    return(0);
}

int Parser::parse_global_vars()
{
    token = peek();
    int tempI;
    if (token.token_type == ID)
    {
        cerr << "\n Rule parsed: global_vars -> var_decl_list \n";
        tempI = parse_var_decl_list();
    }
    else
    {
        syntax_error();
    }
    return(0);
}


int Parser::parse_program()
{
    token = peek();
    int tempI;
    while (token.token_type != END_OF_FILE)
    {
        if (token.token_type == ID)
        {
            cerr << "\n Rule parsed: program -> global_vars scope \n";
            tempI = parse_global_vars();
            tempI = parse_body();
        }
        else if (token.token_type == LBRACE)
        {
            cerr << "\n Rule parsed: global_vars -> epsilon \n";
            tempI = parse_body();
        }
        else if (token.token_type == END_OF_FILE)
        {
            return(0);
        }
        else
        {
            syntax_error();
            return(0);
        }
        token = peek();
    }
}

string output = "";

void Parser::printList()
{

    sTable* temp = symbolTable;
    int temp1;

    while (temp->next != NULL)
    {
        if (temp->item->type > 3 && temp->item->printed == 0)
        {
            temp1 = temp->item->type;
            output += temp->item->name;
            temp->item->printed = 1;
            while (temp->next != NULL)
            {
                temp = temp->next;
                if (temp->item->type == temp1)
                {
                    output += ", " + temp->item->name;
                    temp->item->printed = 1;
                }
            }
            output += ": ? #";
            cout << output << endl;
            temp->item->printed = 1;
            output = "";
            temp = symbolTable;
        }
        else if (temp->item->type < 4 && temp->item->printed == 0)
        {
            string lCase = keyword[(temp->item->type) - 1];
            int temp1 = temp->item->type;
            output = temp->item->name + ": " + lCase + " #";
            cout << output << endl;
            output = "";
            temp->item->printed = 1;

            while (temp->next != NULL && temp->next->item->type == temp1)
            {
                temp = temp->next;
                string lCase2 = keyword[(temp->item->type) - 1];
                output = temp->item->name + ": " + lCase2 + " #";
                cout << output << endl;
                temp->item->printed = 1;
                output = "";
            }
        }
        else
        {
            temp = temp->next;
        }
    }
    if (temp->item->type <= 3 && temp->item->printed == 0)
    {
        string lCase3 = keyword[(temp->item->type) - 1];
        output += temp->item->name + ": " + lCase3 + " #";
        cout << output << endl;
        output = "";
    }
    else if (temp->item->type > 3 && temp->item->printed == 0)
    {
        output += temp->item->name + ":" + " ? " + "#";
        cout << output << endl;
        output = "";
    }
}
