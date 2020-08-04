//Gaston Heaps
//CSE 340

#include <iostream>
#include <fstream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include "stdlib.h"
#include "inputbuf.h"
#include "parser.h"
#include <stack>



using namespace std;

string reserved[] = { "END_OF_FILE",
	"INT", "REAL", "BOOL", "TRUE", "FALSE",
	"IF", "WHILE", "SWITCH", "CASE", "NOT",
	"PLUS", "MINUS", "MULT", "DIV",	"GREATER", 
	"LESS", "GTEQ", "LTEQ", "NOTEQUAL",	"LPAREN", 
	"RPAREN", "NUM", "REALNUM", "PUBLIC", "PRIVATE", 
	"EQUAL", "COLON", "COMMA", "SEMICOLON", "LBRACE", 
	"RBRACE", "ID", "ERROR"
};

#define KEYWORDS_COUNT 11
string keyword[] = { 
	"int", "real", "bool", "true", "false",
	"if", "while", "switch", "case", "public", 
	"private"
};

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

bool LexicalAnalyzer::SkipComment() {

	char c,c2;
	bool comment_encountered = false;
	input.GetChar(c);

	while (c == '/') {
		input.GetChar(c);
		if (c2 != '/') {
			input.UngetChar(c2);
			comment_encountered = false;
			tmp.token_type = ERROR;
			return comment_encountered;
		}
		else {
			while (c != '\n' && !input.EndOfInput()) {
				comment_encountered = true;
				input.GetChar(c);
				line_no += (c == '\n');
			}
			SkipSpace();
			input.GetChar(c);
		}
	}

	return comment_encountered;
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

Token LexicalAnalyzer::ScanNumber()
{
	char c;
	bool afterDot = false;

	//REALNUM
	tmp.lexeme = "";
	input.GetChar(c);
	tmp.lexeme += c;
	if (c == '0') {
		input.GetChar(c);
		//Period found after 0
		if (!input.EndOfInput() && c == '.') {
			tmp.lexeme += '.';
			input.GetChar(c);
			//Scan numbers after 0
			while (!input.EndOfInput() && c == '0') {
				tmp.lexeme += c;
				input.GetChar(c);
			}
			if (!input.EndOfInput() && c >= '1' && c <= '9') {
				while (!input.EndOfInput() && isdigit(c)) {
					tmp.lexeme += c;
					input.GetChar(c);
				}

			}
			if (!input.EndOfInput()) {
				input.UngetChar(c);
			}
			tmp.token_type = REALNUM;
			tmp.line_no = line_no;
			return tmp;
		}
		else {
			if (!input.EndOfInput()) {
				input.UngetChar(c);
			}
		}
	}
	else if (c >= '1' && c <= '9') {
		input.GetChar(c);
		while (!input.EndOfInput() && isdigit(c)) {
			tmp.lexeme += c;
			input.GetChar(c);
		}
		if (!input.EndOfInput()) {
			input.UngetChar(c);
		}
		input.GetChar(c);
		//Period found after number
		if (!input.EndOfInput() && c == '.') {
			tmp.lexeme += '.';
			afterDot = false;
			input.GetChar(c);
			while (!input.EndOfInput() && isdigit(c)) {
				afterDot = true;
				tmp.lexeme += c;
				input.GetChar(c);
			}
			if (!input.EndOfInput()) {
				input.UngetChar(c);
			}
			if (afterDot) {
				tmp.token_type = REALNUM;
				tmp.line_no = line_no;
				return tmp;
			}
		}
		else {
			if (!input.EndOfInput()) {
				input.UngetChar(c);
			}
		}
	}

	//NUM
	input.UngetString(tmp.lexeme);
	tmp.lexeme = "";
	input.GetChar(c);
	tmp.lexeme += c;
	if (isdigit(c)) {
		if (c == '0') {
			tmp.token_type = NUM;
			tmp.line_no = line_no;
			return tmp;
		}
		else {
			input.GetChar(c);
			while (!input.EndOfInput() && isdigit(c)) {
				tmp.lexeme += c;
				input.GetChar(c);
			}
			if (!input.EndOfInput()) {
				input.UngetChar(c);
			}
			tmp.token_type = NUM;
			tmp.line_no = line_no;
			return tmp;
		}
	}

	//ERROR
	input.UngetString(tmp.lexeme);
	tmp.lexeme = "";
	tmp.token_type = ERROR;
	tmp.line_no = line_no;
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

	//Removes comments
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
		if (c == '=') {
			tmp.token_type = GTEQ;
		}
		else {
			if (!input.EndOfInput()) {
				input.UngetChar(c);
			}
			tmp.token_type = GREATER;
		}
		return tmp;
	case '<':
		input.GetChar(c);
		if (c == '=') {
			tmp.token_type = LTEQ;
		}
		else if (c == '>') {
			tmp.token_type = NOTEQUAL;
		}
		else {
			if (!input.EndOfInput()) {
				input.UngetChar(c);
			}
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
		if (isdigit(c)) {
			input.UngetChar(c);
			return ScanNumber();
		}
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

string constraint[] = {"C1", "C2", "C3", "C4", "C5"};

SymbolTable::SymbolTable()
{
	head = NULL;
}
void SymbolTable::addItem(std::string name, std::string type)
{
	//Create new node for symbol table and create a new symbol table item
	symbolTable* symTab = new symbolTable();
	symbolTableItem* newItem = new symbolTableItem();

	newItem->name = name;
	newItem->type = type;

	if (head == NULL) //list is empty
	{
		head = symTab;
		symTab->item = newItem;
		symTab->next = NULL;
		symTab->previous = NULL;
	}
	else //add to end
	{
		symbolTable* traverser = head;

		while (traverser->next != NULL)
		{
			traverser = traverser->next;
		}

		traverser->next = symTab;
		symTab->item = newItem;
		symTab->next = NULL;
		symTab->previous = traverser;
	}
}

void SymbolTable::editType(std::string type)
{
	symbolTable* traverser = head;

	while (traverser != NULL)
	{
		if (traverser->item->type == "?")
		{
			traverser->item->type = type;
		}

		traverser = traverser->next;
	}
}

std::string SymbolTable::searchItem(std::string name)
{
	cerr << "SymbolTable.h -- INSIDE SEARCHITEM\n" << endl;

	symbolTable* traverser = head;

	string tmpString;

	if (head == NULL) //the list is empty
	{
		return "?";
	}

	while (traverser->next != NULL)
	{
		traverser = traverser->next;
	}

	//search, starting with the most recent node added
	while (traverser != NULL)
	{
		if (traverser->item->name == name)
		{
			cerr << "found " << traverser->item->name << ", " << traverser->item->type << endl << endl;
			return traverser->item->type;
		}
		traverser = traverser->previous;
	} 

	return "?";
}

Token Parser::peek()
{
	token = lexer.GetToken();
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

//Below methods parse program based on expected symbols
void Parser::parse_program()
{
	parse_global_vars();
	parse_body();
}

//Print variable assignments
void Parser::print() {

}



void Parser::parse_global_vars()
{
	//global_vars --> epsilon
	//global_vars --> var_decl_list

	token = peek();

	if (token.token_type == LBRACE) //global_vars --> epsilon
	{
		lexer.UngetToken(token);
	}
	else if (token.token_type == ID) //global_vars --> var_decl_list
	{
		parse_var_decl_list();
	}
	else
	{
		syntax_error();
	}
}
void Parser::parse_var_decl_list()
{
	//var_decl_list --> var_decl
	//var_decl_list --> var_decl var_decl_list
	parse_var_decl();

	token = peek();
	if (token.token_type == LBRACE) //followed by parse_body
	{
		//done
	}
	else if (token.token_type == ID)
	{
		parse_var_decl_list();
	}
	else
	{
		syntax_error();
	}
}
void Parser::parse_var_decl()
{
	//var_decl --> var_list COLON type_name SEMICOLON
	parse_var_list();
	expect(COLON);
	string type = parse_type_name();

	table.editType(type);

	expect(SEMICOLON);
}
void Parser::parse_var_list()
{
	cerr << "parser.cc -- INSIDE PARSE_VAR_LIST\n" << endl;

	token = lexer.GetToken();

	if (token.token_type != ID)
	{
		syntax_error();
	}
	else
	{
		Token t2 = peek();

		if (t2.token_type == COMMA) //ID COMMA var_list
		{
			table.addItem(token.lexeme, "?");

			cerr << "parse_var_list just added " << token.lexeme << "to the symbol table, adding another variable\n" << endl;
			expect(COMMA);
			parse_var_list();
		}
		else if (t2.token_type == COLON) //ID
		{
			//done
			table.addItem(token.lexeme, "?");

			cerr << "parse_var_list just added " << token.lexeme << "to the symbol table, LEAVING PARSE_VAR_LIST\n" << endl;

		}
		else
		{
			syntax_error();
		}
	}
}
string Parser::parse_type_name()
{
	token = lexer.GetToken();

	if (token.token_type == INT || token.token_type == REAL || token.token_type == BOOL)
	{
		return token.lexeme;
	}
	else
	{
		syntax_error();
	}
}
void Parser::parse_body()
{
	expect(LBRACE);
	parse_stmt_list();
	expect(RBRACE);
}

void Parser::parse_stmt_list()
{
	//stmt
	//stmt stmt_list

	parse_stmt();

	token = peek();

	if (token.token_type == RBRACE)
	{
		//stmt
	}
	else if (token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH)
	{
		parse_stmt_list();
	}
	else
	{
		syntax_error();
	}
}

void Parser::parse_stmt()
{
	token = peek();

	if (token.token_type == ID) {
		parse_assignment_stmt();
	}
	else if (token.token_type == IF) {
		parse_if_stmt();
	}
	else if (token.token_type == WHILE) {
		parse_while_stmt();
	}
	else if (token.token_type == SWITCH) {
		parse_switch_stmt();
	}
	else {
		syntax_error();
	}
}

void Parser::parse_assignment_stmt()
{
	token = expect(ID);

	cerr << "parse_var_list just added " << token.lexeme << "to the symbol table, LEAVING PARSE_VAR_LIST\n" << endl;

	expect(EQUAL);
	parse_expression();
	expect(SEMICOLON);

	int operatorCount = 0;
	while (!expression.empty()) {
		operatorCount++;
		symbolTableItem* newItem = new symbolTableItem();
		Token token = expression.back();
		if (token.token_type == ID ) {
			newItem->name = token.lexeme;
			newItem->type = table.searchItem(token.lexeme);

			result.push(newItem);
		}
		else if (token.token_type == NUM) {
			newItem->name = "T" + operatorCount;
			newItem->type = "int";

			result.push(newItem);
		}
		else if (token.token_type == REALNUM) {
			newItem->name = "T" + operatorCount;
			newItem->type = "real";

			result.push(newItem);
		}
		else if (token.token_type == TRUE || token.token_type == FALSE) {
			newItem->name = "T" + operatorCount;
			newItem->type = "bool";

			result.push(newItem);
		}
		else if (token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV) {
			string type1 = result.top()->type;
			result.pop();

			string type2 = result.top()->type;
			result.pop();

			if (type1 == type2) {
				symbolTableItem* newItem = new symbolTableItem();

				newItem->name = "T" + operatorCount;
				newItem->type = type1;
				result.push(newItem);
			}
			else {
				type_mismatch(C2);
			}
		}
		else if (token.token_type == GREATER || token.token_type == LESS || token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL) {
			string type1 = result.top()->type;
			result.pop();

			string type2 = result.top()->type;
			result.pop();

			if (type1 == type2) {
				symbolTableItem* newItem = new symbolTableItem();

				newItem->name = "T" + operatorCount;
				newItem->type = "bool";
				result.push(newItem);
			}
			else {
				type_mismatch(C2);
			}
		}
		else if (token.token_type == NOT) {
			string type1 = result.top()->type;
			result.pop();

			if (type1 == "bool") {
				symbolTableItem* newItem = new symbolTableItem();

				newItem->name = "T" + operatorCount;
				newItem->type = "bool";
				result.push(newItem);
			}
			else {
				type_mismatch(C3);
			}
		}
		expression.pop_back();
	}
	string leftType = table.searchItem(token.lexeme);
	string rightType = result.top()->type;
	if (leftType == "?") {
		table.addItem(token.lexeme, rightType);
	}
	else if (leftType == rightType) {

	}
	else {
		type_mismatch(C1);
	}

}

void Parser::parse_expression()
{
	token = peek();

	if (token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TRUE || token.token_type == FALSE)
	{
		parse_primary();
	}
	else if (token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV)
	{
		parse_binary_operator();
		parse_expression();
		parse_expression();
	}
	else if (token.token_type == GREATER || token.token_type == LESS || token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL)
	{
		parse_binary_operator();
		parse_expression();
		parse_expression();
	}
	else if (token.token_type == NOT)
	{
		parse_unary_operator();
		parse_expression();
	}
	else
	{
		syntax_error();
	}
}

void Parser::parse_unary_operator()
{
	token = lexer.GetToken();
	if (token.token_type == NOT) {
		expression.push_back(token);
	}
}

void Parser::parse_binary_operator()
{
	token = lexer.GetToken();

	if (token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV)
	{
		expression.push_back(token);
	}
	else if (token.token_type == GREATER || token.token_type == LESS || token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL)
	{
		expression.push_back(token);
	}
	else
	{
		syntax_error();
	}

}

void Parser::parse_primary()
{
	token = lexer.GetToken();
	if (token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TRUE || token.token_type == FALSE) {
		expression.push_back(token);
	}
}

void Parser::parse_if_stmt()
{
}

void Parser::parse_while_stmt()
{
}

void Parser::parse_switch_stmt()
{
}

void Parser::parse_case_list()
{
}

void Parser::parse_case()
{
}

//Unexpected token found
void Parser::syntax_error()
{
	cout << "Syntax Error\n";
	exit(EXIT_FAILURE);
}

void Parser::type_mismatch(Constraint c)
{
	cout << "TYPE MISMATCH " << token.line_no << " " << constraint[c] << endl;
	exit(1);
}


int main()
{
	ifstream in("test.txt");
	streambuf* cinbuf = cin.rdbuf();
	std::cin.rdbuf(in.rdbuf());
	Parser parser;
	parser.parse_program();
	//parser.print();
	return 0;
}