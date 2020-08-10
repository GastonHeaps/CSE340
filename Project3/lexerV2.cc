#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE", "INT", "REAL", "BOOL", "TR", "FA", "IF", "WHILE", "SWITCH", "CASE", "PUBLIC", "PRIVATE", "NUM", "REALNUM", "NOT", "PLUS", "MINUS", "MULT", "DIV", "GTEQ", "GREATER", "LTEQ", "NOTEQUAL", "LESS", "LPAREN", "RPAREN", "EQUAL", "COLON", "COMMA", "SEMICOLON", "LBRACE", "RBRACE", "ID", "ERROR" // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 11
string keyword[] = { "int", "real", "bool", "true", "false", "if", "while", "switch", "case", "public", "private" };

LexicalAnalyzer lexer;
Token token;
TokenType tempTokenType;
int enumType;
int enumCount = 4;

struct sTableEntry
{
    string name;
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

sTable* symbolTable;
char* lResolve;
char* rResolve;
int line = 0;

void addList(std::string name, int line, int type)
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
        //memcpy(newItem->scope, currentScope.c_str(), currentScope.size()+1);
        newItem->type = type;
        newItem->printed = 0;

        newEntry->item = newItem;
        newEntry->next = NULL;
        newEntry->prev = temp;
        temp->next = newEntry;
    }
}

int Search_List(std::string n)
{
    sTable* temp = symbolTable;
    bool found = false;
    if (symbolTable == NULL)
    {
        //cout<<"duplicate check"<<endl;
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
        else
        {

        }
    }
}


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
        // TODO: You can check for REALNUM, BASE08NUM and BASE16NUM here!
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
    //cout << "\n Char obtained " << c << "\n";
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
        //cout << "\n I am here" << c << " \n";
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
            //cout << "\n ID scan " << c << " \n";
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

