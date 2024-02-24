#pragma once
#include "lexer.h"
#include <deque>
#include <stack>
#include <unordered_map>
#include <vector>

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

enum class StorageSpecifier {
    STATIC,
    EXTERN,
    AUTO
};

struct Symbol {
    Symbol(Type type, StorageSpecifier storageSpecifier);

    Type type;
    StorageSpecifier storageSpecifier;
};

class SymbolTable {
public:
    SymbolTable();
    ~SymbolTable();

    bool lookup(const std::string& symbolName, Symbol& outSymbol);
    bool insert(const std::string& symbolName, const Symbol& symbol);
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
    void printSyntaxTree();

    struct SyntaxTreeNode {
        SyntaxTreeNode(Token val);
        virtual ~SyntaxTreeNode();

        Token val;
        SyntaxTreeNode* next;
        SyntaxTreeNode* children;
    };
    struct FunctionNode : SyntaxTreeNode {
    };

    SyntaxTreeNode* root;

private:
    void levelOrderTraversal(SyntaxTreeNode* node, unsigned int level, std::vector<std::vector<SyntaxTreeNode*>>& levels);
    std::unordered_map<std::string, std::deque<SyntaxTreeNode*>> nonTerminalsToNodes;
};

/* Each compilation unit should have their own Parser instance */
class Parser {
public:
    virtual ~Parser() = default;

    virtual bool parse(SyntaxTree& res) = 0;
    virtual void convertToAst(SyntaxTree& st) = 0;

protected:
    Parser(SharedBuffer& buffer);

    void addToSymbolTable(Token& token, Type type);
    SymbolTable symbolTable;
    SharedBuffer& buffer;
    /* Maps non-terminals to a map which maps terminals to productions */
    std::unordered_map<std::string, std::unordered_map<Terminal, std::vector<std::string>>> parsingTable;
    std::stack<std::string> grammarSymbols;
    std::unordered_set<std::string> terminals;
    const static std::unordered_map<Terminal, std::string> terminalsToProductions;
    const static std::unordered_map<std::string, Terminal> productionsToTerminals;
};

class LL1Parser : public Parser {
public:
    LL1Parser(SharedBuffer& buffer);

    bool parse(SyntaxTree& res) override;
    void convertToAst(SyntaxTree& st) override;

private:
    SyntaxTree::SyntaxTreeNode* convertToAstHelper(SyntaxTree::SyntaxTreeNode* node);
    void continueGrammarMatching();
    void expandProduction(SyntaxTree& st);
};

}
