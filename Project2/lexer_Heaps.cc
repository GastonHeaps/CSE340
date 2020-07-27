#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>


#include "lexer_Heaps.h"
#include "inputbuf.h"

using namespace std;



string reserved[] = { "END_OF_FILE",
    "PUBLIC", "PRIVATE",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRACE", "RBRACE", "ID", "ERROR"
};

#define KEYWORDS_COUNT 2
string keyword[] = { "public", "private" };

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
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType)(i + 1);
        }
    }
    return ERROR;
}


Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    }
    else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c, c2;

    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;


	input.GetChar(c);

	while (c == '/') {
		input.GetChar(c2);
		if (c2 != '/') {
			input.UngetChar(c2);
			tmp.token_type = ERROR;
			return tmp;
		}
		else {
			while (c != '\n' && !input.EndOfInput()) {
				input.GetChar(c);
			}
			SkipSpace();
			input.GetChar(c);
		}
	}
	

    switch (c) {
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
        if (isalpha(c)) {
            input.UngetChar(c);
            return ScanIdOrKeyword();
        }
        else if (input.EndOfInput())
            tmp.token_type = END_OF_FILE;
        else
            tmp.token_type = ERROR;

        return tmp;
    }
}
