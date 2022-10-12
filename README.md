# CCC - C subset Compiler in C++
Written for learning purposes using the Dragon Book as reference.
## Lexer
Uses finite state automata for each rule in the grammar outputting a token consisting of a terminal and a lexeme.
This is done in a multithreaded producer consumer model where the consumer is the parser.
