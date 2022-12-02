#include "lexer.h"
#include "parser.h"
#include <iostream>

int main(){
	std::queue<ccc::Token*> foo;
	std::string file="../tests/parser_test_file.txt";
	ccc::Lexer l;
	l.run(file, foo);
	/* while(!foo.empty()){ */
		/* std::cout<<foo.front()->lexeme<<std::endl; */
		/* foo.pop(); */
	/* } */
	ccc::LL1Parser p(foo);
	p.parsingAlgorithm();
}
