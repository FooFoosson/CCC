#pragma once
#include "globals.h"
#include <cmath>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ccc {

enum class Terminal {
	ID,
	BUILTIN_TYPE,
	CONTROL_FLOW,
	ARITHMETIC_OP,
	LOGICAL_OP,
	ASSIGNMENT_OP,
	INT_LITERAL,
	FLOAT_LITERAL,
	STRING_LITERAL,
	SEMICOLON,
	SCOPE
};

class Token {
public:
	Token(std::string lexeme, Terminal term);
	bool operator==(Token& other) const;
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
	std::unordered_map<char, std::unordered_map<unsigned int, unsigned int>> transitionTable;
	FiniteAutomaton();
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
	Lexer(std::vector<Token*>& sharedBuffer);
	/* Only ASCII for now */
	bool run(std::string filePath);
	~Lexer();

private:
	std::vector<Token*>& sharedBuffer;
	std::unordered_map<std::string, Terminal> reservedWords;
};

}
