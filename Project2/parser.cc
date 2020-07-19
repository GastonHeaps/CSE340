#include <iostream>
#include <fstream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include "lexer.h"
#include "parser.h"

using namespace std;

SymbolTable::SymbolTable()
{
	head = new symbolTable;
	head->next = NULL;
	head->previous = NULL;
	head->item = new symbolTableItem;
	temp = head;
	currentScope = "::";
	currentAccess = GLOBAL_ACCESS;
}

void SymbolTable::addItem(string name, string scope, AccessSpecifier access)
{
	//Create new node for symbol n and create a new symbol n tmpItem
	symbolTable* n = new symbolTable();
	n->item = new symbolTableItem(name, scope, access);

	if (head == NULL) //list is empty
	{
		head = n;
		temp = n;
		n->next = NULL;
		n->previous = NULL;
	}
	else //add to end
	{
		temp->next = n;
		n->previous = temp;
		n->next = NULL;
		temp = temp->next;
	}
}


string SymbolTable::searchList(string variable) {
	bool found = false;
	string name = "";
	pointer = temp;
	if (pointer->previous == NULL) {
		if (pointer->item->name == variable) {
			if (pointer->item->scope != currentScope && pointer->item->scope != "::") name += "?.";
			else {
				name += pointer->item->scope;
				if (pointer->item->scope != "::") name += ".";
			}
			name += variable;
		}
		else {
			name += "?.";
			name += variable;
		}
	}
	else {
		while (pointer != NULL && !found) {
			if (pointer->item->name == variable) {
				if ((pointer->item->scope != currentScope && (pointer->item->access == GLOBAL_ACCESS || pointer->item->access == PUBLIC_ACCESS))
					|| pointer->item->scope == currentScope || pointer->item->scope == "::") {
					name += pointer->item->scope;
					if (pointer->item->scope != "::") name += ".";
					found = true;
				}
			}
			pointer = pointer->previous;
		}
		if (!found) name += "?.";
		name += variable;
	}
	return name;
}

Parser::Parser()
{
	assignments = "";
}

void Parser::parse_program()
//program -> global_vars scope
{
	bool scopeParsed = false;
	token = lexer.GetToken();
	if (token.token_type == ID) {
		Token token2 = lexer.GetToken();
		if (token2.token_type == COMMA || token2.token_type == SEMICOLON) {
			lexer.UngetToken(token2);
			lexer.UngetToken(token);
			parse_global_vars();
			parse_scope();
			scopeParsed = true;
		}
		else if (token2.token_type == LBRACE) {
			lexer.UngetToken(token2);
			lexer.UngetToken(token);
			parse_scope();
			scopeParsed = true;
		}
		else {
			syntax_error();
		}
	}

	token = lexer.GetToken();
	if (token.token_type == LBRACE || scopeParsed) {
		//End found
	}
	else {
		syntax_error();
	}
}

void Parser::parse_global_vars()
//global_vars -> epsilon
//global_vars -> var_list SEMICOLON
{
	token = lexer.GetToken();
	if (token.token_type == ID) {
		lexer.UngetToken(token);
		parse_var_list();
	}
	else {
		syntax_error();
	}

	token = lexer.GetToken();
	if (token.token_type == SEMICOLON) {
		//End found
	}
	else {
		syntax_error();
	}
}

void Parser::parse_var_list()
//var_list -> ID
//var_list -> ID COMMA var_list
{
	token = lexer.GetToken();
	if (token.token_type == ID) {
		table.addItem(token.lexeme, table.currentScope, table.currentAccess);
		token = lexer.GetToken();
		if (token.token_type == COMMA) {
			parse_var_list();
		}
		else if (token.token_type == SEMICOLON) {
			lexer.UngetToken(token);
		}
	}
	else {
		syntax_error();
	}
}

