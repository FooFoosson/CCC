#include "parser.h"
#include <queue>
#include <unordered_map>
#include <iostream>

ccc::Parser::Parser(std::queue<Token*>& sharedBuffer)
	: sharedBuffer(sharedBuffer)
{
	grammarSymbols.push("$");
	grammarSymbols.push("E");
}

ccc::LL1Parser::LL1Parser(std::queue<Token*>& sharedBuffer)
	: Parser(sharedBuffer)
{
	//is emplace better?
	parsingTable["E"] = std::unordered_map<Terminal, std::vector<std::string>>();
	parsingTable["E"][Terminal::ID] = std::vector<std::string>{"S", "E'"};
	parsingTable["E"][Terminal::INT_LITERAL] = std::vector<std::string>{"S", "E'"};
	parsingTable["E"][Terminal::FLOAT_LITERAL] = std::vector<std::string>{"S", "E'"};
	parsingTable["E"][Terminal::OPENING_BRACKET] = std::vector<std::string>{"S", "E'"};

	parsingTable["S"] = std::unordered_map<Terminal, std::vector<std::string>>();
	parsingTable["S"][Terminal::ARITHMETIC_OP_PLUS] = std::vector<std::string>{"S", "E'", "+"};
	parsingTable["S"][Terminal::ARITHMETIC_OP_MINUS] = std::vector<std::string>{"S", "E'", "-"};
	parsingTable["S"][Terminal::FILE_END] = std::vector<std::string>{"epsilon"};
	parsingTable["S"][Terminal::CLOSING_BRACKET] = std::vector<std::string>{"epsilon"};

	parsingTable["E'"] = std::unordered_map<Terminal, std::vector<std::string>>();
	parsingTable["E'"][Terminal::ID] = std::vector<std::string>{"F", "T"};
	parsingTable["E'"][Terminal::INT_LITERAL] = std::vector<std::string>{"F", "T"};
	parsingTable["E'"][Terminal::FLOAT_LITERAL] = std::vector<std::string>{"F", "T"};
	parsingTable["E'"][Terminal::OPENING_BRACKET] = std::vector<std::string>{"F", "T"};

	parsingTable["F"] = std::unordered_map<Terminal, std::vector<std::string>>();
	parsingTable["F"][Terminal::ARITHMETIC_OP_MULT] = std::vector<std::string>{"F", "T", "*"};
	parsingTable["F"][Terminal::ARITHMETIC_OP_DIV] = std::vector<std::string>{"F", "T", "/"};
	parsingTable["F"][Terminal::ARITHMETIC_OP_PLUS] = std::vector<std::string>{"epsilon"};
	parsingTable["F"][Terminal::ARITHMETIC_OP_MINUS] = std::vector<std::string>{"epsilon"};
	parsingTable["F"][Terminal::FILE_END] = std::vector<std::string>{"epsilon"};
	parsingTable["F"][Terminal::CLOSING_BRACKET] = std::vector<std::string>{"epsilon"};

	parsingTable["T"] = std::unordered_map<Terminal, std::vector<std::string>>();
	parsingTable["T"][Terminal::ID] = std::vector<std::string>{"id"};
	parsingTable["T"][Terminal::INT_LITERAL] = std::vector<std::string>{"int literal"};
	parsingTable["T"][Terminal::FLOAT_LITERAL] = std::vector<std::string>{"float literal"};
	parsingTable["T"][Terminal::OPENING_BRACKET] = std::vector<std::string>{")", "E", "("};
}

void ccc::Parser::expandProduction(std::string& grammarSymbol, Terminal term)
{
	grammarSymbols.pop();
	for (std::string& production : parsingTable[grammarSymbol][term])
		grammarSymbols.push(production);
}

void ccc::Parser::continueGrammarMatching(size_t& i)
{
	grammarSymbols.pop();
	sharedBuffer.pop();
}

bool ccc::LL1Parser::parsingAlgorithm()
{
	AST res;
	std::unordered_set<std::string> terminals;
	size_t i = 0;
	std::string currentStackSymbol = grammarSymbols.top();

	while (currentStackSymbol != std::string("$")) {
		std::cout<<currentStackSymbol<<std::endl;
		Terminal inputTerm = sharedBuffer.front()->term;

		if (currentStackSymbol == Token::terminalNames[inputTerm])
			continueGrammarMatching(i);
		else if (terminals.find(currentStackSymbol) != terminals.end()) {
			//error currentStackSymbol is a terminal but not the one in the input which means the grammar did not match
			std::cout<<"Error\n";
			return false;
		} else if (parsingTable[currentStackSymbol].find(inputTerm) == parsingTable[currentStackSymbol].end()) {
			//error currentStackSymbol is a non terminal but there is no entry in the parsing table for the input terminal and the current non terminal and thusly the grammar did not match
			std::cout<<"Error\n";
			return false;
		} else
			expandProduction(currentStackSymbol, inputTerm);

		currentStackSymbol = grammarSymbols.top();
	}
	return true;
}
