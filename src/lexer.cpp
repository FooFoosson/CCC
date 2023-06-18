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

std::unordered_map<ccc::Terminal, std::string> ccc::Token::terminalNames = {
    { Terminal::ID, "id" },
    { Terminal::BUILTIN_TYPE_INT, "int" },
    { Terminal::BUILTIN_TYPE_FLOAT, "float" },
    { Terminal::BUILTIN_TYPE_CHAR, "char" },
    { Terminal::CONTROL_FLOW_BRANCH, "if" },
    { Terminal::CONTROL_FLOW_WHILE, "while" },
    { Terminal::ARITHMETIC_OP_PLUS, "+" },
    { Terminal::ARITHMETIC_OP_MINUS, "-" },
    { Terminal::ARITHMETIC_OP_MULT, "*" },
    { Terminal::ARITHMETIC_OP_DIV, "/" },
    { Terminal::LOGICAL_OP, "logop" },
    { Terminal::ASSIGNMENT_OP, "=" },
    { Terminal::INT_LITERAL, "int literal" },
    { Terminal::FLOAT_LITERAL, "float literal" },
    { Terminal::STRING_LITERAL, "string literal" },
    { Terminal::SEMICOLON, ";" },
    { Terminal::OPEN_SCOPE, "{" },
    { Terminal::CLOSED_SCOPE, "}" },
    { Terminal::OPENING_BRACKET, "(" },
    { Terminal::CLOSING_BRACKET, ")" },
    { Terminal::FILE_END, "eof" }
};

bool ccc::Token::operator==(const Token& other) const
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

