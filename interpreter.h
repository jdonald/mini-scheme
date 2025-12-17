#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

// AST node types
enum NodeType {
    NODE_INTEGER,
    NODE_SYMBOL,
    NODE_LIST,
    NODE_LAMBDA
};

// Forward declaration
struct ASTNode;

// Lambda/function definition
struct Lambda {
    std::vector<std::string> params;
    std::shared_ptr<ASTNode> body;
};

// AST Node structure
struct ASTNode {
    NodeType type;
    int value;                                    // for NODE_INTEGER
    std::string symbol;                           // for NODE_SYMBOL
    std::vector<std::shared_ptr<ASTNode>> children; // for NODE_LIST
    Lambda lambda;                                // for NODE_LAMBDA

    ASTNode(NodeType t) : type(t), value(0) {}
};

// Environment for variable/function bindings
class Environment {
public:
    std::map<std::string, std::shared_ptr<ASTNode>> bindings;
    std::shared_ptr<Environment> parent;

    Environment(std::shared_ptr<Environment> p = nullptr) : parent(p) {}

    void define(const std::string& name, std::shared_ptr<ASTNode> value) {
        bindings[name] = value;
    }

    std::shared_ptr<ASTNode> lookup(const std::string& name) {
        if (bindings.find(name) != bindings.end()) {
            return bindings[name];
        }
        if (parent) {
            return parent->lookup(name);
        }
        return nullptr;
    }
};

// Interpreter functions
std::shared_ptr<ASTNode> eval(std::shared_ptr<ASTNode> node, std::shared_ptr<Environment> env);
void initGlobalEnvironment(std::shared_ptr<Environment> env);

// Parser interface
extern std::shared_ptr<ASTNode> parseResult;
extern std::shared_ptr<Environment> globalEnv;

#endif
