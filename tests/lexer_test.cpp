#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <queue>

std::vector<std::vector<ccc::SyntaxTree::SyntaxTreeNode*>> res;

//put into ast
void printSyntaxTree(ccc::SyntaxTree::SyntaxTreeNode* root, int level){
	if(!root)
		return;
	if(res.size()==level)
		res.push_back(std::vector<ccc::SyntaxTree::SyntaxTreeNode*>());
	res[level].push_back(root);
	ccc::SyntaxTree::SyntaxTreeNode* child=root->children;
	while(child){
		printSyntaxTree(child, level+1);
		child=child->next;
	}
}

int main(){
	std::queue<ccc::Token*> foo;
	std::string file="../tests/parser_test_file.txt";
	ccc::Lexer l;
	l.run(file, foo);
	/* while(!foo.empty()){ */
		/* std::cout<<foo.front()->lexeme<<std::endl; */
		/* foo.pop(); */
	/* } */
	ccc::Parser* p=new ccc::LL1Parser(foo);
	ccc::SyntaxTree ast;
	p->parsingAlgorithm(ast);
	printSyntaxTree(ast.root, 0);
	for(auto& i:res){
		for(auto j:i)
			std::cout<<j->val.lexeme;
		std::cout<<std::endl;
	}
	delete p;
}
