#pragma once
#include "parser.h"
#include <stack>
#include <unordered_map>
#include <unordered_set>

namespace ccc {

class VM {
public:
    virtual ~VM() = default;

    virtual void run() = 0;
};

class StackBasedVM : public VM {
public:
    StackBasedVM(SyntaxTree& ast);
    void run() override;

private:
    bool eval(SyntaxTree::SyntaxTreeNode* node);
    bool binaryOp(Token token);
    bool pushOperand(Token token);
    template <typename T>
    T calc(T firstOperand, T secondOperand, Terminal op);

    SyntaxTree& ast;
    std::unordered_map<Terminal, bool (ccc::StackBasedVM::*)(Token)> dispatchTable;
    std::stack<Token> operands;
};

}
