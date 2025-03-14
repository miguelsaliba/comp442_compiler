#pragma once

#include "lexer.h"
#include "ast.h"
#include <initializer_list>
#include <ostream>
#include <utility>
#include <vector>

class Parser {
public:
    Parser(Lexer lexer, std::ostream &derivations, std::ostream &syntax_errors, std::ostream &ast_output)
        : lexer(std::move(lexer)), derivations(derivations),
          syntax_errors(syntax_errors), ast_output(ast_output) {};
    void parse();


private:
    Lexer lexer;
    Token curtok;
    Token nexttok;
    std::vector<std::string> error_stack;
    std::vector<std::string> derivation;
    int derivation_index = 0;
    std::ostream& derivations;
    std::ostream& syntax_errors;
    std::ostream& ast_output;

    std::unordered_set<std::string> statement_starters = {"if", "while", "read", "write", "return", "self"};

    bool token_in_values(const std::unordered_set<std::string>& values) const;
    bool token_in(const std::unordered_set<TokenType>& types) const;
    void nextsym();
    void error(const std::string &expected);
    void print_derivation();
    void insert_derivation(std::initializer_list<std::string> new_derivation);
    void accept_token(std::string value);
    void accept_epsilon();
    bool peek(TokenType type) const;
    bool match(TokenType type);
    bool expect(TokenType type);
    bool skipErrors(const std::unordered_set<TokenType>& first, const std::unordered_set<TokenType>& follow);

    bool program(AST* p);
    bool block(AST* p);
    bool classdef(AST* c);
    bool implementation(AST* i);
    bool funcdef(AST *fdef);
    bool funchead(AST *f);
    bool fparams(AST *fp);
    bool funcbody(AST *body);
    bool identifier(AST* id);
    bool localvardeclorstat(AST *declorstat);
    bool type(AST* t);
    bool returntype(AST* t);
    bool vardecl(AST *decl);
    bool arraysize(AST *size);
    bool arraysizetail(AST *size);
    bool addop(AST *a);
    bool idorself(AST *i);
    bool visibility(AST* v);
    bool reptfparams(AST *fp);
    bool attributedecl(AST *attr);
    bool localvardecl(AST *loc);
    bool statement(AST *s);
    bool relop(AST *r);
    bool multop(AST *m);
    bool relexpr(AST *rel);
    bool arithexpr(AST **a);
    bool term(AST **t);
    bool rightrecterm(AST *left, AST **right);
    bool sign(AST *s);
    bool factor(AST **f);
    bool expr(AST *e);
    bool exprtail(AST *left, AST **right);
    bool statblock(AST *sb);
    bool variable(AST *v);
    bool idnesttail(AST *left, AST **right);
    bool indice(AST *i);
    bool varidnest(AST *left, AST **result);
    bool varidnesttail(AST *left, AST **result);
    bool aparams(AST *params);
    bool reptaparams(AST *params);
    bool rightrecarithexpr(AST *left, AST **right);
    bool idnest(AST *left, AST **right);
    bool reptidnest(AST *left, AST **right);
    bool reptfuncbody(AST *body);
    bool isa(AST *i);
    bool reptisa(AST* id);
    bool vismemberdecl(AST* members);
    bool memdecl(AST *mem);
    bool implbody(AST *body);
    bool funcallorassign(AST *f);
    bool funcallorassign2(AST *left, AST **right);
    bool funcallorassign3(AST *left, AST **right);
    bool funcallorassign4(AST *l, AST **r);
    bool indices(AST *i);
    bool statements(AST *stmts);
    bool factor2(AST *left, AST **right);
    bool variable2(AST *left, AST **var);
    bool reptvariable(AST *left, AST **var);
    bool arraysizes(AST *as);
    bool aparamstail(AST *e);
    bool intlit(ASTIntLit* i);
    bool floatlit(ASTFloatLit *f);

    bool isRelop() const;
    bool isStatement() const;
    bool isFactor() const;
    bool isAddop() const;
    bool isMultop() const;
};