void Parser::parse_scope()
//scope -> ID LBRACE public_vars private_vars stmt_list RBRACE
{
	token = lexer.GetToken();
	if (token.token_type == ID) {
		table.currentScope = token.lexeme;
		token = lexer.GetToken();
		if (token.token_type == LBRACE) {
			parse_public_vars();
			parse_private_vars();
			parse_stmt_list();
			token = lexer.GetToken();
			if (token.token_type == RBRACE) {
			//End found
			}
			else {
				syntax_error();
			}
		}
		else {
			syntax_error();
		}
	}
	else {
		syntax_error();
	}
}

void Parser::parse_public_vars()
//var_list -> epsilon
//var_list -> PUBLIC COLON var_list SEMICOLON
{
	token = lexer.GetToken();
	if (token.token_type == PUBLIC) {
		table.currentAccess = PUBLIC_ACCESS;
		token = lexer.GetToken();
		if (token.token_type == COLON) {
			token = lexer.GetToken();
			if (token.token_type == ID) {
				lexer.UngetToken(token);
				parse_var_list();
			}
			else {
				syntax_error();
			}
		}
		else {
			syntax_error();
		}
	}
	else {
		lexer.UngetToken(token);
		return;
	}

	token = lexer.GetToken();
	if (token.token_type == SEMICOLON) {
	}
	else {
		syntax_error();
	}
}

void Parser::parse_private_vars()
//var_list -> epsilon
//var_list -> PRIVATE COLON var_list SEMICOLON
{
	token = lexer.GetToken();
	if (token.token_type == PRIVATE) {
		table.currentAccess = PRIVATE_ACCESS;
		token = lexer.GetToken();
		if (token.token_type == COLON) {
			token = lexer.GetToken();
			if (token.token_type == ID) {
				lexer.UngetToken(token);
				parse_var_list();
			}
			else {
				syntax_error();
			}
		}
		else {
			syntax_error();
		}
	}
	else {
		lexer.UngetToken(token);
		return;
	}

	token = lexer.GetToken();
	if (token.token_type == SEMICOLON) {
		//End found
	}
	else {
		syntax_error();
	}
}

void Parser::parse_stmt_list()
//stmt_list -> stmt
//stmt_list -> stmt stmt_list
{
	token = lexer.GetToken();
	if (token.token_type == ID) {
		Token token2 = lexer.GetToken();
		if (token2.token_type == EQUAL || token2.token_type == LBRACE) {
			lexer.UngetToken(token2);
			lexer.UngetToken(token);
			parse_stmt();
		}
		else {
			lexer.UngetToken(token2);
			lexer.UngetToken(token);
		}

		token = lexer.GetToken();
		if (token.token_type == ID) {
			lexer.UngetToken(token);
			parse_stmt_list();
		}
		else {
			lexer.UngetToken(token);
		}
	}
}

void Parser::parse_stmt()
//stmt -> ID EQUAL ID SEMICOLON
//stmt -> scope
{
	token = lexer.GetToken();
	if (token.token_type == ID) {
		
		Token token2 = lexer.GetToken();
		if (token2.token_type == EQUAL) {
			Token token3 = lexer.GetToken();
			Token token4 = lexer.GetToken();
			if (token3.token_type != ID && token4.token_type != SEMICOLON) {
				syntax_error();
			}
			else {
				assignments += table.searchList(token.lexeme) + " = " + table.searchList(token3.lexeme) + "\n";
			}
		}
		else if (token2.token_type == LBRACE) {
			lexer.UngetToken(token2);
			lexer.UngetToken(token);
			parse_scope();
		}
		else {
			syntax_error();
		}
	}
	else {
		lexer.UngetToken(token);
	}
}

void Parser::syntax_error()
{
	cout << "Syntax Error\n";
	exit(1);
}


void Parser::print() {
	parse_program();
	cout << assignments;
}

int main()
{
	/*
	std::ifstream in("test_comments_01.txt");
	std::streambuf* cinbuf = std::cin.rdbuf(); //save old buf
	std::cin.rdbuf(in.rdbuf()); //redirect std::cin to in.txt!
	*/
	Parser parser;
	parser.print();
}