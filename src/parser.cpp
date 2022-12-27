#include "parser.h"

#include <iostream>
#include <queue>
#include <unordered_map>

#include "include/parser.h"

ccc::SymbolTable::SymbolTable()
{
}

ccc::SymbolTable::~SymbolTable(){
	while(!symbols.empty()){
		delete symbols.top();
		symbols.pop();
	}
}

void ccc::SymbolTable::addScope()
{
    symbols.push(new std::unordered_map<std::string, Symbol>());
}

void ccc::SymbolTable::removeScope()
{
    symbols.pop();
}

bool ccc::SymbolTable::insert(std::string& symbolName, Symbol& symbol)
{
    std::unordered_map<std::string, Symbol>* currentTable = symbols.top();
    if (currentTable->find(symbolName) != currentTable->end())
        return false;
    currentTable->insert({ symbolName, symbol });
    return true;
}

bool ccc::SymbolTable::lookup(std::string& symbolName, Symbol& outSymbol)
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
    if (!parentNonTerminal.size()) {
        root = new SyntaxTreeNode(tokens[0]);
        nonTerminalsToNodes[tokens[0].lexeme].push(root);
        return true;
    }
    if (nonTerminalsToNodes.find(parentNonTerminal) == nonTerminalsToNodes.end())
        return false;
    SyntaxTreeNode* parent = nonTerminalsToNodes[parentNonTerminal].top();
    bool sameNonTerminalEncountered = false;
    // insert node as a child of the last inserted grammarSymbol node
    for (const Token& t : tokens) {
        SyntaxTreeNode* children;
        if (t.lexeme == parentNonTerminal) {
            nonTerminalsToNodes[parentNonTerminal].pop();
            sameNonTerminalEncountered = true;
        }
        if (parent->children == nullptr) {
            children = new SyntaxTreeNode(t);
            parent->children = children;
        } else {
            while (children->next != nullptr)
                children = children->next;
            children->next = new SyntaxTreeNode(t);
            children = children->next;
        }
        // add the newly added node to the map
        if (t.term == Terminal::NON_TERMINAL)
            nonTerminalsToNodes[t.lexeme].push(children);
    }
    if (!sameNonTerminalEncountered)
        nonTerminalsToNodes[parentNonTerminal].pop();
    return true;
}

ccc::SyntaxTree::SyntaxTreeNode::~SyntaxTreeNode()
{
    delete next;
    delete children;
}

ccc::Parser::Parser(std::queue<Token*>& sharedBuffer)
    : sharedBuffer(sharedBuffer)
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

ccc::LL1Parser::LL1Parser(std::queue<Token*>& sharedBuffer)
    : Parser(sharedBuffer)
{
    // is emplace better?
    parsingTable["E"] = std::unordered_map<Terminal, std::vector<std::string>>();
    parsingTable["E"][Terminal::ID] = std::vector<std::string> { "S", "E'" };
    parsingTable["E"][Terminal::INT_LITERAL] = std::vector<std::string> { "S", "E'" };
    parsingTable["E"][Terminal::FLOAT_LITERAL] = std::vector<std::string> { "S", "E'" };
    parsingTable["E"][Terminal::OPENING_BRACKET] = std::vector<std::string> { "S", "E'" };

    parsingTable["S"] = std::unordered_map<Terminal, std::vector<std::string>>();
    parsingTable["S"][Terminal::ARITHMETIC_OP_PLUS] = std::vector<std::string> { "S", "E'", "+" };
    parsingTable["S"][Terminal::ARITHMETIC_OP_MINUS] = std::vector<std::string> { "S", "E'", "-" };
    parsingTable["S"][Terminal::FILE_END] = std::vector<std::string> { "epsilon" };
    parsingTable["S"][Terminal::CLOSING_BRACKET] = std::vector<std::string> { "epsilon" };

    parsingTable["E'"] = std::unordered_map<Terminal, std::vector<std::string>>();
    parsingTable["E'"][Terminal::ID] = std::vector<std::string> { "F", "T" };
    parsingTable["E'"][Terminal::INT_LITERAL] = std::vector<std::string> { "F", "T" };
    parsingTable["E'"][Terminal::FLOAT_LITERAL] = std::vector<std::string> { "F", "T" };
    parsingTable["E'"][Terminal::OPENING_BRACKET] = std::vector<std::string> { "F", "T" };

    parsingTable["F"] = std::unordered_map<Terminal, std::vector<std::string>>();
    parsingTable["F"][Terminal::ARITHMETIC_OP_MULT] = std::vector<std::string> { "F", "T", "*" };
    parsingTable["F"][Terminal::ARITHMETIC_OP_DIV] = std::vector<std::string> { "F", "T", "/" };
    parsingTable["F"][Terminal::ARITHMETIC_OP_PLUS] = std::vector<std::string> { "epsilon" };
    parsingTable["F"][Terminal::ARITHMETIC_OP_MINUS] = std::vector<std::string> { "epsilon" };
    parsingTable["F"][Terminal::FILE_END] = std::vector<std::string> { "epsilon" };
    parsingTable["F"][Terminal::CLOSING_BRACKET] = std::vector<std::string> { "epsilon" };

    parsingTable["T"] = std::unordered_map<Terminal, std::vector<std::string>>();
    parsingTable["T"][Terminal::ID] = std::vector<std::string> { "id" };
    parsingTable["T"][Terminal::INT_LITERAL] = std::vector<std::string> { "int literal" };
    parsingTable["T"][Terminal::FLOAT_LITERAL] = std::vector<std::string> { "float literal" };
    parsingTable["T"][Terminal::OPENING_BRACKET] = std::vector<std::string> { ")", "E", "(" };
}

/* void ccc::Parser::addToSymbolTable(Token& token){ */
/* 	if(token.term==Terminal::OPEN_SCOPE) */
/* 		symbolTable.addScope(); */
/* 	else if(token.term==Terminal::CLOSED_SCOPE) */
/* 		symbolTables.closeScope(); */
/* 	else if(token.term==Terminal::ID) */
/* 		symbolTable.insert(, new Symbol(, )); */
/* } */

void ccc::LL1Parser::expandProduction(SyntaxTree& st)
{
    Terminal inputTerm = sharedBuffer.front()->term;
    std::string currentGrammarSymbol = grammarSymbols.top();
    std::vector<Token> productionTokens;

    grammarSymbols.pop();

    for (std::string& production : parsingTable[currentGrammarSymbol][inputTerm])
        if (production != "epsilon") {
            grammarSymbols.push(production);
            productionTokens.emplace_back(Token(production, Terminal::NON_TERMINAL));
        }
    st.insert(currentGrammarSymbol, productionTokens);
}

void ccc::LL1Parser::continueGrammarMatching()
{
    grammarSymbols.pop();
    sharedBuffer.pop();
}

bool ccc::LL1Parser::parsingAlgorithm(SyntaxTree& res)
{
    res.insert("", std::vector<Token> { Token("E", Terminal::NON_TERMINAL) });
    std::string currentGrammarSymbol = grammarSymbols.top();

    while (currentGrammarSymbol != std::string("$")) {
        Terminal inputTerm = sharedBuffer.front()->term;
        // terminal
        if (currentGrammarSymbol == Token::terminalNames[inputTerm]) {
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
