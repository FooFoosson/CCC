#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

namespace ccc {

// add arrays and pointers
enum class Terminal {
    ID,
    BUILTIN_TYPE_INT,
    BUILTIN_TYPE_FLOAT,
    BUILTIN_TYPE_CHAR,
    INT_PTR,
    FLOAT_PTR,
    CHAR_PTR,
    CONTROL_FLOW_BRANCH,
    CONTROL_FLOW_WHILE,
    ARITHMETIC_OP_PLUS,
    ARITHMETIC_OP_MINUS,
    ARITHMETIC_OP_MULT,
    ARITHMETIC_OP_DIV,
    LOGICAL_OP,
    ASSIGNMENT_OP,
    DEREFERENCE_OP,
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

    bool operator==(const Token& other) const;

    std::string lexeme;
    Terminal term;
};

class SharedBuffer {
public:
    SharedBuffer();
    ~SharedBuffer();

    void produce(Token* token);
    Token* consume();
    void pop();

private:
    std::queue<Token*> buffer;
    unsigned long long count;
    std::mutex m;
    std::condition_variable condition;
};

}
