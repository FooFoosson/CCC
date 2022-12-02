#pragma once
#include "globals.h"
#include <cmath>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>

namespace ccc {

enum class Terminal {
	ID,
	BUILTIN_TYPE_INT,
	BUILTIN_TYPE_FLOAT,
	BUILTIN_TYPE_CHAR,
	CONTROL_FLOW,
	ARITHMETIC_OP_PLUS,
	ARITHMETIC_OP_MINUS,
	ARITHMETIC_OP_MULT,
	ARITHMETIC_OP_DIV,
	LOGICAL_OP,
	ASSIGNMENT_OP,
	INT_LITERAL,
	FLOAT_LITERAL,
	STRING_LITERAL,
	SEMICOLON,
	OPEN_SCOPE,
	CLOSED_SCOPE,
	OPENING_BRACKET,
	CLOSING_BRACKET,
	FILE_END,
	ERROR
};

struct Token {
	Token(std::string lexeme, Terminal term);
	bool operator==(Token& other) const;
	static std::unordered_map<Terminal, std::string> terminalNames;
	~Token();

	std::string lexeme;
	Terminal term;
};

class FiniteAutomaton {
public:
	bool transition(char input);
	virtual Terminal getTerminal() = 0;
	virtual ~FiniteAutomaton();

	unsigned int currentState;
	std::unordered_set<unsigned int> acceptingStates;
protected:
	FiniteAutomaton();
	std::unordered_map<char, std::unordered_map<unsigned int, unsigned int>> transitionTable;
};

class ArithmeticOpAutomaton : public FiniteAutomaton {
public:
	Terminal getTerminal() override;
	ArithmeticOpAutomaton();
	~ArithmeticOpAutomaton();
};

class LogicalOpAutomaton : public FiniteAutomaton {
public:
	Terminal getTerminal() override;
	LogicalOpAutomaton();
	~LogicalOpAutomaton();
};

class AssignmentOpAutomaton : public FiniteAutomaton {
public:
	Terminal getTerminal() override;
	AssignmentOpAutomaton();
	~AssignmentOpAutomaton();
};

class BuiltinTypeAutomaton : public FiniteAutomaton {
public:
	Terminal getTerminal() override;
	BuiltinTypeAutomaton();
	~BuiltinTypeAutomaton();
};

class ControlFlowAutomaton : public FiniteAutomaton {
public:
	Terminal getTerminal() override;
	ControlFlowAutomaton();
	~ControlFlowAutomaton();
};

class StringLiteralAutomaton : public FiniteAutomaton {
public:
	Terminal getTerminal() override;
	StringLiteralAutomaton();
	~StringLiteralAutomaton();
};

class ScopeAutomaton : public FiniteAutomaton {
public:
	Terminal getTerminal() override;
	ScopeAutomaton();
	~ScopeAutomaton();
};

class BracketAutomaton : public FiniteAutomaton {
public:
	Terminal getTerminal() override;
	BracketAutomaton();
	~BracketAutomaton();
};

class SemicolonAutomaton : public FiniteAutomaton {
public:
	Terminal getTerminal() override;
	SemicolonAutomaton();
	~SemicolonAutomaton();
};

class IntLiteralAutomaton : public FiniteAutomaton {
public:
	Terminal getTerminal() override;
	IntLiteralAutomaton();
	~IntLiteralAutomaton();
};

class FloatLiteralAutomaton : public FiniteAutomaton {
public:
	Terminal getTerminal() override;
	FloatLiteralAutomaton();
	~FloatLiteralAutomaton();
};

class IdAutomaton : public FiniteAutomaton {
public:
	Terminal getTerminal() override;
	IdAutomaton();
	~IdAutomaton();
};

class Lexer {
public:
	Lexer();
	/* Only ASCII for now */
	bool run(std::string filePath, std::queue<Token*>& sharedBuffer);
	~Lexer();

private:
	/* std::unordered_map<std::string, Terminal> reservedWords; */
};

}
