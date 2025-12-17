#include "interpreter.h"
#include <iostream>
#include <stdexcept>

// Helper function to create an integer node
std::shared_ptr<ASTNode> makeInteger(int value) {
    auto node = std::make_shared<ASTNode>(NODE_INTEGER);
    node->value = value;
    return node;
}

// Evaluate arithmetic and comparison operations
std::shared_ptr<ASTNode> evalOperator(const std::string& op,
                                      const std::vector<std::shared_ptr<ASTNode>>& args,
                                      std::shared_ptr<Environment> env) {
    // Evaluate all arguments (eager evaluation)
    std::vector<int> values;
    for (size_t i = 0; i < args.size(); i++) {
        auto result = eval(args[i], env);
        if (result->type != NODE_INTEGER) {
            throw std::runtime_error("Operator arguments must evaluate to integers");
        }
        values.push_back(result->value);
    }

    if (op == "+") {
        int sum = 0;
        for (int v : values) sum += v;
        return makeInteger(sum);
    } else if (op == "-") {
        if (values.size() == 0) return makeInteger(0);
        if (values.size() == 1) return makeInteger(-values[0]);
        int result = values[0];
        for (size_t i = 1; i < values.size(); i++) result -= values[i];
        return makeInteger(result);
    } else if (op == "*") {
        int product = 1;
        for (int v : values) product *= v;
        return makeInteger(product);
    } else if (op == "/") {
        if (values.size() < 2) throw std::runtime_error("Division requires at least 2 arguments");
        int result = values[0];
        for (size_t i = 1; i < values.size(); i++) {
            if (values[i] == 0) throw std::runtime_error("Division by zero");
            result /= values[i];
        }
        return makeInteger(result);
    } else if (op == "%") {
        if (values.size() != 2) throw std::runtime_error("Modulo requires exactly 2 arguments");
        if (values[1] == 0) throw std::runtime_error("Modulo by zero");
        return makeInteger(values[0] % values[1]);
    } else if (op == "=") {
        if (values.size() < 2) return makeInteger(1);
        for (size_t i = 1; i < values.size(); i++) {
            if (values[i] != values[0]) return makeInteger(0);
        }
        return makeInteger(1);
    } else if (op == "<") {
        if (values.size() < 2) return makeInteger(1);
        for (size_t i = 1; i < values.size(); i++) {
            if (values[i-1] >= values[i]) return makeInteger(0);
        }
        return makeInteger(1);
    } else if (op == ">") {
        if (values.size() < 2) return makeInteger(1);
        for (size_t i = 1; i < values.size(); i++) {
            if (values[i-1] <= values[i]) return makeInteger(0);
        }
        return makeInteger(1);
    }

    throw std::runtime_error("Unknown operator: " + op);
}

