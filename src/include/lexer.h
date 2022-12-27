#pragma once
#include "globals.h"
#include <cmath>
#include <fstream>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ccc {

enum class Terminal {
    ID,
    BUILTIN_TYPE_INT,
    BUILTIN_TYPE_FLOAT,
    BUILTIN_TYPE_CHAR,
    CONTROL_FLOW_BRANCH,
    CONTROL_FLOW_WHILE,
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
    NON_TERMINAL,
    ERROR
};

struct Token {
    Token(std::string lexeme, Terminal term);
    Token(const Token& other) = default;
    Token(Token&& other) = default;
    Token& operator=(const Token& other) = default;
    Token& operator=(Token&& other) = default;
    ~Token()=default;

    bool operator==(const Token& other) const;
    static std::unordered_map<Terminal, std::string> terminalNames;

    std::string lexeme;
    Terminal term;
};

class FiniteAutomaton {
public:
    virtual ~FiniteAutomaton()=default;

    bool transition(char input);
    virtual Terminal getTerminal() = 0;

    unsigned int currentState;
    std::unordered_set<unsigned int> acceptingStates;
protected:
    FiniteAutomaton();
    std::unordered_map<char, std::unordered_map<unsigned int, unsigned int>> transitionTable;
};

class ArithmeticOpAutomaton : public FiniteAutomaton {
public:
    ArithmeticOpAutomaton();
    ~ArithmeticOpAutomaton()=default;

    Terminal getTerminal() override;
};

class LogicalOpAutomaton : public FiniteAutomaton {
public:
    LogicalOpAutomaton();
    ~LogicalOpAutomaton()=default;

    Terminal getTerminal() override;
};

class AssignmentOpAutomaton : public FiniteAutomaton {
public:
    AssignmentOpAutomaton();
    ~AssignmentOpAutomaton()=default;

    Terminal getTerminal() override;
};

class BuiltinTypeAutomaton : public FiniteAutomaton {
public:
    BuiltinTypeAutomaton();
    ~BuiltinTypeAutomaton()=default;

    Terminal getTerminal() override;
};

class ControlFlowAutomaton : public FiniteAutomaton {
public:
    ControlFlowAutomaton();
    ~ControlFlowAutomaton()=default;

    Terminal getTerminal() override;
};

class StringLiteralAutomaton : public FiniteAutomaton {
public:
    StringLiteralAutomaton();
    ~StringLiteralAutomaton()=default;

    Terminal getTerminal() override;
};

class ScopeAutomaton : public FiniteAutomaton {
public:
    ScopeAutomaton();
    ~ScopeAutomaton()=default;

    Terminal getTerminal() override;
};

class BracketAutomaton : public FiniteAutomaton {
public:
    BracketAutomaton();
    ~BracketAutomaton()=default;

    Terminal getTerminal() override;
};

class SemicolonAutomaton : public FiniteAutomaton {
public:
    SemicolonAutomaton();
    ~SemicolonAutomaton()=default;

    Terminal getTerminal() override;
};

class IntLiteralAutomaton : public FiniteAutomaton {
public:
    IntLiteralAutomaton();
    ~IntLiteralAutomaton()=default;

    Terminal getTerminal() override;
};

class FloatLiteralAutomaton : public FiniteAutomaton {
public:
    FloatLiteralAutomaton();
    ~FloatLiteralAutomaton()=default;

    Terminal getTerminal() override;
};

class IdAutomaton : public FiniteAutomaton {
public:
    IdAutomaton();
    ~IdAutomaton()=default;

    Terminal getTerminal() override;
};

class Lexer {
public:
    Lexer();
    ~Lexer()=default;
    /* Only ASCII for now */
    bool run(std::string filePath, std::queue<Token*>& sharedBuffer);
};

}
