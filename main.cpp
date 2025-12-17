#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include "interpreter.h"

extern int yyparse();
extern FILE* yyin;
extern std::shared_ptr<ASTNode> parseResult;
extern std::shared_ptr<Environment> globalEnv;

// Declare yylex_destroy for cleanup
extern int yylex_destroy();

void runREPL() {
    std::cout << "mini-scheme REPL (type expressions and press Enter, Ctrl+D to exit)\n";

    globalEnv = std::make_shared<Environment>();
    initGlobalEnvironment(globalEnv);

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            std::cout << "\nBye!\n";
            break;
        }

        if (line.empty()) {
            continue;
        }

        // Write to temporary file for parsing
        FILE* tmp = fmemopen((void*)line.c_str(), line.size(), "r");
        if (!tmp) {
            std::cerr << "Error creating temporary buffer\n";
            continue;
        }

        yyin = tmp;
        parseResult = nullptr;

        if (yyparse() == 0 && parseResult) {
            try {
                // In REPL mode, parseResult is a list with one expression
                if (parseResult->type == NODE_LIST && !parseResult->children.empty()) {
                    auto result = eval(parseResult->children[0], globalEnv);
                    if (result && result->type == NODE_INTEGER) {
                        std::cout << result->value << std::endl;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }

        fclose(tmp);
    }
}

void runFile(const char* filename) {
    globalEnv = std::make_shared<Environment>();
    initGlobalEnvironment(globalEnv);

    FILE* file = fopen(filename, "r");
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        exit(1);
    }

    yyin = file;
    parseResult = nullptr;

    if (yyparse() != 0) {
        std::cerr << "Parse error\n";
        fclose(file);
        exit(1);
    }

    if (parseResult && parseResult->type == NODE_LIST) {
        // Evaluate each expression in the program
        for (auto& expr : parseResult->children) {
            try {
                eval(expr, globalEnv);
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                fclose(file);
                exit(1);
            }
        }
    }

    fclose(file);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        // REPL mode
        runREPL();
    } else if (argc == 2) {
        // File execution mode
        runFile(argv[1]);
    } else {
        std::cerr << "Usage: " << argv[0] << " [filename]\n";
        return 1;
    }

    return 0;
}
