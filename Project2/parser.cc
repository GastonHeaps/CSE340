#include <iostream>
#include <fstream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "lexer.h"
#include "parser.h"

using namespace std;

Parser parser;
LexicalAnalyzer lexer;
Token token;
SymbolTable table;

SymbolTable::SymbolTable()
{
	current = new scopeTable;
	current->previous = NULL;
	pointer = current;
}
void SymbolTable::addScope(string scope)
{
	scopeTable* n = new scopeTable();
	n->scope = scope;
	if (pointer != NULL) {
		n->previous = pointer;
	}
	pointer = n;
	current = pointer;
}
void SymbolTable::exitScope()
{
	pointer = current;
	if (pointer->previous == NULL) {
		pointer = NULL;
	}
	else {
		pointer = pointer->previous;
	}
	current = pointer;
}
void SymbolTable::addVariable(string var, AccessSpecifier access) {
	pointer = current;
	symbolTable n = symbolTable();
	n.variable = var;
	n.access = access;
	pointer->symbols.emplace_back(n);
}
void SymbolTable::addAssignment(string lhs, string rhs) {
	assignments.emplace_back(std::make_pair(findVariable(lhs), findVariable(rhs)));
}
symbolTable SymbolTable::findVariable(string var) {
	pointer = current;
	symbolTable n = symbolTable();
	while (pointer != NULL) {
		for (auto i = pointer->symbols.begin(); i != pointer->symbols.end(); ++i) {
			if (i->variable == var) {
				n.variable = i->variable;
				n.access = i->access;
				n.declaration = pointer->scope;
				if (i->access == PRIVATE_ACCESS) {
					if (pointer->scope == current->scope) {
						return n;
					}
				}
				else {
					return n;
				}
			}
		}

		if (pointer->previous != NULL) {
			pointer = pointer->previous;
		}
		else {
			pointer = NULL;
		}
	}
	n.variable = var;
	n.declaration = "?";
	return n;
}

Parser::Parser() {

}

void Parser::parse_program()
{
	bool scopeParsed = false;
	table.addScope("::");
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
		table.exitScope();
	}
	else {
		syntax_error();
	}
}
void Parser::print() {
	for (auto i = table.assignments.begin(); i != table.assignments.end(); ++i) {
		cout << i->first.declaration << ((i->first.declaration != "::") ? "." : "") << i->first.variable << " = " << i->second.declaration << ((i->second.declaration != "::") ? "." : "") << i->second.variable << endl;
	}
}



void Parser::parse_global_vars()
{
	token = lexer.GetToken();
	if (token.token_type == ID) {
		lexer.UngetToken(token);
		parse_var_list(GLOBAL_ACCESS);
	}
	else {
		syntax_error();
	}

	token = lexer.GetToken();
	if (token.token_type == SEMICOLON) {
	}
	else {
		syntax_error();
	}
}
void Parser::parse_var_list(AccessSpecifier access)
{
	token = lexer.GetToken();
	if (token.token_type == ID) {
		table.addVariable(token.lexeme, access);
		token = lexer.GetToken();
		if (token.token_type == COMMA) {
			parse_var_list(access);
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
{
	token = lexer.GetToken();
	if (token.token_type == ID) {
		table.addScope(token.lexeme);
		token = lexer.GetToken();
		if (token.token_type == LBRACE) {
			parse_public_vars();
			parse_private_vars();
			parse_stmt_list();
			token = lexer.GetToken();
			if (token.token_type == RBRACE) {
				table.exitScope();
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
{
	token = lexer.GetToken();
	if (token.token_type == PUBLIC) {
		token = lexer.GetToken();
		if (token.token_type == COLON) {
			token = lexer.GetToken();
			if (token.token_type == ID) {
				lexer.UngetToken(token);
				parse_var_list(PUBLIC_ACCESS);
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
{
	token = lexer.GetToken();
	if (token.token_type == PRIVATE) {
		token = lexer.GetToken();
		if (token.token_type == COLON) {
			token = lexer.GetToken();
			if (token.token_type == ID) {
				lexer.UngetToken(token);
				parse_var_list(PRIVATE_ACCESS);
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

void Parser::parse_stmt_list()
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
			return;
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
{
	token = lexer.GetToken();
	if (token.token_type == ID) {

		Token token2 = lexer.GetToken();
		if (token2.token_type == EQUAL) {
			Token token3 = lexer.GetToken();
			Token token4 = lexer.GetToken();
			if (token3.token_type == ID && token4.token_type == SEMICOLON) {
				table.addAssignment(token.lexeme, token3.lexeme);
			}
			else {
				syntax_error();
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
	exit(EXIT_FAILURE);
}


int main()
{
	/*
	ifstream in("tests/test_comments_01.txt");
	streambuf* cinbuf = cin.rdbuf();
	std::cin.rdbuf(in.rdbuf());
	*/
	parser.parse_program();
	parser.print();
	return 0;
}