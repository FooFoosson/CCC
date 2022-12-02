#include "lexer.h"
#include <cwchar>
#include <iostream>
#include <memory>
#include <string.h>
#include <unordered_map>

#define INPUT_BUFFER_SIZE 4096

ccc::Token::Token(std::string lexeme, Terminal term)
	: lexeme(lexeme)
	, term(term)
{
}

std::unordered_map<ccc::Terminal, std::string> ccc::Token::terminalNames={
	{Terminal::ID, "id"}, 
	{Terminal::INT_LITERAL, "int literal"},
	{Terminal::FLOAT_LITERAL, "float literal"},
	{Terminal::STRING_LITERAL, "string literal"},
	{Terminal::BUILTIN_TYPE_INT, "int"},
 	{Terminal::BUILTIN_TYPE_FLOAT, "float"},
 	{Terminal::BUILTIN_TYPE_CHAR, "char"},
 	{Terminal::CONTROL_FLOW, "if"},
 	{Terminal::ARITHMETIC_OP_PLUS, "+"},
 	{Terminal::ARITHMETIC_OP_MINUS, "-"},
 	{Terminal::ARITHMETIC_OP_MULT, "*"},
 	{Terminal::ARITHMETIC_OP_DIV, "/"},
 	{Terminal::LOGICAL_OP, "logop"},
 	{Terminal::ASSIGNMENT_OP, "="},
	{Terminal::SEMICOLON, ";"},
	{Terminal::OPEN_SCOPE, "{"},
 	{Terminal::CLOSED_SCOPE, "}"},
 	{Terminal::OPENING_BRACKET, "("},
 	{Terminal::CLOSING_BRACKET, ")"},
 	{Terminal::FILE_END, "eof"}
};

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
	: currentState(0)
{
}

ccc::FiniteAutomaton::~FiniteAutomaton()
{
}

