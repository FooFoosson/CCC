#include "lexer.h"
#include "parser.h"
#include "utility.h"
#include "vm.h"
#include <thread>

int main(int argc, char** argv)
{
    ccc::SharedBuffer buffer;
    ccc::Lexer lexer;

    for (int i = 1; i < argc; ++i) {
        std::thread lexer_thread { &ccc::Lexer::run, &lexer, argv[i], std::ref(buffer) };

        ccc::LL1Parser parser { buffer };
        ccc::SyntaxTree ast;
        std::thread parser_thread { &ccc::LL1Parser::parse, &parser, std::ref(ast) };

        lexer_thread.join();
        parser_thread.join();
        parser.convertToAst(ast);

        ccc::StackBasedVM vm { ast };
        vm.run();
    }
    return 0;
}
