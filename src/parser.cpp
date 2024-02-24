#include "parser.h"

#include <algorithm>
#include <cstddef>
#include <iostream>

ccc::Symbol::Symbol(Type type, StorageSpecifier storageSpecifier)
    : type { type }
    , storageSpecifier { storageSpecifier }
{
}

ccc::SymbolTable::SymbolTable()
{
}

ccc::SymbolTable::~SymbolTable()
{
    while (!symbols.empty()) {
        delete symbols.top();
        symbols.pop();
    }
}

void ccc::SymbolTable::addScope()
{
    symbols.push(new std::unordered_map<std::string, Symbol> {});
}

void ccc::SymbolTable::removeScope()
{
    symbols.pop();
}

bool ccc::SymbolTable::insert(const std::string& symbolName, const Symbol& symbol)
{
    std::unordered_map<std::string, Symbol>* currentTable = symbols.top();
    if (currentTable->find(symbolName) != currentTable->end())
        return false;
    currentTable->insert({ symbolName, symbol });
    return true;
}

bool ccc::SymbolTable::lookup(const std::string& symbolName, Symbol& outSymbol)
{
    while (!symbols.empty()) {
        std::unordered_map<std::string, Symbol>* currentTable = symbols.top();
        auto searchedSymbol = currentTable->find(symbolName);

        if (searchedSymbol != currentTable->end()) {
            outSymbol = searchedSymbol->second;
            return true;
        }
        helperStack.push(currentTable);
        symbols.pop();
    }
    while (!helperStack.empty()) {
        symbols.push(helperStack.top());
        helperStack.pop();
    }
    return false;
}

ccc::SyntaxTree::SyntaxTree()
    : root(nullptr)
{
}

ccc::SyntaxTree::~SyntaxTree()
{
    delete root;
}

ccc::SyntaxTree::SyntaxTreeNode::SyntaxTreeNode(Token val)
    : val(val)
    , children(nullptr)
    , next(nullptr)
{
}

bool ccc::SyntaxTree::insert(const std::string& parentNonTerminal, const std::vector<Token>& tokens)
{
    if (parentNonTerminal.empty()) {
        root = new SyntaxTreeNode(tokens[0]);
        nonTerminalsToNodes[tokens[0].lexeme].push_back(root);
        return true;
    }
    // E->SEE
    // E->E
    auto parentIterator = nonTerminalsToNodes.find(parentNonTerminal);
    if (parentIterator == nonTerminalsToNodes.end())
        return false;
    SyntaxTreeNode* parent = parentIterator->second.front();
    bool sameNonTerminalEncountered = false;
    // insert node as a child of the last inserted grammarSymbol node
    for (const Token& t : tokens) {
        SyntaxTreeNode* children;
        if (t.lexeme == parentNonTerminal && !sameNonTerminalEncountered) {
            parentIterator->second.pop_front();
            sameNonTerminalEncountered = true;
        }
        if (parent->children == nullptr) {
            children = new SyntaxTreeNode(t);
            parent->children = children;
        } else {
            children = parent->children;
            while (children->next != nullptr)
                children = children->next;
            children->next = new SyntaxTreeNode(t);
            children = children->next;
        }
        // add the newly added node to the map
        if (t.term == Terminal::NON_TERMINAL) {
            if (parentIterator->second.empty())
                nonTerminalsToNodes[t.lexeme].push_back(children);
            else
                nonTerminalsToNodes[t.lexeme].push_front(children);
        }
    }
    if (!sameNonTerminalEncountered) {
        parentIterator->second.pop_front();
        if (parentIterator->second.empty())
            nonTerminalsToNodes.erase(parentIterator);
    }
    return true;
}

ccc::SyntaxTree::SyntaxTreeNode::~SyntaxTreeNode()
{
    delete next;
    delete children;
}

void ccc::SyntaxTree::printSyntaxTree()
{
    std::vector<std::vector<SyntaxTreeNode*>> levels;
    levelOrderTraversal(root, 0, levels);    
	for(auto& level:levels){
		for(auto node:level){
			std::cout<<node->val.lexeme;
            unsigned int numChildren=0;
            SyntaxTreeNode* child=node->children;
            while(child!=nullptr){
                ++numChildren;
                child=child->next;
            }
            std::cout<<"("<<numChildren<<") ";
        }
		std::cout<<std::endl;
	}
}

void ccc::SyntaxTree::levelOrderTraversal(SyntaxTreeNode* node, unsigned int level,std::vector<std::vector<SyntaxTreeNode*>>& levels)
{
	if(node==nullptr)
		return;
	if(levels.size()==level)
		levels.push_back(std::vector<ccc::SyntaxTree::SyntaxTreeNode*>());
	levels[level].push_back(node);
	SyntaxTreeNode* child=node->children;
	while(child){
		levelOrderTraversal(child, level+1, levels);
		child=child->next;
	}
}

