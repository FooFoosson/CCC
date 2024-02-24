# CCC - C subset Compiler in C++
Written for learning purposes using the Dragon Book as reference.
## Lexer
Uses finite state automata for each rule in the grammar to produce tokens consisting of a terminal and a lexeme.
Outputs a buffer of tokens encapsulated in a semaphore protected producer consumer model class.
## Parser
Takes the buffer and using an LL(1) parsing method outputs a syntax tree.
It then converts the tree to an abstract syntax tree.
## Interpreter
Evaluates the AST using a stack-based VM.
## Main
Instantiates all three parts while running the lexer and the parser in parallel.