ccc::ArithmeticOpAutomaton::ArithmeticOpAutomaton()
{
	transitionTable.reserve(4);
	transitionTable['+'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['+'][0] = 1;
	transitionTable['-'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['-'][0] = 2;
	transitionTable['*'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['*'][0] = 3;
	transitionTable['/'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['/'][0] = 4;

	acceptingStates.insert(1);
	acceptingStates.insert(2);
	acceptingStates.insert(3);
	acceptingStates.insert(4);
}

ccc::ArithmeticOpAutomaton::~ArithmeticOpAutomaton()
{
}

ccc::Terminal ccc::ArithmeticOpAutomaton::getTerminal()
{
	switch(currentState){
	case 1:
		return Terminal::ARITHMETIC_OP_PLUS;
	case 2:
		return Terminal::ARITHMETIC_OP_MINUS;
	case 3:
		return Terminal::ARITHMETIC_OP_MULT;
	case 4:
		return Terminal::ARITHMETIC_OP_DIV;
	default:
		return Terminal::ERROR;
	}
}

ccc::LogicalOpAutomaton::LogicalOpAutomaton()
{
	transitionTable.reserve(4);
	transitionTable['<'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['<'][0] = 1;
	transitionTable['>'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['>'][0] = 1;
	transitionTable['!'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['!'][0] = 3;
	transitionTable['='] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['='][0] = 3;
	transitionTable['='][1] = 2;
	transitionTable['='][3] = 2;

	acceptingStates.insert(1);
	acceptingStates.insert(2);
}

ccc::LogicalOpAutomaton::~LogicalOpAutomaton()
{
}

ccc::Terminal ccc::LogicalOpAutomaton::getTerminal()
{
	return Terminal::LOGICAL_OP;
}

ccc::AssignmentOpAutomaton::AssignmentOpAutomaton()
{
	transitionTable['='] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['='][0] = 1;

	acceptingStates.insert(1);
}

ccc::AssignmentOpAutomaton::~AssignmentOpAutomaton()
{
}

ccc::Terminal ccc::AssignmentOpAutomaton::getTerminal()
{
	return Terminal::ASSIGNMENT_OP;
}

ccc::BuiltinTypeAutomaton::BuiltinTypeAutomaton()
{
	transitionTable.reserve(7);
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
	transitionTable['a'][6] = 7;
	transitionTable['t'][7] = 8;

	acceptingStates.insert(3);
	acceptingStates.insert(8);
}

ccc::Terminal ccc::BuiltinTypeAutomaton::getTerminal()
{
	switch (currentState) {
	case 3:
		return Terminal::BUILTIN_TYPE_INT;
	case 8:
		return Terminal::BUILTIN_TYPE_FLOAT;
	default:
		return Terminal::ERROR;
	}
}

ccc::BuiltinTypeAutomaton::~BuiltinTypeAutomaton()
{
}

ccc::IntLiteralAutomaton::IntLiteralAutomaton()
{
	transitionTable.reserve(10);
	for (char i = '0'; i <= '9'; ++i) {
		transitionTable[i] = std::unordered_map<unsigned int, unsigned int>();
		transitionTable[i][0] = 1;
		transitionTable[i][1] = 1;
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
	transitionTable.reserve(10);
	for (char i = '0'; i <= '9'; ++i) {
		transitionTable[i] = std::unordered_map<unsigned int, unsigned int>();
		transitionTable[i][0] = 0;
		transitionTable[i][1] = 2;
		transitionTable[i][2] = 2;
	}
	transitionTable['.'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['.'][0] = 1;

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
	transitionTable['}'][0] = 2;

	acceptingStates.insert(1);
	acceptingStates.insert(2);
}

ccc::Terminal ccc::ScopeAutomaton::getTerminal()
{
	switch(currentState){
		case 1:
			return Terminal::OPEN_SCOPE;
		case 2:
			return Terminal::CLOSED_SCOPE;
		default:
			return Terminal::ERROR;
	}
}

ccc::ScopeAutomaton::~ScopeAutomaton()
{
}

ccc::BracketAutomaton::BracketAutomaton()
{
	transitionTable['('] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable['('][0] = 1;
	transitionTable[')'] = std::unordered_map<unsigned int, unsigned int>();
	transitionTable[')'][0] = 2;

	acceptingStates.insert(1);
	acceptingStates.insert(2);
}

ccc::Terminal ccc::BracketAutomaton::getTerminal()
{
	switch(currentState){
		case 1:
			return Terminal::OPENING_BRACKET;
		case 2:
			return Terminal::CLOSING_BRACKET;
		default:
			return Terminal::ERROR;
	}
}

ccc::BracketAutomaton::~BracketAutomaton()
{
}

ccc::ControlFlowAutomaton::ControlFlowAutomaton()
{
	transitionTable.reserve(7);
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
	transitionTable.reserve(95);
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
	transitionTable.reserve(62);
	for (char i = 48; i < 58; ++i) {
		transitionTable[i] = std::unordered_map<unsigned int, unsigned int>();
		transitionTable[i][0] = 1;
		transitionTable[i][1] = 1;
	}
	for (char i = 65; i < 91; ++i) {
		transitionTable[i] = std::unordered_map<unsigned int, unsigned int>();
		transitionTable[i][0] = 1;
		transitionTable[i][1] = 1;
	}
	for (char i = 97; i < 123; ++i) {
		transitionTable[i] = std::unordered_map<unsigned int, unsigned int>();
		transitionTable[i][0] = 1;
		transitionTable[i][1] = 1;
	}
	acceptingStates.insert(1);
}

ccc::Terminal ccc::IdAutomaton::getTerminal()
{
	return Terminal::ID;
}

ccc::IdAutomaton::~IdAutomaton()
{
}

ccc::Lexer::Lexer()
{
	//only one of the following two for now
	/* reservedWords["while"] = Terminal::CONTROL_FLOW; */
	/* reservedWords["for"] = Terminal::CONTROL_FLOW; */
	/* reservedWords["if"] = Terminal::CONTROL_FLOW; */
	/* reservedWords["int"] = Terminal::BUILTIN_TYPE; */
	/* reservedWords["float"] = Terminal::BUILTIN_TYPE; */
	//todo char and pointers
	/* reservedWords["<"] = Terminal::OPERATOR; */
	/* reservedWords[">"] = Terminal::OPERATOR; */
	/* reservedWords[">="] = Terminal::OPERATOR; */
	/* reservedWords["<="] = Terminal::OPERATOR; */
	/* reservedWords["=="] = Terminal::OPERATOR; */
	/* reservedWords["="] = Terminal::OPERATOR; */
	/* reservedWords["+"] = Terminal::OPERATOR; */
	/* reservedWords["-"] = Terminal::OPERATOR; */
	/* reservedWords["*"] = Terminal::OPERATOR; */
	/* reservedWords["/"] = Terminal::OPERATOR; */
	/* reservedWords[";"] = Terminal::SEMICOLON; */
	/* reservedWords["{"] = Terminal::SCOPE; */
	/* reservedWords["}"] = Terminal::SCOPE; */
}

ccc::Lexer::~Lexer()
{
}

static void reloadInputBuffer(unsigned long long& current, unsigned long long starting, unsigned long long& numReadChars, char*& inputBuffer, std::ifstream& file)
{
	unsigned long long end = current - starting + 1;
	memcpy(inputBuffer, inputBuffer + starting, end);
	file.read(inputBuffer + end, INPUT_BUFFER_SIZE - end);
	numReadChars = end + file.gcount() - 1;
	current = end;
}

bool ccc::Lexer::run(std::string filePath, std::queue<Token*>& sharedBuffer)
{
	//TODO: dedicated error codes instead of bool
	std::ifstream file;
	file.open(filePath);
	if (!file.is_open())
		return false;

	char* inputBuffer = new char[INPUT_BUFFER_SIZE];
	file.read(inputBuffer, INPUT_BUFFER_SIZE);
	unsigned long long numReadChars = file.gcount() - 1;

	std::vector<FiniteAutomaton*> automata {
		new StringLiteralAutomaton(),
		new BuiltinTypeAutomaton(),
		new SemicolonAutomaton,
		new ScopeAutomaton,
		new ControlFlowAutomaton(),
		new ArithmeticOpAutomaton(),
		new LogicalOpAutomaton(),
		new AssignmentOpAutomaton(),
		new FloatLiteralAutomaton(),
		new IntLiteralAutomaton(),
		new IdAutomaton()
	};

	for (unsigned long long i = 0; i < numReadChars; ++i) {
		if (inputBuffer[i] == ' ' || inputBuffer[i] == '\t' || inputBuffer[i] == '\n')
			continue;
		FiniteAutomaton* dfa;
		for (FiniteAutomaton* dfa : automata) {
			unsigned long long starting = i;
			while (dfa->transition(inputBuffer[i])) {
				if (i == numReadChars - 1 && !file.eof())
					reloadInputBuffer(i, starting, numReadChars, inputBuffer, file);
				else if (i++ == numReadChars - 1)
					break;
			}
			if (dfa->acceptingStates.find(dfa->currentState) != dfa->acceptingStates.end()) {
				sharedBuffer.push(new Token(
					std::string(inputBuffer + starting, i - starting) /* Copy elision */,
					dfa->getTerminal()));
				--i;
				dfa->currentState = 0;
				break;
			}
			dfa->currentState = 0;
			if (dfa->getTerminal() == Terminal::ID)
				return false;
			i = starting;
		}
	}
	sharedBuffer.push(new Token("eof", Terminal::FILE_END));

	file.close();
	delete[] inputBuffer;
	for (FiniteAutomaton* dfa : automata)
		delete dfa;

	return true;
}
