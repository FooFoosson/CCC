#pragma once
#include "lexer.h"
#include <stack>
#include <unordered_map>

namespace ccc {

enum class Type {
	FUNC,
	INT,
	FLOAT,
};

class AST{

};

struct Symbol {
	Symbol(Type type, std::string lexeme);
	bool operator==(Symbol& other) const
	{
		return other.lexeme == lexeme;
	}

	Type type;
	std::string lexeme;
};

/* Each compilation unit should have their own Parser instance */
class Parser {
public:
	virtual bool parsingAlgorithm()=0;
protected:
	Parser(std::queue<Token*>& sharedBuffer);
	//is this helper method different for each parser type?
	void expandProduction(std::string& grammarSymbol, Terminal term);
	void continueGrammarMatching(size_t& i);

	//need a default symbol constructor or store pointers instead
	/* std::unordered_set<Symbol> symbolTable; */
	std::queue<Token*>& sharedBuffer;
	/* Maps non-terminals to a map which maps terminals to productions */
	std::unordered_map<std::string, std::unordered_map<Terminal, std::vector<std::string>>> parsingTable;
	std::stack<std::string> grammarSymbols;
};

class LL1Parser : public Parser {
public:
	LL1Parser(std::queue<Token*>& sharedBuffer);
	bool parsingAlgorithm() override;
};

}
