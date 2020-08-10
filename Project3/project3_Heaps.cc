#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>

#include "inputbuf.h"
#include "lexer.h"
#include "parser.h"

using namespace std;

int main()
{
	/*
	ifstream in("test.txt");
	streambuf* cinbuf = cin.rdbuf();
	std::cin.rdbuf(in.rdbuf());
	*/
	Parser parser;
	int i;
	i = parser.parse_program();
	parser.printList();
}