ccc::ArithmeticOpAutomaton::ArithmeticOpAutomaton()
{
    transitionTable.reserve(4);
    transitionTable.emplace('+', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['+'].emplace(0, 1);
    transitionTable.emplace('-', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['-'].emplace(0, 2);
    transitionTable.emplace('*', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['*'].emplace(0, 3);
    transitionTable.emplace('/', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['/'].emplace(0, 4);

    acceptingStates.insert(1);
    acceptingStates.insert(2);
    acceptingStates.insert(3);
    acceptingStates.insert(4);
}

ccc::Terminal ccc::ArithmeticOpAutomaton::getTerminal()
{
    switch (currentState) {
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
    transitionTable.emplace('<', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['<'].emplace(0, 1);
    transitionTable.emplace('>', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['>'].emplace(0, 1);
    transitionTable.emplace('!', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['!'].emplace(0, 3);
    transitionTable.emplace('=', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['='].emplace(0, 3);
    transitionTable['='].emplace(1, 2);
    transitionTable['='].emplace(3, 2);

    acceptingStates.insert(1);
    acceptingStates.insert(2);
}

ccc::Terminal ccc::LogicalOpAutomaton::getTerminal()
{
    return Terminal::LOGICAL_OP;
}

ccc::AssignmentOpAutomaton::AssignmentOpAutomaton()
{
    transitionTable.emplace('=', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['='].emplace(0, 1);

    acceptingStates.insert(1);
}

ccc::Terminal ccc::AssignmentOpAutomaton::getTerminal()
{
    return Terminal::ASSIGNMENT_OP;
}

ccc::BuiltinTypeAutomaton::BuiltinTypeAutomaton()
{
    // add spaces so it isn't confused with identifiers
    transitionTable.reserve(10);
    transitionTable.emplace('i', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['i'].emplace(0, 1);
    transitionTable.emplace('n', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['n'].emplace(1, 2);
    transitionTable.emplace('t', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['t'].emplace(2, 3);
    transitionTable.emplace('f', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['f'].emplace(0, 4);
    transitionTable.emplace('l', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['l'].emplace(4, 5);
    transitionTable.emplace('o', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['o'].emplace(5, 6);
    transitionTable.emplace('a', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['a'].emplace(6, 7);
    transitionTable['t'].emplace(7, 8);
    transitionTable.emplace('c', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['c'].emplace(0, 9);
    transitionTable.emplace('h', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['h'].emplace(9, 10);
    transitionTable['a'].emplace(10, 11);
    transitionTable.emplace('r', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['r'].emplace(11, 12);

    acceptingStates.insert(3);
    acceptingStates.insert(8);
    acceptingStates.insert(12);
}

ccc::Terminal ccc::BuiltinTypeAutomaton::getTerminal()
{
    switch (currentState) {
    case 3:
        return Terminal::BUILTIN_TYPE_INT;
    case 8:
        return Terminal::BUILTIN_TYPE_FLOAT;
    case 12:
        return Terminal::BUILTIN_TYPE_CHAR;
    default:
        return Terminal::ERROR;
    }
}

ccc::IntLiteralAutomaton::IntLiteralAutomaton()
{
    transitionTable.reserve(10);
    for (char i = '0'; i <= '9'; ++i) {
        transitionTable.emplace(i, std::unordered_map<unsigned int, unsigned int> {});
        transitionTable[i].emplace(0, 1);
        transitionTable[i].emplace(1, 1);
    }

    acceptingStates.insert(1);
}

ccc::Terminal ccc::IntLiteralAutomaton::getTerminal()
{
    return Terminal::INT_LITERAL;
}

ccc::FloatLiteralAutomaton::FloatLiteralAutomaton()
{
    transitionTable.reserve(10);
    for (char i = '0'; i <= '9'; ++i) {
        transitionTable.emplace(i, std::unordered_map<unsigned int, unsigned int> {});
        transitionTable[i].emplace(0, 0);
        transitionTable[i].emplace(1, 2);
        transitionTable[i].emplace(2, 2);
    }
    transitionTable.emplace('.', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['.'].emplace(0, 1);

    acceptingStates.insert(2);
}

ccc::Terminal ccc::FloatLiteralAutomaton::getTerminal()
{
    return Terminal::FLOAT_LITERAL;
}

ccc::SemicolonAutomaton::SemicolonAutomaton()
{
    transitionTable.emplace(';', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable[';'].emplace(0, 1);

    acceptingStates.insert(1);
}

ccc::Terminal ccc::SemicolonAutomaton::getTerminal()
{
    return Terminal::SEMICOLON;
}

ccc::ScopeAutomaton::ScopeAutomaton()
{
    transitionTable.emplace('{', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['{'].emplace(0, 1);
    transitionTable.emplace('}', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['}'].emplace(0, 2);

    acceptingStates.insert(1);
    acceptingStates.insert(2);
}

ccc::Terminal ccc::ScopeAutomaton::getTerminal()
{
    switch (currentState) {
    case 1:
        return Terminal::OPEN_SCOPE;
    case 2:
        return Terminal::CLOSED_SCOPE;
    default:
        return Terminal::ERROR;
    }
}

ccc::BracketAutomaton::BracketAutomaton()
{
    transitionTable.emplace('(', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['('].emplace(0, 1);
    transitionTable.emplace(')', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable[')'].emplace(0, 2);

    acceptingStates.insert(1);
    acceptingStates.insert(2);
}

ccc::Terminal ccc::BracketAutomaton::getTerminal()
{
    switch (currentState) {
    case 1:
        return Terminal::OPENING_BRACKET;
    case 2:
        return Terminal::CLOSING_BRACKET;
    default:
        return Terminal::ERROR;
    }
}

ccc::ControlFlowAutomaton::ControlFlowAutomaton()
{
    transitionTable.reserve(7);
    transitionTable.emplace('i', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['i'].emplace(0, 1);
    transitionTable.emplace('f', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['f'].emplace(1, 2);
    transitionTable.emplace('w', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['w'].emplace(0, 3);
    transitionTable.emplace('h', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['h'].emplace(3, 4);
    transitionTable.emplace('i', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['i'].emplace(4, 5);
    transitionTable.emplace('l', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['l'].emplace(5, 6);
    transitionTable.emplace('e', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['e'].emplace(6, 7);

    acceptingStates.insert(2);
    acceptingStates.insert(7);
}

ccc::Terminal ccc::ControlFlowAutomaton::getTerminal()
{
    switch (currentState) {
    case 1:
        return Terminal::CONTROL_FLOW_BRANCH;
    case 2:
        return Terminal::CONTROL_FLOW_WHILE;
    default:
        return Terminal::ERROR;
    }
}

ccc::StringLiteralAutomaton::StringLiteralAutomaton()
{
    transitionTable.reserve(95);
    transitionTable.emplace('"', std::unordered_map<unsigned int, unsigned int> {});
    transitionTable['"'].emplace(0, 1);
    transitionTable['"'].emplace(1, 2);
    for (char i = 32; i < 127; ++i) {
        if (i == '"')
            continue;
        transitionTable[i].emplace(1, 1);
    }

    acceptingStates.insert(2);
}

ccc::Terminal ccc::StringLiteralAutomaton::getTerminal()
{
    return Terminal::STRING_LITERAL;
}

ccc::IdAutomaton::IdAutomaton()
{
    transitionTable.reserve(62);
    for (char i = '0'; i <= '9'; ++i) {
        transitionTable.emplace(i, std::unordered_map<unsigned int, unsigned int> {});
        transitionTable[i].emplace(0, 1);
        transitionTable[i].emplace(1, 1);
    }
    for (char i = 'A'; i <= 'Z'; ++i) {
        transitionTable.emplace(i, std::unordered_map<unsigned int, unsigned int> {});
        transitionTable[i].emplace(0, 1);
        transitionTable[i].emplace(1, 1);
    }
    for (char i = 'a'; i <= 'z'; ++i) {
        transitionTable.emplace(i, std::unordered_map<unsigned int, unsigned int> {});
        transitionTable[i].emplace(0, 1);
        transitionTable[i].emplace(1, 1);
    }
    acceptingStates.insert(1);
}

ccc::Terminal ccc::IdAutomaton::getTerminal()
{
    return Terminal::ID;
}

ccc::Lexer::Lexer()
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
    // TODO: dedicated error codes instead of bool
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
        new SemicolonAutomaton(),
        new ScopeAutomaton(),
        new ControlFlowAutomaton(),
        new ArithmeticOpAutomaton(),
        new LogicalOpAutomaton(),
        new AssignmentOpAutomaton(),
        new FloatLiteralAutomaton(),
        new IntLiteralAutomaton(),
        new IdAutomaton()
    };

    for (unsigned long long i = 0; i < numReadChars; ++i) {
        // TODO: support windows CR-LF for new line
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
