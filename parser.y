%{
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <memory>
#include "interpreter.h"

extern int yylex();
extern int yyparse();
extern FILE* yyin;
void yyerror(const char* s);

std::shared_ptr<ASTNode> parseResult;
std::shared_ptr<Environment> globalEnv;

%}

%union {
    int num;
    std::string* str;
    ASTNode* node;
}

%token <num> INTEGER
%token <str> SYMBOL
%token LPAREN RPAREN

%type <node> program expr expr_list program_list

%%

program:
    program_list        { parseResult = std::shared_ptr<ASTNode>($1); }
    ;

program_list:
    expr                {
        auto node = new ASTNode(NODE_LIST);
        node->children.push_back(std::shared_ptr<ASTNode>($1));
        $$ = node;
    }
    | program_list expr {
        $1->children.push_back(std::shared_ptr<ASTNode>($2));
        $$ = $1;
    }
    ;

expr:
    INTEGER             {
        auto node = new ASTNode(NODE_INTEGER);
        node->value = $1;
        $$ = node;
    }
    | SYMBOL            {
        auto node = new ASTNode(NODE_SYMBOL);
        node->symbol = *$1;
        delete $1;
        $$ = node;
    }
    | LPAREN RPAREN     {
        auto node = new ASTNode(NODE_LIST);
        $$ = node;
    }
    | LPAREN expr_list RPAREN {
        $$ = $2;
    }
    ;

expr_list:
    expr                {
        auto node = new ASTNode(NODE_LIST);
        node->children.push_back(std::shared_ptr<ASTNode>($1));
        $$ = node;
    }
    | expr_list expr    {
        $1->children.push_back(std::shared_ptr<ASTNode>($2));
        $$ = $1;
    }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Parse error: %s\n", s);
}
