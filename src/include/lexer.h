#pragma once
#include "utility.h"
#include <cmath>
#include <fstream>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ccc {

class FiniteAutomaton {
public:
    virtual ~FiniteAutomaton() = default;

    bool transition(char input);
    virtual Terminal getTerminal() const = 0;

    unsigned int currentState;
    std::unordered_set<unsigned int> acceptingStates;

protected:
    FiniteAutomaton();
    std::unordered_map<char, std::unordered_map<unsigned int, unsigned int>> transitionTable;
};

class ArithmeticOpAutomaton : public FiniteAutomaton {
public:
    ArithmeticOpAutomaton();

    Terminal getTerminal() const override;
};

class LogicalOpAutomaton : public FiniteAutomaton {
public:
    LogicalOpAutomaton();

    Terminal getTerminal() const override;
};

class AssignmentOpAutomaton : public FiniteAutomaton {
public:
    AssignmentOpAutomaton();

    Terminal getTerminal() const override;
};

class BuiltinTypeAutomaton : public FiniteAutomaton {
public:
    BuiltinTypeAutomaton();

    Terminal getTerminal() const override;
};

class ControlFlowAutomaton : public FiniteAutomaton {
public:
    ControlFlowAutomaton();

    Terminal getTerminal() const override;
};

class StringLiteralAutomaton : public FiniteAutomaton {
public:
    StringLiteralAutomaton();

    Terminal getTerminal() const override;
};

class ScopeAutomaton : public FiniteAutomaton {
public:
    ScopeAutomaton();

    Terminal getTerminal() const override;
};

class BracketAutomaton : public FiniteAutomaton {
public:
    BracketAutomaton();

    Terminal getTerminal() const override;
};

class SemicolonAutomaton : public FiniteAutomaton {
public:
    SemicolonAutomaton();

    Terminal getTerminal() const override;
};

class IntLiteralAutomaton : public FiniteAutomaton {
public:
    IntLiteralAutomaton();

    Terminal getTerminal() const override;
};

class FloatLiteralAutomaton : public FiniteAutomaton {
public:
    FloatLiteralAutomaton();

    Terminal getTerminal() const override;
};

class IdAutomaton : public FiniteAutomaton {
public:
    IdAutomaton();

    Terminal getTerminal() const override;
};

class Lexer {
public:
    Lexer();
    /* Only ASCII for now */
    bool run(const std::string& filePath, SharedBuffer& buffer);
};

}