int parse_varlist(void)
{
    token = lexer.GetToken();
    int tempI;
    char* lexeme = (char*)malloc(sizeof(token.lexeme) + 1);
    memcpy(lexeme, (token.lexeme).c_str(), (token.lexeme).size() + 1);
    addList(lexeme, token.line_no, 0);

    if (token.token_type == ID)
    {
        token = lexer.GetToken();
        if (token.token_type == COMMA)
        {
            //cout << "\n Rule Parsed: var_list -> ID COMMA var_list \n";
            tempI = parse_varlist();
        }
        else if (token.token_type == COLON)
        {
            tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule Parsed: var_list -> ID \n";
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_body(void);
Token token1;

int parse_unaryOperator(void)
{
    token = lexer.GetToken();

    if (token.token_type == NOT)
    {
        //cout << "\n Rule parsed: unary_operator -> NOT";
        return(1);
    }
    else
    {
        cout << "\n Syntax Error\n";
        return(0);
    }
}

int parse_binaryOperator(void)
{
    token = lexer.GetToken();
    if (token.token_type == PLUS)
    {
        //check if lType is not bool
        //cout << "\n Rule parsed: binary_operator -> PLUS\n";
        return(15);
    }
    else if (token.token_type == MINUS)
    {
        //check if lType is not bool
        //cout << "\n Rule parsed: binary_operator -> MINUS \n";
        return(16);
    }
    else if (token.token_type == MULT)
    {
        //check if lType is not bool
        //cout << "\n Rule parsed: binary_operator -> MULT\n";
        return(17);
    }
    else if (token.token_type == DIV)
    {
        //check if lType is not bool
        //cout << "\n Rule parsed: binary_operator -> DIV \n";
        return(18);
    }
    else if (token.token_type == GREATER)
    {
        //check if lType is not bool
        //cout << "\n Rule parsed: binary_operator -> GREATER \n";
        return(20);
    }
    else if (token.token_type == LESS)
    {
        //check if lType is not bool
        //cout << "\n Rule parsed: binary_operator -> LESS\n";
        return(23);
    }
    else if (token.token_type == GTEQ)
    {
        //check if lType is not bool
        //cout << "\n Rule parsed: binary_operator -> GTEQ \n";
        return(19);
    }
    else if (token.token_type == LTEQ)
    {
        //check if lType is not bool
        //cout << "\n Rule parsed: binary_operator -> LTEQ\n";
        return(21);
    }
    else if (token.token_type == EQUAL)
    {
        //this can be any type
        //cout << "\n Rule parsed: binary_operator -> EQUAL \n";
        return(26);
    }
    else if (token.token_type == NOTEQUAL)
    {
        //This can be any type
        //cout << "\n Rule parsed: binary_operator -> NOTEQUAL \n";
        return(22);
    }
    else
    {
        cout << "\n Syntax Error \n";
        return(-1);
    }
}

int parse_primary(void)
{
    token = lexer.GetToken();
    if (token.token_type == ID)
    {
        return(Search_List(token.lexeme));
        //cout << "\n Rule parsed: primary -> ID\n";
    }
    else if (token.token_type == NUM)
    {
        //enumType = 1;
        //cout << "\n Rule parsed: primary -> NUM \n";
        return(1);
    }
    else if (token.token_type == REALNUM)
    {
        //enumType = 2;
        //cout << "\n Rule parsed: primary -> REALNUM\n";
        return(2);
    }
    else if (token.token_type == TR)
    {
        //enumType = 3;
        //cout << "\n Rule parsed: primary -> TRUE \n";
        return(3);
    }
    else if (token.token_type == FA)
    {
        //enumType = 3;
        //cout << "\n Rule parsed: primary -> FALSE \n";
        return(3);
    }
    else
    {
        cout << "\n Syntax Error \n";
        return(0);
    }
}

bool isEpr(int i) {
    if (i == PLUS || i == MINUS || i == MULT || i == DIV || i == GREATER || i == LESS || i == GTEQ || i == LTEQ || i == EQUAL || i == NOTEQUAL) {
        return true;
    }
    else {
        return false;
    }
}

bool isExpress(int c) {
    if (c != 15 && c != 16 && c != 17 && c != 18 && c != 19 && c != 20 && c != 21 && c != 22 && c != 23 && c != 26) {
        return true;
    }
    else {
        return false;
    }
}

int parse_expression(void)
{
    //check for C2 error here
    int tempI;
    token = lexer.GetToken();
    if (token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TR || token.token_type == FA)
    {
        //cout << "\n Rule parsed: expression -> primary \n";
        lexer.UngetToken(token);
        tempI = parse_primary();
    }
    else if (isEpr(token.token_type))
    {
        //cout << "\n Rule parsed: expression -> binary_operator expression expression \n";
        int leftExp;
        int rightExp;
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_binaryOperator(); // this returns 0 if lType cant be bool, 1 if lType can be anything, -1 if type error
        leftExp = parse_expression();
        rightExp = parse_expression();
        if ((leftExp != rightExp) || isExpress(tempI))
        {

            if (tempI == 15 || tempI == 16 || tempI == 17 || tempI == 18)
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
                    cout << "TYPE MISMATCH " << token.line_no << " C2" << endl;
                    exit(1);
                }
            }
            else if (tempI == 19 || tempI == 20 || tempI == 21 || tempI == 22 || tempI == 23 || tempI == 26)
            {
                if (rightExp > 3 && leftExp > 3)
                {
                    update_Types(rightExp, leftExp);
                    rightExp = leftExp;
                    return(3);
                }
                else
                {
                    cout << "TYPE MISMATCH " << token.line_no << " C2" << endl;
                    exit(1);
                }
            }
            else
            {
                cout << "TYPE MISMATCH " << token.line_no << " C2" << endl;
                exit(1);
            }
        }
        if (tempI == 19 || tempI == 20 || tempI == 21 || tempI == 23 || tempI == 26)
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
        //cout << "\n Rule parsed: expression -> unary_operator expression \n";
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_unaryOperator(); // return 0 for error and return 1 for NOT
        tempI = parse_expression();
        if (tempI != 3)
        {
            cout << "TYPE MISMATCH " << token.line_no << " C3" << endl;
            exit(1);
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
        return(0);
    }
    return tempI;
}

void compare_L(int line_No, int token_Type)
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

void update_Types(int currentType, int newType)
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

bool isExp(int n) {

    if (n == ID || n == NUM || n == REALNUM || n == TR || n == FA || n == PLUS || n == MINUS || n == MULT || n == DIV || n == LESS || n == GREATER || n == GTEQ || n == LTEQ || n == EQUAL || n == NOTEQUAL || n == NOT)
    {
        return true;
    }
    else {
        return false;
    }
}

int parse_assstmt(void)
{
    int tempI;
    string name;
    int LHS;
    int RHS;
    token = lexer.GetToken();
    if (token.token_type == ID)
    {
        LHS = Search_List(token.lexeme);
        token = lexer.GetToken();
        if (token.token_type == EQUAL)
        {
            token = lexer.GetToken();
            if (isExp(token.token_type))
            {
                lexer.UngetToken(token);
                RHS = parse_expression();
                if (LHS == 1 || LHS == 2 || LHS == 3)
                {
                    if (LHS == RHS)
                    {

                    }
                    else
                    {
                        if (LHS < 3)
                        {
                            cout << "TYPE MISMATCH " << token.line_no << " C1" << endl;
                            exit(1);
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
                token = lexer.GetToken();
                if (token.token_type == SEMICOLON)
                {
                    //cout << "\n Rule parsed: assignment_stmt -> ID EQUAL expression SEMICOLON"<<endl;

                }
                else
                {
                    cout << "\n HI Syntax Error " << token.token_type << " \n";
                }
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_case(void)
{

    int tempI;
    token = lexer.GetToken();
    if (token.token_type == CASE)
    {
        token = lexer.GetToken();
        if (token.token_type == NUM)
        {
            token = lexer.GetToken();
            if (token.token_type == COLON)
            {
                //cout << "\n Rule parsed: case -> CASE NUM COLON body";
                tempI = parse_body();
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
}

int parse_caselist(void)
{
    int tempI;
    token = lexer.GetToken();
    if (token.token_type == CASE)
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_case();
        token = lexer.GetToken();
        if (token.token_type == CASE)
        {
            tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule parsed: case_list -> case case_list \n";
            tempI = parse_caselist();
        }
        else if (token.token_type == RBRACE)
        {
            tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule parsed: case_list -> case  \n";
        }
    }
    return(0);
}

int parse_switchstmt(void)
{
    int tempI;
    token = lexer.GetToken();
    if (token.token_type == SWITCH)
    {
        token = lexer.GetToken();
        if (token.token_type == LPAREN)
        {
            tempI = parse_expression();
            if (tempI <= 3 && tempI != 1)
            {
                cout << "TYPE MISMATCH " << token.line_no << " C5" << endl;
                exit(1);
            }
            token = lexer.GetToken();
            if (token.token_type == RPAREN)
            {
                token = lexer.GetToken();
                if (token.token_type == LBRACE)
                {
                    tempI = parse_caselist();
                    token = lexer.GetToken();
                    if (token.token_type == RBRACE)
                    {
                        //cout << "\n Rule parsed: switch_stmt -> SWITCH LPAREN expression RPAREN LBRACE case_list RBRACE \n";
                    }
                    else
                    {
                        cout << "\n Syntax Error \n";
                    }
                }
                else
                {
                    cout << "\n Syntax Error \n";
                }
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_whilestmt(void)
{
    int tempI;

    token = lexer.GetToken();
    if (token.token_type == WHILE)
    {
        token = lexer.GetToken();
        if (token.token_type == LPAREN)
        {
            tempI = parse_expression();
            if (tempI != 3)
            {
                cout << "TYPE MISMATCH " << token.line_no << " C4" << endl;
                exit(1);
            }
            token = lexer.GetToken();
            if (token.token_type == RPAREN)
            {
                //cout << "\n Rule parsed: whilestmt -> WHILE LPAREN expression RPAREN body \n";
                tempI = parse_body();
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_ifstmt(void)
{
    int tempI;
    token = lexer.GetToken();
    if (token.token_type == IF)
    {
        token = lexer.GetToken();
        if (token.token_type == LPAREN)
        {
            tempI = parse_expression();
            if (tempI != 3)
            {
                cout << "TYPE MISMATCH " << token.line_no << " C4" << endl;
                exit(1);
            }
            token = lexer.GetToken();
            if (token.token_type == RPAREN)
            {
                //cout << "\n Rule parsed: ifstmt -> IF LPAREN expression RPAREN body \n";
                tempI = parse_body();

            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_stmt(void)
{
    int tempI;
    token = lexer.GetToken();
    if (token.token_type == ID)
    {
        tempTokenType = lexer.UngetToken(token);
        //cout << "\n Rule parsed: stmt -> assignment_stmt \n";
        tempI = parse_assstmt();

    }
    else if (token.token_type == IF)
    {
        tempTokenType = lexer.UngetToken(token);
        //cout << "\n Rule parsed: stmt -> if_stmt";
        tempI = parse_ifstmt();
    }
    else if (token.token_type == WHILE)
    {
        tempTokenType = lexer.UngetToken(token);
        //cout << "\n Rule parsed: stmt -> while_stmt";
        tempI = parse_whilestmt();
    }
    else if (token.token_type == SWITCH)
    {
        tempTokenType = lexer.UngetToken(token);
        //cout << "\n Rule parsed: stmt -> switch_stmt";
        tempI = parse_switchstmt();
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_stmtlist(void)
{
    token = lexer.GetToken();
    //token.Print();
    int tempI;
    if (token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH)
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_stmt();
        token = lexer.GetToken();
        //token.Print();
        if (token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH)
        {
            tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule Parsed: stmt_list -> stmt stmt_list \n";
            tempI = parse_stmtlist();

        }
        else if (token.token_type == RBRACE)
        {
            tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule parsed: stmt_list -> stmt \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_body(void)
{
    token = lexer.GetToken();
    //token.Print();
    int tempI;
    if (token.token_type == LBRACE)
    {
        //cout << "\n Rule Parsed: scope -> ID LBRACE public_vars private_vars stmt_list RBRACE \n";
        tempI = parse_stmtlist();
        token = lexer.GetToken();
        //token.Print();
        if (token.token_type == RBRACE)
        {
            //cout << "\n Rule parsed: body -> LBRACE stmt_list RBRACE \n";
            return(0);
        }
        else
        {
            cout << "\n Syntax Error\n ";
            return(0);
        }
    }
    else if (token.token_type == END_OF_FILE)
    {
        tempTokenType = lexer.UngetToken(token);
        return(0);
    }
    else
    {
        cout << "\n Syntax Error \n ";
        return(0);
    }
}

int parse_typename(void)
{
    token = lexer.GetToken();
    if (token.token_type == INT || token.token_type == REAL || token.token_type == BOOL)
    {
        compare_L(token.line_no, token.token_type);
    }
    else
    {
        cout << "\n Syntax Error in parse_typename \n";
    }
    return(0);
}

int parse_vardecl(void)
{
    int tempI;
    token = lexer.GetToken();
    if (token.token_type == ID)
    {
        //
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_varlist();
        token = lexer.GetToken();
        if (token.token_type == COLON)
        {
            tempI = parse_typename();

            //addList(lexeme, token.line_no,tempI);
            token = lexer.GetToken();
            if (token.token_type == SEMICOLON)
            {
                //cout << "\n Rule parsed: var_decl -> var_list COLON type_name SEMICOLON"<<endl;
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_vardecllist(void)
{
    int tempI;
    token = lexer.GetToken();
    while (token.token_type == ID)
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_vardecl();
        token = lexer.GetToken();
        if (token.token_type != ID)
        {
            //tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule Parsed: var_decl_list -> var_decl \n";

        }
        else
        {
            //tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule Parsed: var_decl_list -> var_decl var_decl_list \n";
        }
    }
    tempTokenType = lexer.UngetToken(token);
    return(0);
}

string global = "::";
int parse_globalVars(void)
{
    token = lexer.GetToken();
    int tempI;
    if (token.token_type == ID)
    {
        tempTokenType = lexer.UngetToken(token);
        //cout << "\n Rule parsed: globalVars -> var_decl_list \n";
        tempI = parse_vardecllist();
    }
    else
    {
        cout << "Syntax Error";
    }
    return(0);
}

int parse_program(void)
{
    token = lexer.GetToken();
    int tempI;
    while (token.token_type != END_OF_FILE)
    {
        if (token.token_type == ID)
        {
            tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule parsed: program -> global_vars scope \n";
            tempI = parse_globalVars();
            tempI = parse_body();
        }
        else if (token.token_type == LBRACE)
        {
            tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule parsed: global_vars -> epsilon \n";
            tempI = parse_body();
        }
        else if (token.token_type == END_OF_FILE)
        {
            return(0);
        }
        else
        {
            cout << "\n Syntax Error\n";
            return(0);
        }
        token = lexer.GetToken();
    }
}

string output = "";


void printList(void)
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
                else
                {

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
    else
    {

    }
}




char null[] = "NULL";
int main()
{
    int i;
    i = parse_program();
    printList();
    //cout << "\n End of Program \n";

}