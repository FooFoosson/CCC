#include "lexer.h"
#include <iostream>

int main(){
	std::vector<ccc::Token*> foo;
	std::string file="../tests/lexer_test_file.txt";
	ccc::Lexer l(foo);
	l.setFile(file);
	l.run();
	for(ccc::Token* f:foo)
		std::cout<<f->lexeme<<std::endl;
}
