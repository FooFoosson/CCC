#include "lexer.h"
#include <iostream>
#include <memory>
#include <string.h>
#include <unordered_map>

ccc::Token::Token(std::string lexeme, Terminal term)
	: lexeme(lexeme)
	, term(term)
{
}

ccc::Token::~Token()
{
}

bool ccc::Token::operator==(Token& other) const
{
	return other.lexeme == lexeme;
}

bool ccc::FiniteAutomaton::transition(char input)
{
	if (transitionTable.find(input) == transitionTable.end())
		return false;
	std::unordered_map<unsigned int, unsigned int>& row = transitionTable[input];
	if (row.find(currentState) == row.end())
		return false;
	currentState = row[currentState];
	return true;
}

ccc::FiniteAutomaton::FiniteAutomaton()
{
	currentState = 0;
}

ccc::FiniteAutomaton::~FiniteAutomaton()
{
}

ccc::OperatorAutomaton::OperatorAutomaton()
{
	transitionTable['<'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['<'][0] = 1;
	transitionTable['>'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['>'][0] = 1;
	transitionTable['='] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['='][0] = 1;
	transitionTable['='][1] = 2;
	transitionTable['='][4] = 2;
	transitionTable['!'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['!'][0] = 4;
	transitionTable['+'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['+'][0] = 3;
	transitionTable['-'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['-'][0] = 3;
	transitionTable['*'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['*'][0] = 3;
	transitionTable['/'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['/'][0] = 3;
	acceptingStates.insert(1);
	acceptingStates.insert(2);
	acceptingStates.insert(3);
}

ccc::OperatorAutomaton::~OperatorAutomaton()
{
}

ccc::Terminal ccc::OperatorAutomaton::getTerminal()
{
	return Terminal::OPERATOR;
}

ccc::BuiltinTypeAutomaton::BuiltinTypeAutomaton()
{
	transitionTable['i'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['i'][0] = 1;
	transitionTable['n'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['n'][1] = 2;
	transitionTable['t'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['t'][2] = 3;
	transitionTable['f'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['f'][0] = 4;
	transitionTable['l'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['l'][4] = 5;
	transitionTable['o'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['o'][5] = 6;
	transitionTable['a'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['a'][6] = 2;

	acceptingStates.insert(3);
}

ccc::Terminal ccc::BuiltinTypeAutomaton::getTerminal()
{
	return Terminal::BUILTIN_TYPE;
}

ccc::BuiltinTypeAutomaton::~BuiltinTypeAutomaton()
{
}

ccc::IntLiteralAutomaton::IntLiteralAutomaton()
{
	for (char i = '0'; i <= '9'; ++i) {
		transitionTable[i] = std::unordered_map<unsigned int, unsigned int>();
		transitionTable[i][0] = 1;
	}
	acceptingStates.insert(1);
}

ccc::Terminal ccc::IntLiteralAutomaton::getTerminal()
{
	return Terminal::INT_LITERAL;
}

ccc::IntLiteralAutomaton::~IntLiteralAutomaton()
{
}

ccc::FloatLiteralAutomaton::FloatLiteralAutomaton()
{
	transitionTable['0'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['0'][0] = 0;

	transitionTable['1'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['1'][0] = 0;
	transitionTable['2'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['2'][0] = 0;
	transitionTable['3'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['3'][0] = 0;
	transitionTable['4'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['4'][0] = 0;
	transitionTable['5'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['5'][0] = 0;
	transitionTable['6'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['6'][0] = 0;
	transitionTable['7'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['7'][0] = 0;
	transitionTable['8'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['8'][0] = 0;
	transitionTable['9'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['9'][0] = 0;
	transitionTable['.'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['.'][0] = 1;
	transitionTable['0'][1] = 2;
	transitionTable['1'][1] = 2;
	transitionTable['2'][1] = 2;
	transitionTable['3'][1] = 2;
	transitionTable['4'][1] = 2;
	transitionTable['5'][1] = 2;
	transitionTable['6'][1] = 2;
	transitionTable['7'][1] = 2;
	transitionTable['8'][1] = 2;
	transitionTable['9'][1] = 2;
	transitionTable['0'][2] = 2;
	transitionTable['1'][2] = 2;
	transitionTable['2'][2] = 2;
	transitionTable['3'][2] = 2;
	transitionTable['4'][2] = 2;
	transitionTable['5'][2] = 2;
	transitionTable['6'][2] = 2;
	transitionTable['7'][2] = 2;
	transitionTable['8'][2] = 2;
	transitionTable['9'][2] = 2;

	acceptingStates.insert(2);
}

ccc::Terminal ccc::FloatLiteralAutomaton::getTerminal()
{
	return Terminal::FLOAT_LITERAL;
}

ccc::FloatLiteralAutomaton::~FloatLiteralAutomaton()
{
}

ccc::SemicolonAutomaton::SemicolonAutomaton()
{
	transitionTable[';'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable[';'][0] = 1;

	acceptingStates.insert(1);
}

ccc::Terminal ccc::SemicolonAutomaton::getTerminal()
{
	return Terminal::SEMICOLON;
}

ccc::SemicolonAutomaton::~SemicolonAutomaton()
{
}

ccc::ScopeAutomaton::ScopeAutomaton()
{
	transitionTable['{'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['{'][0] = 1;

	transitionTable['}'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['}'][0] = 1;

	acceptingStates.insert(1);
}

ccc::Terminal ccc::ScopeAutomaton::getTerminal()
{
	return Terminal::SCOPE;
}

ccc::ScopeAutomaton::~ScopeAutomaton()
{
}

ccc::ControlFlowAutomaton::ControlFlowAutomaton()
{
	transitionTable['i'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['i'][0] = 1;

	transitionTable['f'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['f'][1] = 2;
	transitionTable['w'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['w'][0] = 3;

	transitionTable['h'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['h'][3] = 4;
	transitionTable['i'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['i'][4] = 5;

	transitionTable['l'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['l'][5] = 6;
	transitionTable['e'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['e'][6] = 2;

	acceptingStates.insert(2);
}

ccc::Terminal ccc::ControlFlowAutomaton::getTerminal()
{
	return Terminal::CONTROL_FLOW;
}

ccc::ControlFlowAutomaton::~ControlFlowAutomaton()
{
}

ccc::StringLiteralAutomaton::StringLiteralAutomaton()
{
	transitionTable['"'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['"'][0] = 1;
	transitionTable['"'][1] = 2;

	for (char i = 32; i < 127; ++i) {
		if (i == '"')
			continue;
		transitionTable[i][1] = 1;
	}

	acceptingStates.insert(2);
}

ccc::Terminal ccc::StringLiteralAutomaton::getTerminal()
{
	return Terminal::STRING_LITERAL;
}

ccc::StringLiteralAutomaton::~StringLiteralAutomaton()
{
}

ccc::IdAutomaton::IdAutomaton()
{
	for (char i = 48; i < 58; ++i) {
		transitionTable[i] = std::unordered_map<unsigned int, unsigned int>();
		transitionTable[i][0] = 0;
	}
	for (char i = 65; i < 91; ++i) {
		transitionTable[i] = std::unordered_map<unsigned int, unsigned int>();
		transitionTable[i][0] = 0;
	}
	for (char i = 97; i < 123; ++i) {
		transitionTable[i] = std::unordered_map<unsigned int, unsigned int>();
		transitionTable[i][0] = 0;
	}
	acceptingStates.insert(0);
}

ccc::Terminal ccc::IdAutomaton::getTerminal()
{
	return Terminal::ID;
}

ccc::IdAutomaton::~IdAutomaton()
{
}

ccc::Lexer::Lexer(std::string& filePath, std::vector<Token*>& sharedBuffer)
	: sharedBuffer(sharedBuffer)
{
	fstream.open(filePath);
	if (!fstream.is_open()) {
		std::cout << "Invalid file\n";
		//set error
	}
	inputBuffer = new char[4096];
	/* inputBuffer2.reserve(4096); */
	//only one of the following two for now
	reservedWords["while"] = Terminal::CONTROL_FLOW;
	reservedWords["for"] = Terminal::CONTROL_FLOW;
	reservedWords["if"] = Terminal::CONTROL_FLOW;
	reservedWords["int"] = Terminal::BUILTIN_TYPE;
	reservedWords["float"] = Terminal::BUILTIN_TYPE;
	//todo char and pointers
	reservedWords["<"] = Terminal::OPERATOR;
	reservedWords[">"] = Terminal::OPERATOR;
	reservedWords[">="] = Terminal::OPERATOR;
	reservedWords["<="] = Terminal::OPERATOR;
	reservedWords["=="] = Terminal::OPERATOR;
	reservedWords["="] = Terminal::OPERATOR;
	reservedWords["+"] = Terminal::OPERATOR;
	reservedWords["-"] = Terminal::OPERATOR;
	reservedWords["*"] = Terminal::OPERATOR;
	reservedWords["/"] = Terminal::OPERATOR;
	reservedWords[";"] = Terminal::SEMICOLON;
	reservedWords["{"] = Terminal::SCOPE;
	reservedWords["}"] = Terminal::SCOPE;
}

ccc::Lexer::~Lexer()
{
	delete inputBuffer;
}

void ccc::Lexer::run()
{
	//inout buffer local variable so you can call run on different files
	//TODO: make vector of smart instead of raw pointers
	std::vector<FiniteAutomaton*> automata {
		new StringLiteralAutomaton(),
		new BuiltinTypeAutomaton(),
		new SemicolonAutomaton,
		new ScopeAutomaton,
		new ControlFlowAutomaton(),
		new OperatorAutomaton(),
		new FloatLiteralAutomaton(),
		new IntLiteralAutomaton(),
		new IdAutomaton()
	};

	fstream.read(inputBuffer, 4096);
	for (int i = 0; i < fstream.gcount() - 1; ++i) {
		if (inputBuffer[i] == ' ' || inputBuffer[i] == '\t' || inputBuffer[i] == '\n')
			continue;
		int j = 0;
		for (FiniteAutomaton* dfa = automata[j]; j < automata.size(); dfa = automata[++j]) {
			int starting = i;
			while (dfa->transition(inputBuffer[i])) {
				std::cout << "i: " << i << ", j: " << j << "\n"
						  << std::flush;
				if (i == fstream.gcount() - 2 && !fstream.eof()) {
					//perhaps an error if above we check for the last char maybe then eof will not be set, read the docs
					//extract gcount to a readable variable
					//reloads the buffer, maybe extract it
					int end = i - starting + 1;
					memcpy(inputBuffer, inputBuffer + starting, end);
					//extract 4096 to a macro or variable named buffer_size
					fstream.read(inputBuffer + end, 4096 - end);
					i = end;
				} else if (i == fstream.gcount() - 2) {
					++i;
					break;
				} else
					++i;
			}
			if (dfa->acceptingStates.find(dfa->currentState) != dfa->acceptingStates.end()) {
				sharedBuffer.emplace_back(new Token(
					std::string(inputBuffer + starting, i - starting) /* Copy elision */,
					dfa->getTerminal()));
				--i;
				//or maybe have a func resetDFA
				//or move to dfa somehow e.g. transition doing this when it returns false
				dfa->currentState = 0;
				break;
			}
			dfa->currentState = 0;
			//if not id automaton
			//when will the id automaton fail?
			if (j != automata.size() - 1)
				i = starting;
		}
	}
}
