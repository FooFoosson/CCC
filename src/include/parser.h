#pragma once
#include "lexer.h"
#include <stack>
#include <unordered_map>

namespace ccc {

enum class Type {
    FUNC,
    INT,
    FLOAT,
    CHAR,
    STRING,
    INT_PTR,
    FLOAT_PTR,
    CHAR_PTR,
    STRING_PTR,
};

enum class Scope {
    STATIC,
    EXTERN,
    AUTO
};

struct Symbol {
    Symbol(Type type, Scope scope);

    Type type;
    Scope scope;
};

class SymbolTable {
public:
    SymbolTable();
    ~SymbolTable();

    bool lookup(std::string& symbolName, Symbol& outSymbol);
    bool insert(std::string& symbolName, Symbol& symbol);
    void addScope();
    void removeScope();

private:
    std::stack<std::unordered_map<std::string, Symbol>*> symbols;
    std::stack<std::unordered_map<std::string, Symbol>*> helperStack;
};

class SyntaxTree {
public:
    SyntaxTree();
    ~SyntaxTree();

    bool insert(const std::string& parentNonTerminal, const std::vector<Token>& tokens);

    class SyntaxTreeNode {
    public:
        SyntaxTreeNode(Token val);
        ~SyntaxTreeNode();

        Token val;
        SyntaxTreeNode* next;
        SyntaxTreeNode* children;
    };
    SyntaxTreeNode* root;

private:
    std::unordered_map<std::string, std::stack<SyntaxTreeNode*>> nonTerminalsToNodes;
};

/* Each compilation unit should have their own Parser instance */
class Parser {
public:
    virtual ~Parser() = default;

    virtual bool parsingAlgorithm(SyntaxTree& res) = 0;

protected:
    Parser(std::queue<Token*>& sharedBuffer);

    /* void addToSymbolTable(Token& token); */
    SymbolTable symbolTable;
    std::queue<Token*>& sharedBuffer;
    /* Maps non-terminals to a map which maps terminals to productions */
    std::unordered_map<std::string, std::unordered_map<Terminal, std::vector<std::string>>> parsingTable;
    std::stack<std::string> grammarSymbols;
    std::unordered_set<std::string> terminals;
};

class LL1Parser : public Parser {
public:
    LL1Parser(std::queue<Token*>& sharedBuffer);

    bool parsingAlgorithm(SyntaxTree& res) override;

private:
    void continueGrammarMatching();
    void expandProduction(SyntaxTree& st);
};

}
