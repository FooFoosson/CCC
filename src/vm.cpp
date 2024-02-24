#include "vm.h"

ccc::StackBasedVM::StackBasedVM(SyntaxTree& ast)
    : ast { ast }
{
    dispatchTable[Terminal::INT_LITERAL] = &StackBasedVM::pushOperand;
    dispatchTable[Terminal::FLOAT_LITERAL] = &StackBasedVM::pushOperand;
    dispatchTable[Terminal::ARITHMETIC_OP_DIV] = &StackBasedVM::binaryOp;
    dispatchTable[Terminal::ARITHMETIC_OP_MULT] = &StackBasedVM::binaryOp;
    dispatchTable[Terminal::ARITHMETIC_OP_PLUS] = &StackBasedVM::binaryOp;
    dispatchTable[Terminal::ARITHMETIC_OP_MINUS] = &StackBasedVM::binaryOp;
}

void ccc::StackBasedVM::run()
{
    eval(ast.root);
}

bool ccc::StackBasedVM::eval(SyntaxTree::SyntaxTreeNode* node)
{
    if (node == nullptr)
        return true;

    bool res = eval(node->children) && eval(node->next);

    auto func = dispatchTable[node->val.term];
    if (!(this->*func)(node->val))
        return false;

    return res;
}

bool ccc::StackBasedVM::pushOperand(Token token)
{
    operands.push(token);
    return true;
}

template <typename T>
T ccc::StackBasedVM::calc(T firstOperand, T secondOperand, Terminal term)
{
    T res;
    switch (term) {
    case Terminal::ARITHMETIC_OP_DIV:
        res = secondOperand / firstOperand;
        break;
    case Terminal::ARITHMETIC_OP_MULT:
        res = firstOperand * secondOperand;
        break;
    case Terminal::ARITHMETIC_OP_PLUS:
        res = firstOperand + secondOperand;
        break;
    case Terminal::ARITHMETIC_OP_MINUS:
        res = secondOperand - firstOperand;
        break;
    }
    return res;
}

bool ccc::StackBasedVM::binaryOp(Token token)
{
    if (operands.size() < 2)
        return false;

    Token firstOperand = operands.top();
    operands.pop();
    Token secondOperand = operands.top();
    operands.pop();

    if (firstOperand.term == Terminal::FLOAT_LITERAL || secondOperand.term == Terminal::FLOAT_LITERAL) {
        auto res = calc(std::stod(firstOperand.lexeme), std::stod(secondOperand.lexeme), token.term);
        operands.push({ std::to_string(res), Terminal::FLOAT_LITERAL });
    } else {
        auto res = calc(std::stoll(firstOperand.lexeme), std::stoll(secondOperand.lexeme), token.term);
        operands.push({ std::to_string(res), Terminal::INT_LITERAL });
    }

    return true;
}
