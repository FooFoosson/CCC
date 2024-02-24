#include "lexer.h"
#include "parser.h"
#include "vm.h"

int main(){
	std::string file="../tests/parser_test_file.txt";
    ccc::SharedBuffer b;
	ccc::Lexer l;
	l.run(file, b);
	ccc::Parser* p=new ccc::LL1Parser(b);
	ccc::SyntaxTree ast;
	p->parse(ast);
    p->convertToAst(ast);
    ccc::StackBasedVM vm{ast};
    vm.run();

	ast.printSyntaxTree();
	delete p;
}