// Main evaluation function
std::shared_ptr<ASTNode> eval(std::shared_ptr<ASTNode> node, std::shared_ptr<Environment> env) {
    if (!node) {
        throw std::runtime_error("Null node in eval");
    }

    switch (node->type) {
        case NODE_INTEGER:
            return node;

        case NODE_SYMBOL: {
            auto value = env->lookup(node->symbol);
            if (!value) {
                throw std::runtime_error("Undefined symbol: " + node->symbol);
            }
            return value;
        }

        case NODE_LAMBDA:
            return node;

        case NODE_LIST: {
            if (node->children.empty()) {
                throw std::runtime_error("Empty list cannot be evaluated");
            }

            auto first = node->children[0];

            // Handle special forms
            if (first->type == NODE_SYMBOL) {
                const std::string& sym = first->symbol;

                // (define (name params...) body) or (define name value)
                if (sym == "define") {
                    if (node->children.size() < 3) {
                        throw std::runtime_error("define requires at least 2 arguments");
                    }

                    // Function definition: (define (name params...) body)
                    if (node->children[1]->type == NODE_LIST) {
                        auto defList = node->children[1];
                        if (defList->children.empty() || defList->children[0]->type != NODE_SYMBOL) {
                            throw std::runtime_error("Invalid function definition");
                        }

                        std::string funcName = defList->children[0]->symbol;
                        std::vector<std::string> params;
                        for (size_t i = 1; i < defList->children.size(); i++) {
                            if (defList->children[i]->type != NODE_SYMBOL) {
                                throw std::runtime_error("Function parameters must be symbols");
                            }
                            params.push_back(defList->children[i]->symbol);
                        }

                        auto lambdaNode = std::make_shared<ASTNode>(NODE_LAMBDA);
                        lambdaNode->lambda.params = params;
                        lambdaNode->lambda.body = node->children[2];

                        env->define(funcName, lambdaNode);
                        return makeInteger(1);
                    }
                    // Variable definition: (define name value)
                    else if (node->children[1]->type == NODE_SYMBOL) {
                        std::string varName = node->children[1]->symbol;
                        auto value = eval(node->children[2], env);
                        env->define(varName, value);
                        return makeInteger(1);
                    } else {
                        throw std::runtime_error("Invalid define syntax");
                    }
                }

                // (if condition then-expr else-expr)
                if (sym == "if") {
                    if (node->children.size() != 4) {
                        throw std::runtime_error("if requires exactly 3 arguments");
                    }
                    auto condition = eval(node->children[1], env);
                    if (condition->type != NODE_INTEGER) {
                        throw std::runtime_error("if condition must evaluate to integer");
                    }
                    if (condition->value != 0) {
                        return eval(node->children[2], env);
                    } else {
                        return eval(node->children[3], env);
                    }
                }

                // (and expr1 expr2 ...)
                if (sym == "and") {
                    for (size_t i = 1; i < node->children.size(); i++) {
                        auto result = eval(node->children[i], env);
                        if (result->type != NODE_INTEGER) {
                            throw std::runtime_error("and arguments must evaluate to integers");
                        }
                        if (result->value == 0) {
                            return makeInteger(0);
                        }
                    }
                    return makeInteger(1);
                }

                // (or expr1 expr2 ...)
                if (sym == "or") {
                    for (size_t i = 1; i < node->children.size(); i++) {
                        auto result = eval(node->children[i], env);
                        if (result->type != NODE_INTEGER) {
                            throw std::runtime_error("or arguments must evaluate to integers");
                        }
                        if (result->value != 0) {
                            return makeInteger(1);
                        }
                    }
                    return makeInteger(0);
                }

                // (print expr1 expr2 ...)
                if (sym == "print") {
                    for (size_t i = 1; i < node->children.size(); i++) {
                        auto result = eval(node->children[i], env);
                        if (result->type != NODE_INTEGER) {
                            throw std::runtime_error("print arguments must evaluate to integers");
                        }
                        std::cout << result->value;
                        if (i < node->children.size() - 1) {
                            std::cout << " ";
                        }
                    }
                    std::cout << std::endl;
                    return makeInteger(1);
                }

                // Check if it's an operator
                if (sym == "+" || sym == "-" || sym == "*" || sym == "/" ||
                    sym == "%" || sym == "=" || sym == "<" || sym == ">") {
                    std::vector<std::shared_ptr<ASTNode>> args(node->children.begin() + 1, node->children.end());
                    return evalOperator(sym, args, env);
                }
            }

            // Function application
            auto func = eval(first, env);
            if (func->type != NODE_LAMBDA) {
                throw std::runtime_error("Cannot apply non-function");
            }

            // Create new environment for function execution
            auto funcEnv = std::make_shared<Environment>(env);

            // Bind parameters
            if (node->children.size() - 1 != func->lambda.params.size()) {
                throw std::runtime_error("Function called with wrong number of arguments");
            }

            for (size_t i = 0; i < func->lambda.params.size(); i++) {
                auto argValue = eval(node->children[i + 1], env);
                funcEnv->define(func->lambda.params[i], argValue);
            }

            // Evaluate body
            return eval(func->lambda.body, funcEnv);
        }
    }

    throw std::runtime_error("Unknown node type");
}

void initGlobalEnvironment(std::shared_ptr<Environment> env) {
    // Nothing to initialize for now - all primitives are handled in eval
}