ccc::Parser::Parser(SharedBuffer& buffer)
    : buffer{buffer}
{
    grammarSymbols.push("$");
    grammarSymbols.push("E");

    terminals.emplace("id");
    terminals.emplace("int");
    terminals.emplace("float");
    terminals.emplace("char");
    terminals.emplace("if");
    terminals.emplace("while");
    terminals.emplace("+");
    terminals.emplace("-");
    terminals.emplace("*");
    terminals.emplace("/");
    terminals.emplace("logop");
    terminals.emplace("=");
    terminals.emplace("int literal");
    terminals.emplace("float literal");
    terminals.emplace("string literal");
    terminals.emplace(";");
    terminals.emplace("{");
    terminals.emplace("}");
    terminals.emplace("(");
    terminals.emplace(")");
}

const std::unordered_map<ccc::Terminal, std::string> ccc::Parser::terminalsToProductions = {
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

const std::unordered_map<std::string, ccc::Terminal> ccc::Parser::productionsToTerminals = {
    { "id", Terminal::ID },
    { "int", Terminal::BUILTIN_TYPE_INT },
    { "float", Terminal::BUILTIN_TYPE_FLOAT },
    { "char", Terminal::BUILTIN_TYPE_CHAR },
    { "if", Terminal::CONTROL_FLOW_BRANCH },
    { "while", Terminal::CONTROL_FLOW_WHILE },
    { "+", Terminal::ARITHMETIC_OP_PLUS },
    { "-", Terminal::ARITHMETIC_OP_MINUS },
    { "*", Terminal::ARITHMETIC_OP_MULT },
    { "/", Terminal::ARITHMETIC_OP_DIV },
    { "logop", Terminal::LOGICAL_OP },
    { "=", Terminal::ASSIGNMENT_OP },
    { "int literal", Terminal::INT_LITERAL },
    { "float literal", Terminal::FLOAT_LITERAL },
    { "string literal", Terminal::STRING_LITERAL },
    { ";", Terminal::SEMICOLON },
    { "{", Terminal::OPEN_SCOPE },
    { "}", Terminal::CLOSED_SCOPE },
    { "(", Terminal::OPENING_BRACKET },
    { ")", Terminal::CLOSING_BRACKET },
    { "eof", Terminal::FILE_END }
};

ccc::LL1Parser::LL1Parser(SharedBuffer& buffer)
    : Parser{buffer}
{
    parsingTable.emplace("E", std::unordered_map<Terminal, std::vector<std::string>> {});
    parsingTable["E"].emplace(Terminal::ID, std::vector<std::string> { "E'", "S" });
    parsingTable["E"].emplace(Terminal::INT_LITERAL, std::vector<std::string> { "E'", "S" });
    parsingTable["E"].emplace(Terminal::FLOAT_LITERAL, std::vector<std::string> { "E'", "S" });
    parsingTable["E"].emplace(Terminal::OPENING_BRACKET, std::vector<std::string> { "E'", "S" });

    parsingTable.emplace("S", std::unordered_map<Terminal, std::vector<std::string>> {});
    parsingTable["S"].emplace(Terminal::ARITHMETIC_OP_PLUS, std::vector<std::string> { "+", "E'", "S" });
    parsingTable["S"].emplace(Terminal::ARITHMETIC_OP_MINUS, std::vector<std::string> { "-", "E'", "S" });
    parsingTable["S"].emplace(Terminal::FILE_END, std::vector<std::string> { "epsilon" });
    parsingTable["S"].emplace(Terminal::CLOSING_BRACKET, std::vector<std::string> { "epsilon" });

    parsingTable.emplace("E'", std::unordered_map<Terminal, std::vector<std::string>> {});
    parsingTable["E'"].emplace(Terminal::ID, std::vector<std::string> { "T", "F" });
    parsingTable["E'"].emplace(Terminal::INT_LITERAL, std::vector<std::string> { "T", "F" });
    parsingTable["E'"].emplace(Terminal::FLOAT_LITERAL, std::vector<std::string> { "T", "F" });
    parsingTable["E'"].emplace(Terminal::OPENING_BRACKET, std::vector<std::string> { "T", "F" });

    parsingTable.emplace("F", std::unordered_map<Terminal, std::vector<std::string>> {});
    parsingTable["F"].emplace(Terminal::ARITHMETIC_OP_MULT, std::vector<std::string> { "*", "T", "F" });
    parsingTable["F"].emplace(Terminal::ARITHMETIC_OP_DIV, std::vector<std::string> { "/", "T", "F" });
    parsingTable["F"].emplace(Terminal::ARITHMETIC_OP_PLUS, std::vector<std::string> { "epsilon" });
    parsingTable["F"].emplace(Terminal::ARITHMETIC_OP_MINUS, std::vector<std::string> { "epsilon" });
    parsingTable["F"].emplace(Terminal::FILE_END, std::vector<std::string> { "epsilon" });
    parsingTable["F"].emplace(Terminal::CLOSING_BRACKET, std::vector<std::string> { "epsilon" });

    parsingTable.emplace("T", std::unordered_map<Terminal, std::vector<std::string>> {});
    parsingTable["T"].emplace(Terminal::ID, std::vector<std::string> { "id" });
    parsingTable["T"].emplace(Terminal::INT_LITERAL, std::vector<std::string> { "int literal" });
    parsingTable["T"].emplace(Terminal::FLOAT_LITERAL, std::vector<std::string> { "float literal" });
    parsingTable["T"].emplace(Terminal::OPENING_BRACKET, std::vector<std::string> { "(", "E", ")" });
}

void ccc::Parser::addToSymbolTable(ccc::Token& token, ccc::Type type)
{
    if (token.term == Terminal::OPEN_SCOPE)
        symbolTable.addScope();
    else if (token.term == Terminal::CLOSED_SCOPE)
        symbolTable.removeScope();
    else if (token.term == Terminal::ID) {
        Symbol symbol { type, StorageSpecifier::AUTO };
        symbolTable.insert(token.lexeme, symbol);
    }
}

void ccc::LL1Parser::expandProduction(SyntaxTree& st)
{
    Token* inputToken = buffer.consume();
    std::string currentGrammarSymbol = grammarSymbols.top();
    std::vector<Token> productionTokens;

    grammarSymbols.pop();

    for (std::string& production : parsingTable[currentGrammarSymbol][inputToken->term])
        if (production != "epsilon") {
            auto productionTerm = productionsToTerminals.find(production);

            if (productionTerm != productionsToTerminals.end()) {
                if (productionTerm->second == inputToken->term)
                    productionTokens.push_back(*inputToken);
                else
                    productionTokens.emplace_back(production, productionTerm->second);
            } else
                productionTokens.emplace_back(production, Terminal::NON_TERMINAL);
        }

    for (auto production = productionTokens.rbegin(); production != productionTokens.rend(); ++production) {
        if (production->term == Terminal::NON_TERMINAL)
            grammarSymbols.push(production->lexeme);
        else
            grammarSymbols.push(terminalsToProductions.find(production->term)->second);
    }

    st.insert(currentGrammarSymbol, productionTokens);
}

void ccc::LL1Parser::continueGrammarMatching()
{
    grammarSymbols.pop();
    buffer.pop();
}

bool ccc::LL1Parser::parse(SyntaxTree& res)
{
    res.insert("", std::vector<Token> { Token("E", Terminal::NON_TERMINAL) });
    std::string currentGrammarSymbol = grammarSymbols.top();

    while (currentGrammarSymbol != std::string("$")) {
        Terminal inputTerm = buffer.consume()->term;
        // terminal
        if (terminalsToProductions.find(inputTerm) == terminalsToProductions.end())
            // error out
            return false;
        if (currentGrammarSymbol == terminalsToProductions.find(inputTerm)->second) {
            // if the currentGrammarSymbol is the same terminal as the input
            continueGrammarMatching();
        } else if (terminals.find(currentGrammarSymbol) != terminals.end()) {
            // error currentGrammarSymbol is a terminal but not the one in the
            // input which means the grammar did not match
            std::cout << "Error\n";
            return false;
            // non terminal
        } else if (parsingTable[currentGrammarSymbol].find(inputTerm) == parsingTable[currentGrammarSymbol].end()) {
            // error currentGrammarSymbol is a non terminal but there is no entry
            // in the parsing table for the input terminal and the current non
            // terminal and thusly the grammar did not match
            std::cout << "Error\n";
            return false;
        } else
            expandProduction(res);
        currentGrammarSymbol = grammarSymbols.top();
    }
    return true;
}

void ccc::LL1Parser::convertToAst(SyntaxTree& st)
{
    st.root = convertToAstHelper(st.root);
}

ccc::SyntaxTree::SyntaxTreeNode* ccc::LL1Parser::convertToAstHelper(SyntaxTree::SyntaxTreeNode* node)
{
    if (node == nullptr)
        return node;
    SyntaxTree::SyntaxTreeNode* next = convertToAstHelper(node->next);
    SyntaxTree::SyntaxTreeNode* children = convertToAstHelper(node->children);
    bool reorderChildren = false;

    if (next != nullptr) {
        if (children == nullptr)
            node->children = next;
        else {
            SyntaxTree::SyntaxTreeNode* temp = children;
            while (temp->next != nullptr)
                temp = temp->next;
            temp->next = next->children;
            next->children = children;
            node->children = next;
            reorderChildren = true;
        }
    }
    node->next = nullptr;

    if (children == nullptr && next == nullptr) {
        delete node->children;
        node->children = nullptr;
    }
    if (node->val.term == Terminal::NON_TERMINAL) {
        SyntaxTree::SyntaxTreeNode* temp = node->children;
        node->children = nullptr;
        delete node;
        if (reorderChildren)
            return temp;
        return children;
    }
    return node;
}
