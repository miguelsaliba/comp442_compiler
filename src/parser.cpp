#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

#include "parser.h"

#include <cassert>
#include <utility>
#include "lexer.h"
#include "ast.h"

using enum TokenType;

typedef std::unordered_set<TokenType> tokentype_set;

/*
START -> PROG
PROG -> CLASSIMPLFUNC PROG | EPSILON
CLASSIMPLFUNC -> CLASSDECL | IMPLDEF | FUNCDEF
CLASSDECL -> class id ISA lbrace VISMEMBERDECL rbrace semicolon
VISMEMBERDECL -> VISIBILITY MEMDECL VISMEMBERDECL | EPSILON
ISA -> isa id REPTISA | EPSILON
REPTISA -> comma id REPTISA | EPSILON
IMPLDEF -> implementation id lbrace IMPLBODY rbrace
IMPLBODY -> FUNCDEF IMPLBODY | EPSILON
FUNCDEF -> FUNCHEAD FUNCBODY
VISIBILITY -> public | private
MEMDECL -> FUNCDECL | ATTRDECL
FUNCDECL -> FUNCHEAD semicolon
FUNCHEAD -> function id lparen FPARAMS rparen arrow RETURNTYPE
| constructor lparen FPARAMS rparen
FUNCBODY -> lbrace REPTFUNCBODY rbrace
LOCALVARDECLORSTAT -> LOCALVARDECL | STATEMENT
REPTFUNCBODY -> LOCALVARDECLORSTAT REPTFUNCBODY | EPSILON
ATTRDECL -> attribute VARDECL
LOCALVARDECL -> local VARDECL
VARDECL -> id colon TYPE ARRAYSIZES semicolon
STATEMENT -> FUNCALLORASSIGN semicolon
| if lparen RELEXPR rparen then STATBLOCK else STATBLOCK semicolon
| while lparen RELEXPR rparen STATBLOCK semicolon
| read lparen VARIABLE rparen semicolon
| write lparen EXPR rparen semicolon
| return lparen EXPR rparen semicolon
FUNCALLORASSIGN -> IDORSELF FUNCALLORASSIGN2
FUNCALLORASSIGN2 -> INDICES FUNCALLORASSIGN3 | lparen APARAMS rparen FUNCALLORASSIGN4
FUNCALLORASSIGN3 -> ASSIGNOP EXPR | dot id FUNCALLORASSIGN2
FUNCALLORASSIGN4 -> dot id FUNCALLORASSIGN2 | EPSILON
STATBLOCK -> lbrace STATEMENTS rbrace | STATEMENT | EPSILON
STATEMENTS -> STATEMENT STATEMENTS | EPSILON
EXPR -> ARITHEXPR EXPR2
EXPR2 -> RELOP ARITHEXPR | EPSILON
RELEXPR -> ARITHEXPR RELOP ARITHEXPR
ARITHEXPR -> TERM RIGHTRECARITHEXPR
RIGHTRECARITHEXPR -> ADDOP TERM RIGHTRECARITHEXPR | EPSILON
SIGN -> plus | minus
TERM -> FACTOR RIGHTRECTERM
RIGHTRECTERM -> MULTOP FACTOR RIGHTRECTERM | EPSILON
FACTOR -> IDORSELF FACTOR2 REPTIDNEST
| floatlit
| intlit
| lparen ARITHEXPR rparen
| not FACTOR
| SIGN FACTOR
FACTOR2 -> lparen APARAMS rparen | INDICES
INDICES -> INDICE INDICES | EPSILON
REPTIDNEST -> IDNEST REPTIDNEST | EPSILON
VARIABLE -> IDORSELF VARIABLE2
VARIABLE2 -> INDICES REPTVARIABLE | lparen APARAMS rparen VARIDNEST
REPTVARIABLE -> VARIDNEST REPTVARIABLE | EPSILON
VARIDNEST -> dot id VARIDNESTTAIL
VARIDNESTTAIL -> lparen APARAMS rparen VARIDNEST | INDICES
INDICE -> lsqbr ARITHEXPR rsqbr
IDNEST -> dot id IDNESTTAIL
IDNESTTAIL -> lparen APARAMS rparen | INDICES
ARRAYSIZE -> lsqbr ARRAYSIZE2
ARRAYSIZE2 -> intlit rsqbr | rsqbr
ARRAYSIZES -> ARRAYSIZE ARRAYSIZES | EPSILON
TYPE -> int | float | id
RETURNTYPE -> TYPE | void
APARAMS -> EXPR REPTAPARAMS1 | EPSILON
REPTAPARAMS1 -> APARAMSTAIL REPTAPARAMS1 | EPSILON
APARAMSTAIL -> comma EXPR
FPARAMS -> id colon TYPE ARRAYSIZES REPTFPARAMS1 | EPSILON
REPTFPARAMS1 -> FPARAMSTAIL REPTFPARAMS1 | EPSILON
FPARAMSTAIL -> comma id colon TYPE ARRAYSIZES
ASSIGNOP -> assign
RELOP -> eq | neq | lt | gt | lteq | gteq
ADDOP -> plus | minus | or
MULTOP -> mult | div | and
IDORSELF -> id | self

 */


void Parser::error(const std::string &expected) {
    syntax_errors << "Line " << nexttok.line << ": ";
    syntax_errors << "Syntax error: Expected " << expected << " but found " << nexttok.value << std::endl;
    syntax_errors << std::endl;
}

void Parser::print_derivation() {
    derivations << nexttok.line << ": ";
    for (const std::string &value: derivation) {
        derivations << value << ' ';
    }
    derivations << std::endl;
}

void Parser::nextsym() {
    curtok = nexttok;
    nexttok = lexer.nextToken();
}


void Parser::insert_derivation(std::initializer_list<std::string> new_derivation) {
    const auto index = derivation.begin() + derivation_index;
    if (!derivation.empty() && derivation_index < derivation.size()) {
        derivation.erase(index);
    }
    derivation.insert(index, new_derivation);
    print_derivation();
}

void Parser::accept_token(std::string value) {
    if (derivation_index >= derivation.size()) {
        derivation.emplace_back(std::move(value));
    } else {
        derivation[derivation_index] = std::move(value);
    }
    derivation_index++;
    print_derivation();
}

void Parser::accept_epsilon() {
    if (!derivation.empty()) {
        derivation.erase(derivation.begin() + derivation_index);
    }
}

bool Parser::token_in_values(const std::unordered_set<std::string> &values) const {
    return values.contains(nexttok.value);
}

bool Parser::token_in(const std::unordered_set<TokenType> &types) const {
    return types.contains(nexttok.type);
}

bool Parser::peek(TokenType type) const {
    return nexttok.type == type;
}

bool Parser::match(TokenType type) {
    if (peek(type)) {
        accept_token(nexttok.value);
        nextsym();
        return true;
    }
    return false;
}

bool Parser::expect(TokenType type) {
    if (match(type)) {
        return true;
    }
    error(find_builtin(type));

    accept_epsilon();
    nextsym();
    return false;
}

bool Parser::skipErrors(const std::unordered_set<TokenType> &first, const std::unordered_set<TokenType> &follow = {}) {
    if (nexttok.type == EOF_TOKEN) {
        return false;
    }

    if (token_in(first) || (first.contains(EPSILON) && token_in(follow))) {
        return true;
    }

    // Print error once then keep looping
    syntax_errors << "Line " << nexttok.line << ": ";
    if (first.size() == 1) {
        syntax_errors << "Expected " << find_builtin(*first.begin()) << ' ';
    } else {
        syntax_errors << "Expected one of: (";
        for (TokenType value: first) {
            syntax_errors << find_builtin(value) << ", ";
        }
        syntax_errors << ") ";
    }
    syntax_errors << "but found " << nexttok.value << std::endl;

    while (!(token_in(first) || token_in(follow))) {
        nextsym();
        if (nexttok.type == EOF_TOKEN) {
            return false;
        }
        if (first.contains(EPSILON) && token_in(follow)) {
            accept_epsilon();
            return false;
        }
    }
    return true;
}

AST *Parser::parse() {
    syntax_errors << "Syntax errors:" << std::endl;
    nextsym();
    insert_derivation({"START"});
    insert_derivation({"PROGRAM"});

    auto p = new AST(ASTType::PROGRAM, nexttok.line);
    has_error = program(p);

    accept_epsilon();
    print_derivation();

    p->recPrint(ast_output);

    return p;
}


bool Parser::program(AST *p) {
    if (!skipErrors({CLASS, IMPLEMENTATION, FUNCTION, EPSILON}, {EOF_TOKEN})) return false;

    if (peek(CLASS) || peek(IMPLEMENTATION) || peek(FUNCTION)) {
        insert_derivation({"CLASSIMPLFUNC", "PROGRAM"});
        return block(p) & program(p);
    } else if (peek(EOF_TOKEN)) {
        accept_epsilon();
        return true;
    } else {
        return false;
    }
}

bool Parser::block(AST *p) {
    if (!skipErrors({CLASS, IMPLEMENTATION, FUNCTION, EPSILON}, {EOF_TOKEN})) return false;

    if (peek(CLASS)) {
        insert_derivation({"CLASS"});
        auto c = new AST(ASTType::CLASSDEF, nexttok.line);
        p->adopt(c);
        return classdef(c);
    } else if (peek(IMPLEMENTATION)) {
        insert_derivation({"IMPLEMENTATION"});
        auto i = new AST(ASTType::IMPLDEF, nexttok.line);
        p->adopt(i);
        return implementation(i);
    } else if (peek(FUNCTION) || peek(CONSTRUCTOR)) {
        insert_derivation({"FUNCDEF"});
        auto f = new AST(ASTType::FUNCDEF, nexttok.line);
        p->adopt(f);
        return funcdef(f);
    } else if (peek(EOF_TOKEN)) {
        accept_epsilon();
        return true;
    } else {
        error("class, implementation, function, or constructor");
        return false;
    }
}

bool Parser::classdef(AST *c) {
    insert_derivation({"class", "id", "ISA", "{", "VISMEMBERDECL", "}", ";"});
    auto i = new AST(ASTType::ISA, nexttok.line);
    auto id = new AST(ASTType::ID, nexttok.line);
    auto m = new AST(ASTType::MEMBERS, nexttok.line);

    if (expect(CLASS) & identifier(id) & isa(i) & expect(LBRACE) & vismemberdecl(m) &
        expect(RBRACE) & expect(SEMICOLON)) {
        c->adopt({id, i, m});
        return true;
    } else {
        delete i;
        delete id;
        delete m;
        return false;
    }
}

bool Parser::vismemberdecl(AST *members) {
    if (!skipErrors({PUBLIC, PRIVATE, EPSILON}, {RBRACE})) return false;

    if (peek(PUBLIC) || peek(PRIVATE)) {
        insert_derivation({"VISIBILITY", "MEMBERDECL", "VISMEMBERDECL"});
        auto vismem = new AST(ASTType::CLASSMEM, nexttok.line);
        auto v = new AST(ASTType::VISIBILITY, nexttok.line);
        auto mem = new AST(nexttok.line);
        vismem->adopt({v, mem});
        members->adopt(vismem);
        if (visibility(v) & memdecl(mem) & vismemberdecl(members)) {
            return true;
        }
        delete vismem;
        delete v;
        delete mem;
        return false;
    } else if (peek(RBRACE)) {
        accept_epsilon();
        return true;
    } else {
        accept_epsilon();
        return false;
    }
}

bool Parser::memdecl(AST *mem) {
    if (peek(FUNCTION) || peek(CONSTRUCTOR)) {
        insert_derivation({"FUNCHEAD", ";"});
        if (funchead(mem) & expect(SEMICOLON)) {
            return true;
        }
        return false;
    }

    if (peek(ATTRIBUTE)) {
        insert_derivation({"ATTRIBUTE"});
        mem->type = ASTType::VARDECL;
        if (attributedecl(mem)) {
            return true;
        }
        return false;
    }

    return false;
}

bool Parser::isa(AST *i) {
    if (!skipErrors({ISA, EPSILON}, {LBRACE})) return false;

    if (peek(ISA)) {
        insert_derivation({"isa", "id", "REPTISA"});
        auto id = new AST(ASTType::ID, nexttok.line);
        if (expect(ISA) & identifier(id) & reptisa(id)) {
            i->adopt(id);
            return true;
        }
        return false;
    }

    if (peek(LBRACE)) {
        accept_epsilon();
        return true;
    }

    return false;
}

bool Parser::reptisa(AST *id) {
    if (!skipErrors({COMMA, EPSILON}, {LBRACE})) return false;

    if (peek(COMMA)) {
        insert_derivation({",", "id", "REPTISA"});
        auto id2 = new AST(ASTType::ID, nexttok.line);
        if (expect(COMMA) & identifier(id2) & reptisa(id2)) {
            id->addSibling(id2);
            return true;
        }
        return false;
    } else if (peek(LBRACE)) {
        accept_epsilon();
        return true;
    } else {
        return false;
    }
}

bool Parser::implementation(AST *i) {
    insert_derivation({"implementation", "id", "{", "IMPLBODY", "}"});
    auto id = new AST(ASTType::ID, nexttok.line);
    auto body = new AST(ASTType::IMPLBODY, nexttok.line);

    if (expect(IMPLEMENTATION) & identifier(id) & expect(LBRACE) & implbody(body) &
        expect(RBRACE)) {
        i->adopt({id, body});
        return true;
    } else {
        return false;
    }
}

bool Parser::implbody(AST *body) {
    if (!skipErrors({FUNCTION, CONSTRUCTOR, EPSILON}, {RBRACE})) return false;

    if (token_in({FUNCTION, CONSTRUCTOR})) {
        insert_derivation({"FUNCDEF", "IMPLBODY"});
        auto fdef = new AST(ASTType::FUNCDEF, nexttok.line);

        body->adopt(fdef);
        if (funcdef(fdef) & implbody(body)) {
            return true;
        }
        return false;
    } else if (peek(RBRACE)) {
        accept_epsilon();
        return true;
    } else {
        return false;
    }
}

bool Parser::funcdef(AST *fdef) {
    if (peek(FUNCTION) || peek(CONSTRUCTOR)) {
        insert_derivation({"FUNCHEAD", "FUNCBODY"});
        auto head = new AST(nexttok.line);
        auto body = new AST(ASTType::FUNCBODY, nexttok.line);

        if (funchead(head) & funcbody(body)) {
            fdef->adopt({head, body});
            return true;
        }
        delete head;
        delete body;
        return false;
    } else {
        error("function or constructor");
        return false;
    }
}

bool Parser::visibility(AST *v) {
    if (match(PUBLIC) || match(PRIVATE)) {
        v->str_value = curtok.value;
        return true;
    }
    error("visibility");
    return false;
}

bool Parser::funchead(AST *f) {
    if (peek(FUNCTION)) {
        insert_derivation({"function", "id", "(", "FPARAMS", ")", "=>", "RETURNTYPE"});
        f->type = ASTType::FUNCHEAD;
        auto id = new AST(ASTType::ID, nexttok.line);
        auto params = new AST(ASTType::FPARAMS, nexttok.line);
        auto rtype = new AST(ASTType::TYPE, nexttok.line);

        if (expect(FUNCTION) & identifier(id) & expect(LPAREN) & fparams(params) &
            expect(RPAREN) & expect(ARROW) & returntype(rtype)) {
            f->adopt({id, params, rtype});
            return true;
        }
        delete id;
        delete params;
        delete rtype;
        return false;
    } else if (peek(CONSTRUCTOR)) {
        insert_derivation({"constructor", "(", "FPARAMS", ")"});
        f->type = ASTType::CONSTRUCTOR;
        auto params = new AST(ASTType::FPARAMS, nexttok.line);

        if (expect(CONSTRUCTOR) & expect(LPAREN) & fparams(params) & expect(RPAREN)) {
            f->adopt(params);
            return true;
        }
        return false;
    } else {
        error("function or constructor");
        return false;
    }
}

bool Parser::funcbody(AST *body) {
    insert_derivation({"{", "REPTFUNCBODY", "}"});

    return expect(LBRACE) & reptfuncbody(body) & expect(RBRACE);
}

bool Parser::reptfuncbody(AST *body) {
    // statement_starters or local or id
    if (!skipErrors({LOCAL, IDENTIFIER, SELF, IF, WHILE, READ, WRITE, RETURN, EPSILON}, {RBRACE})) return false;

    if (peek(LOCAL) || isStatement()) {
        insert_derivation({"LOCALVARDECLORSTAT", "REPTFUNCBODY"});
        auto declorstat = new AST(nexttok.line);
        body->adopt(declorstat);
        if (localvardeclorstat(declorstat) & reptfuncbody(body)) {
            return true;
        }
        delete declorstat;
        return false;
    }
    if (peek(RBRACE)) {
        accept_epsilon();
        return true;
    }

    return false;
}

bool Parser::localvardeclorstat(AST *declorstat) {
    if (peek(LOCAL)) {
        insert_derivation({"LOCALVARDECL"});
        declorstat->type = ASTType::VARDECL;
        if (localvardecl(declorstat)) {
            return true;
        }
        return false;
    }
    if (isStatement()) {
        insert_derivation({"STATEMENT"});
        declorstat->type = ASTType::STATEMENT;
        if (statement(declorstat)) {
            return true;
        }
        return false;
    } else {
        accept_epsilon();
        return false;
    }
}

bool Parser::attributedecl(AST *attr) {
    insert_derivation({"attribute", "VARDECL"});
    if (expect(ATTRIBUTE) & vardecl(attr)) {
        return true;
    }
    return false;
}

bool Parser::localvardecl(AST *decl) {
    insert_derivation({"local", "VARDECL"});
    if (expect(LOCAL) & vardecl(decl)) {
        return true;
    }
    return false;
}

bool Parser::vardecl(AST *decl) {
    if (!skipErrors({IDENTIFIER})) return false;
    insert_derivation({"id", ":", "TYPE", "ARRAYSIZES", ";"});
    auto id = new AST(ASTType::ID, nexttok.line);
    auto t = new AST(ASTType::TYPE, nexttok.line);
    auto as = new AST(ASTType::ARRAYSIZES, nexttok.line);
    if (identifier(id) & expect(COLON) & type(t) & arraysizes(as) & expect(SEMICOLON)) {
        decl->adopt({id, t, as});
        return true;
    }
    delete id;
    delete t;
    delete as;
    accept_epsilon();
    return false;
}

bool Parser::statement(AST *s) {
    if (peek(IDENTIFIER) || peek(SELF)) {
        insert_derivation({"FUNCALLORASSIGN", ";"});
        AST *f = nullptr;
        if (funcallorassign(&f) & expect(SEMICOLON)) {
            s->adopt(f);
            return true;
        }
        delete f;
        return false;
    } else if (peek(IF)) {
        insert_derivation({"if", "(", "RELEXPR", ")", "then", "STATBLOCK", "else", "STATBLOCK", ";"});
        auto r = new AST(ASTType::RELOP, nexttok.line);
        auto s1 = new AST(ASTType::STATBLOCK, nexttok.line);
        auto s2 = new AST(ASTType::STATBLOCK, nexttok.line);
        if (expect(IF) & expect(LPAREN) & relexpr(r) & expect(RPAREN) & expect(THEN) &
            statblock(s1) & expect(ELSE) & statblock(s2) & expect(SEMICOLON)) {
            auto i = new AST(ASTType::IF, nexttok.line, {r, s1, s2});
            s->adopt(i);
            return true;
        }
        delete r;
        delete s1;
        delete s2;
        return false;
    } else if (peek(WHILE)) {
        insert_derivation({"while", "(", "RELEXPR", ")", "STATBLOCK", ";"});
        auto r = new AST(ASTType::RELOP, nexttok.line);
        auto sb = new AST(ASTType::STATBLOCK, nexttok.line);

        if (expect(WHILE) & expect(LPAREN) & relexpr(r) & expect(RPAREN) &
            statblock(sb) & expect(SEMICOLON)) {
            auto w = new AST(ASTType::WHILE, nexttok.line, {r, sb});
            s->adopt(w);
            return true;
        }
        return false;
    } else if (peek(READ)) {
        insert_derivation({"read", "(", "VARIABLE", ")", ";"});
        auto r = new AST(ASTType::READ, nexttok.line);
        auto v = new AST(ASTType::VARIABLE, nexttok.line);

        if (expect(READ) & expect(LPAREN) & variable(v) & expect(RPAREN) &
            expect(SEMICOLON)) {
            r->adopt(v);
            s->adopt(r);
            return true;
        }
        delete v;
        return false;
    } else if (peek(WRITE)) {
        insert_derivation({"write", "(", "EXPR", ")", ";"});
        auto w = new AST(ASTType::WRITE, nexttok.line);
        auto e = new AST(ASTType::EXPR, nexttok.line);
        if (expect(WRITE) & expect(LPAREN) & expr(e) & expect(RPAREN) &
            expect(SEMICOLON)) {
            w->adopt(e);
            s->adopt(w);
            return true;
        }
        return false;
    } else if (peek(RETURN)) {
        insert_derivation({"return", "(", "EXPR", ")", ";"});
        auto r = new AST(ASTType::RETURN, nexttok.line);
        auto e = new AST(ASTType::EXPR, nexttok.line);

        if (expect(RETURN) & expect(LPAREN) & expr(e) & expect(RPAREN) &
            expect(SEMICOLON)) {
            r->adopt(e);
            s->adopt(r);
            return true;
        }
        return false;
    } else {
        error("statement");
        return false;
    }
}

bool Parser::funcallorassign(AST **f) {
    if (peek(IDENTIFIER) || peek(SELF)) {
        insert_derivation({"IDORSELF", "FUNCALLORASSIGN2"});
        auto id = new AST(nexttok.line); // Type to be decided by IDORSELF
        if (idorself(id) & funcallorassign2(id, f)) {
            return true;
        }
        delete id;
        return false;
    }
    error("id or self");
    return false;
}

bool Parser::funcallorassign2(AST *left, AST **right) {
    if (peek(LPAREN)) {
        insert_derivation({"(", "APARAMS", ")", "FUNCALLORASSIGN4"});
        auto f = new AST(ASTType::FUNCALL, nexttok.line);
        auto a = new AST(ASTType::APARAMS, nexttok.line);
        f->adopt({left, a});
        if (expect(LPAREN) & aparams(a) & expect(RPAREN) & funcallorassign4(f, right)) {
            return true;
        }
        delete f;
        delete a;
        return false;
    }
    if (peek(LBRACKET) || peek(DOT) || peek(ASSIGN)) {
        insert_derivation({"INDICES", "FUNCALLORASSIGN3"});
        auto i = new AST(ASTType::INDICES, nexttok.line);
        auto v = new AST(ASTType::DATAMEMBER, nexttok.line);
        v->adopt({left, i});
        if (indices(i) & funcallorassign3(v, right)) {
            return true;
        }
        delete i;
        return false;
    }
    error("'(', '[', '.', or ':='");
    return false;
}

bool Parser::funcallorassign3(AST *left, AST **right) {
    if (peek(ASSIGN)) {
        insert_derivation({":=", "EXPR"});
        auto a = new AST(ASTType::ASSIGN, nexttok.line);
        auto e = new AST(ASTType::EXPR, nexttok.line);
        a->adopt({left, e});
        if (expect(ASSIGN) & expr(e)) {
            *right = a;
            return true;
        }
        delete a;
        delete e;
        return false;
    }
    if (peek(DOT)) {
        insert_derivation({".", "id", "FUNCALLORASSIGN2"});
        auto d = new AST(ASTType::DOT, nexttok.line);
        auto id = new AST(ASTType::ID, nexttok.line);
        d->adopt({left, id});
        return expect(DOT) & identifier(id) & funcallorassign2(d, right);
    }
    error("':=' or '.'");
    return false;
}

bool Parser::funcallorassign4(AST *l, AST **r) {
    if (!skipErrors({DOT, EPSILON}, {SEMICOLON})) return false;

    if (peek(DOT)) {
        insert_derivation({".", "id", "FUNCALLORASSIGN2"});
        auto d = new AST(ASTType::DOT, nexttok.line);
        auto id = new AST(ASTType::ID, nexttok.line);
        d->adopt({l, id});
        auto r1 = new AST(nexttok.line);
        if (expect(DOT) & identifier(id) & funcallorassign2(d, &r1)) {
            *r = r1;
            return true;
        }
        delete d;
        delete id;
        return false;
    }
    if (peek(SEMICOLON)) {
        *r = l;
        accept_epsilon();
        return true;
    } else {
        error("'.' or ';'");
        return false;
    }
}

bool Parser::statblock(AST *sb) {
    if (!skipErrors({LBRACE, IF, WHILE, READ, WRITE, RETURN, IDENTIFIER, SELF, EPSILON}, {ELSE, SEMICOLON})) return
            false;

    if (peek(LBRACE)) {
        insert_derivation({"{", "STATEMENTS", "}"});
        auto stmts = new AST(ASTType::STATEMENTS, nexttok.line);
        if (expect(LBRACE) & statements(stmts) & expect(RBRACE)) {
            sb->adopt(stmts);
            return true;
        }
        delete stmts;
        return false;
    }
    if (isStatement()) {
        insert_derivation({"STATEMENT"});
        auto s = new AST(ASTType::STATEMENT, nexttok.line);
        if (statement(s)) {
            sb->adopt(s);
            return true;
        }
        delete s;
        return false;
    }
    if (peek(ELSE) || peek(SEMICOLON)) {
        accept_epsilon();
        return true;
    }
    error("'{' or statement");
    return false;
}

bool Parser::statements(AST *stmts) {
    assert(stmts != nullptr);
    if (!skipErrors({IF, WHILE, READ, WRITE, RETURN, IDENTIFIER, SELF, EPSILON}, {RBRACE})) return false;

    if (isStatement()) {
        insert_derivation({"STATEMENT", "STATEMENTS"});
        auto s = new AST(ASTType::STATEMENT, nexttok.line);
        stmts->adopt(s);
        if (statement(s) & statements(stmts)) {
            return true;
        }
        delete s;
        return false;
    } else if (peek(RBRACE)) {
        accept_epsilon();
        return true;
    } else {
        return false;
    }
}

bool Parser::expr(AST *e) {
    insert_derivation({"ARITHEXPR", "EXPRTAIL"});
    AST *a = nullptr;
    AST *right = nullptr;
    if (arithexpr(&a) & exprtail(a, &right)) {
        e->adopt(right);
        return true;
    }
    return false;
}

bool Parser::exprtail(AST *left, AST **right) {
    tokentype_set follow = {RPAREN, SEMICOLON, COMMA};
    if (!skipErrors({EQ, NEQ, LT, GT, LTEQ, GTEQ, EPSILON}, follow)) return false;

    if (isRelop()) {
        insert_derivation({"RELOP", "ARITHEXPR"});
        auto r = new AST(ASTType::RELOP, nexttok.line);
        AST *a = nullptr;
        if (relop(r) & arithexpr(&a)) {
            r->adopt({left, a});
            *right = r;
            return true;
        }
        delete r;
        delete a;
        return false;
    } else if (token_in(follow)) {
        accept_epsilon();
        *right = left;
        return true;
    } else {
        error("relop, ')', ';', or ','");
        return false;
    }
}

bool Parser::relexpr(AST *rel) {
    insert_derivation({"ARITHEXPR", "RELOP", "ARITHEXPR"});
    AST *a1 = nullptr;
    AST *a2 = nullptr;
    if (arithexpr(&a1) & relop(rel) & arithexpr(&a2)) {
        rel->adopt({a1, a2});
        return true;
    }
    delete a1;
    delete a2;
    return false;
}

bool Parser::arithexpr(AST **a) {
    insert_derivation({"TERM", "RIGHTRECARITHEXPR"});
    if (term(a) & rightrecarithexpr(*a, a)) {
        return true;
    }
    return false;
}

bool Parser::rightrecarithexpr(AST *left, AST **right) {
    tokentype_set follow = {RPAREN, SEMICOLON, COMMA, EQ, GT, GTEQ, LT, LTEQ, NEQ, RBRACKET};
    if (!skipErrors({ADD, SUB, OR, EPSILON}, follow)) return false;

    if (isAddop()) {
        insert_derivation({"ADDOP", "TERM", "RIGHTRECARITHEXPR"});
        bool success = true;
        auto a = new AST(ASTType::ADDOP, nexttok.line);
        AST *t;
        if (!(addop(a) & term(&t))) success = false;
        a->adopt({left, t});
        if (!rightrecarithexpr(a, right)) success = false;

        if (success) {
            return true;
        }
        delete a;
        return false;
    } else if (token_in(follow)) {
        accept_epsilon();
        *right = left;
        return true;
    } else {
        error("addop, ')', ';', ',', '==', '<>', '<', '>', '<=', '>=', or ']'");
        return false;
    }
}

bool Parser::sign(AST *s) {
    if (match(ADD) || match(SUB)) {
        s->str_value = curtok.value;
        return true;
    }
    error("sign");
    accept_epsilon();
    return false;
}

bool Parser::term(AST **t) {
    insert_derivation({"FACTOR", "RIGHTRECTERM"});
    if (factor(t) & rightrecterm(*t, t)) {
        return true;
    }
    return false;
}

bool Parser::rightrecterm(AST *left, AST **right) {
    if (!skipErrors({MUL, DIV, AND, EPSILON}, {
                        RPAREN, SEMICOLON, COMMA, EQ, GT, GTEQ, LT, LTEQ, NEQ, RBRACKET, SUB, ADD, OR
                    })) return false;

    if (isMultop()) {
        insert_derivation({"MULTOP", "FACTOR", "RIGHTRECTERM"});
        bool success = true;
        auto m = new AST(ASTType::MULTOP, nexttok.line);
        AST *f;
        if (!(multop(m) & factor(&f))) success = false;
        m->adopt({left, f});
        if (!rightrecterm(m, right)) success = false;

        if (success) {
            return true;
        }
        delete m;
        delete f;
        return false;
    } else if (token_in_values({")", ";", ",", "==", ">", ">=", "<", "<=", "<>", "]", "+", "-", "or"})) {
        accept_epsilon();
        *right = left;
        return true;
    } else {
        error("multop, ')', ';', ',', '==', '>', '>=', '<', '<=', '<>', ']', '+', '-', or 'or'");
        return false;
    }
}

bool Parser::factor(AST **f) {
    if (peek(INTLIT)) {
        insert_derivation({"intlit"});
        auto il = new ASTIntLit(nexttok.line);
        if (intlit(il)) {
            *f = il;
            return true;
        }
        delete il;
        return false;
    }
    if (peek(FLOATLIT)) {
        insert_derivation({"floatlit"});
        auto fl = new ASTFloatLit(nexttok.line);
        if (floatlit(fl)) {
            *f = fl;
            return true;
        }
        delete fl;
        return false;
    }
    if (peek(LPAREN)) {
        insert_derivation({"(", "ARITHEXPR", ")"});
        if (expect(LPAREN) & arithexpr(f) & expect(RPAREN)) {
            return true;
        }
        return false;
    }
    if (peek(ADD) || peek(SUB)) {
        insert_derivation({"SIGN", "FACTOR"});
        auto s = new AST(ASTType::SIGN, nexttok.line);
        AST *f2;
        if (sign(s) & factor(&f2)) {
            *f = s->adopt(f2);
            return true;
        }
        delete s;
        delete f2;
        return false;
    }
    if (peek(NOT)) {
        insert_derivation({"not", "FACTOR"});
        auto n = new AST(ASTType::NOT, nexttok.line);
        AST *f2;
        if (expect(NOT) & factor(&f2)) {
            *f = n->adopt(f2);
            return true;
        }
        delete n;
        return false;
    }
    if (peek(IDENTIFIER) || peek(SELF)) {
        insert_derivation({"IDORSELF", "FACTOR2", "REPTIDNEST"});
        auto id = new AST(ASTType::ID, nexttok.line);
        AST *result = nullptr;
        AST *result2 = nullptr;
        if (idorself(id) & factor2(id, &result) & reptidnest(result, &result2)) {
            *f = result2;
            return true;
        }
        std::cout << "Something happened" << std::endl;
        return false;
    } else {
        error("intlit, floatlit, '(', '+', '-', 'not', id, or self");
        return false;
    }
}

bool Parser::factor2(AST *left, AST **right) {
    if (!skipErrors({LPAREN, LBRACKET, EPSILON}, {
                        RPAREN, SEMICOLON, COMMA, EQ, GT, GTEQ, LT, LTEQ, NEQ, RBRACKET, ADD, SUB, OR, MUL, DIV, AND,
                        DOT
                    })) return false;

    if (peek(LPAREN)) {
        insert_derivation({"(", "APARAMS", ")"});
        auto f = new AST(ASTType::FUNCALL, nexttok.line);
        auto a = new AST(ASTType::APARAMS, nexttok.line);
        if (expect(LPAREN) & aparams(a) & expect(RPAREN)) {
            f->adopt({left, a});
            *right = f;
            return true;
        }
        delete f;
        delete a;
        return false;
    } else {
        insert_derivation({"INDICES"});
        auto d = new AST(ASTType::DATAMEMBER, nexttok.line);
        auto i = new AST(ASTType::INDICES, nexttok.line);
        d->adopt({left, i});
        if (indices(i)) {
            *right = d;
            return true;
        }
        return false;
    }
}

bool Parser::indices(AST *i) {
    tokentype_set follow = {
        DOT, ASSIGN, RPAREN, SEMICOLON, COMMA, EQ, GT, GTEQ, LT, LTEQ, NEQ, RBRACKET, ADD, SUB, OR, MUL, DIV, AND
    };
    if (!skipErrors({LBRACKET, EPSILON}, follow)) return false;

    if (peek(LBRACKET)) {
        insert_derivation({"INDICE", "INDICES"});
        bool success = true;
        AST *ind = nullptr;
        if (!indice(&ind)) success = false;
        i->adopt(ind);
        if (!indices(i)) success = false;

        if (!success) {
            delete ind;
            return false;
        }
        return true;
    }
    if (token_in(follow)) {
        accept_epsilon();
        return true;
    }
    else {
        return false;
    }
}

bool Parser::reptidnest(AST *left, AST **right) {
    if (!skipErrors({DOT, EPSILON}, {
                        RPAREN, SEMICOLON, COMMA, EQ, GT, GTEQ, LT, LTEQ, NEQ, RBRACKET, ADD, SUB, OR, MUL, DIV, AND
                    })) return false;

    if (peek(DOT)) {
        insert_derivation({"IDNEST", "REPTIDNEST"});
        AST *result;
        return idnest(left, &result) & reptidnest(result, right);
    }
    if (token_in_values({")", ";", ",", "==", ">", ">=", "<", "<=", "<>", "]", "+", "-", "or", "and", "*", "/"})) {
        *right = left;
        accept_epsilon();
        return true;
    } else {
        return false;
    }
}

bool Parser::variable(AST *v) {
    insert_derivation({"IDORSELF", "VARIABLE2"});
    auto id = new AST(nexttok.line);
    auto result = new AST(nexttok.line);
    if (idorself(id) & variable2(id, &result)) {
        v->adopt(result);
        return true;
    }
    delete id;
    delete result;
    return false;
}

bool Parser::variable2(AST *left, AST **var) {
    if (!skipErrors({LPAREN, LBRACKET, DOT, EPSILON}, {RPAREN})) return false;

    if (peek(LPAREN)) {
        insert_derivation({"(", "APARAMS", ")", "VARIDNEST"});
        auto f = new AST(ASTType::FUNCALL, nexttok.line);
        auto a = new AST(ASTType::APARAMS, nexttok.line);
        f->adopt({left, a});
        AST *result;

        if (expect(LPAREN) & aparams(a) & expect(RPAREN) & varidnest(f, &result)) {
            *var = result;
            return true;
        }

        return false;
    }
    if (peek(LBRACKET) || peek(DOT)) {
        insert_derivation({"INDICES", "REPTVARIABLE"});
        auto d = new AST(ASTType::DATAMEMBER, nexttok.line);
        auto i = new AST(ASTType::INDICES, nexttok.line);
        d->adopt({left, i});
        AST *result;
        if (indices(i) & reptvariable(d, &result)) {
            *var = result;
            return true;
        }
        delete i;
        return false;
    }
    if (token_in({RPAREN})) {
        auto d = new AST(ASTType::DATAMEMBER, nexttok.line);
        auto i = new AST(ASTType::INDICES, nexttok.line);
        d->adopt({left, i});
        *var = d;
        accept_epsilon();
        return true;
    }
    error("'(' or '['");
    return false;
}

bool Parser::reptvariable(AST *left, AST **var) {
    if (!skipErrors({DOT, EPSILON}, {RPAREN})) return false;

    if (peek(DOT)) {
        insert_derivation({"VARIDNEST", "REPTVARIABLE"});
        AST *result = nullptr;
        return varidnest(left, &result) & reptvariable(result, var);
    }
    if (token_in({RPAREN})) {
        *var = left;
        accept_epsilon();
        return true;
    } else {
        return false;
    }
}

bool Parser::varidnest(AST *left, AST **result) {
    if (peek(DOT)) {
        insert_derivation({".", "id", "VARIDNESTTAIL"});
        auto d = new AST(ASTType::DOT, nexttok.line);
        auto id = new AST(ASTType::ID, nexttok.line);
        d->adopt({left, id});
        return expect(DOT) & identifier(id) & varidnesttail(d, result);
    } else {
        error(".");
        return false;
    }
}

bool Parser::varidnesttail(AST *left, AST **result) {
    if (!skipErrors({LPAREN, LBRACKET, EPSILON}, {RPAREN, DOT})) return false;

    if (peek(LPAREN)) {
        insert_derivation({"(", "APARAMS", ")", "VARIDNEST"});
        auto f = new AST(ASTType::FUNCALL, nexttok.line);
        auto a = new AST(ASTType::APARAMS, nexttok.line);
        f->adopt({left, a});
        if (expect(LPAREN) & aparams(a) & expect(RPAREN) & varidnest(f, result)) {
            return true;
        }
        delete f;
        delete a;
        return false;
    } else {
        insert_derivation({"INDICES"});
        auto d = new AST(ASTType::DATAMEMBER, nexttok.line);
        auto i = new AST(ASTType::INDICES, nexttok.line);
        d->adopt({left, i});

        if (indices(i)) {
            *result = d;
            return true;
        }
        delete d;
        return false;
    }
}

bool Parser::indice(AST **i) {
    insert_derivation({"[", "ARITHEXPR", "]"});
    if (expect(LBRACKET) & arithexpr(i) & expect(RBRACKET)) {
        return true;
    }
    return false;
}

bool Parser::idnest(AST *left, AST **right) {
    insert_derivation({".", "id", "IDNESTTAIL"});
    auto d = new AST(ASTType::DOT, nexttok.line);
    auto id = new AST(ASTType::ID, nexttok.line);
    d->adopt({left, id});
    if (expect(DOT) & identifier(id) & idnesttail(d, right)) {
        return true;
    }
    delete d;
    delete id;
    return false;
}

bool Parser::idnesttail(AST *left, AST **right) {
    if (!skipErrors({LPAREN, LBRACKET, EPSILON}, {
                        DOT, RPAREN, SEMICOLON, COMMA, EQ, GT, GTEQ, LT, LTEQ, NEQ, RBRACKET, ADD, SUB, OR, MUL, DIV,
                        AND
                    })) return false;

    if (peek(LPAREN)) {
        insert_derivation({"(", "APARAMS", ")"});
        auto f = new AST(ASTType::FUNCALL, nexttok.line);
        auto a = new AST(ASTType::APARAMS, nexttok.line);
        if (expect(LPAREN) & aparams(a) & expect(RPAREN)) {
            f->adopt({left, a});
            *right = f;
            return true;
        }
        delete f;
        delete a;
        return false;
    } else {
        insert_derivation({"INDICES"});
        auto d = new AST(ASTType::DATAMEMBER, nexttok.line);
        auto i = new AST(ASTType::INDICES, nexttok.line);
        if (indices(i)) {
            d->adopt({left, i});
            *right = d;
            return true;
        }
        delete d;
        delete i;
        return false;
    }
}

bool Parser::arraysize(AST *size) {
    insert_derivation({"[", "ARRAYSIZETAIL"});
    return expect(LBRACKET) & arraysizetail(size);
}

bool Parser::arraysizetail(AST *size) {
    if (peek(INTLIT)) {
        insert_derivation({"intlit", "]"});
        auto i = new ASTIntLit(nexttok.line);
        if (intlit(i) & expect(RBRACKET)) {
            size->adopt(i);
            return true;
        }
        return false;
    } else if (peek(RBRACKET)) {
        insert_derivation({"]"});
        return expect(RBRACKET);
    } else {
        error("intlit or ']'");
        return false;
    }
}

bool Parser::arraysizes(AST *as) {
    std::unordered_set<TokenType> follow = {SEMICOLON, RPAREN, COMMA};
    if (!skipErrors({LBRACKET, EPSILON}, follow)) return false;

    if (peek(LBRACKET)) {
        insert_derivation({"ARRAYSIZE", "ARRAYSIZES"});
        auto size = new AST(ASTType::ARRAYSIZE, nexttok.line);
        if (arraysize(size) & arraysizes(as)) {
            as->adopt(size);
            return true;
        }
        return false;
    } else if (token_in(follow)) {
        accept_epsilon();
        return true;
    } else {
        accept_epsilon();
        return false;
    }
}

bool Parser::type(AST *t) {
    if (match(IDENTIFIER) || match(INT) || match(FLOAT)) {
        t->str_value = curtok.value;
        return true;
    }
    accept_epsilon();
    error("type");
    return false;
}

bool Parser::returntype(AST *t) {
    if (match(VOID)) {
        t->str_value = curtok.value;
        return true;
    } else {
        return type(t);
    }
}

bool Parser::aparams(AST *params) {
    if (!skipErrors({INTLIT, FLOATLIT, IDENTIFIER, LPAREN, ADD, SUB, NOT, SELF, EPSILON}, {RPAREN})) return false;

    if (isFactor()) {
        insert_derivation({"EXPR", "REPTAPARAMS"});
        auto e = new AST(ASTType::EXPR, nexttok.line);
        params->adopt(e);
        if (expr(e) & reptaparams(params)) {
            return true;
        }
        delete e;
        return false;
    } else if (peek(RPAREN)) {
        accept_epsilon();
        return true;
    } else {
        return false;
    }
}

bool Parser::reptaparams(AST *params) {
    if (!skipErrors({COMMA, EPSILON}, {RPAREN})) return false;

    if (peek(COMMA)) {
        insert_derivation({"APARAMSTAIL", "REPTAPARAMS"});
        auto e = new AST(ASTType::EXPR, nexttok.line);
        params->adopt(e);
        if (aparamstail(e) & reptaparams(params)) {
            return true;
        }
        delete e;
        return false;
    } else if (peek(RPAREN)) {
        accept_epsilon();
        return true;
    }
    return false;
}

bool Parser::aparamstail(AST *e) {
    insert_derivation({",", "EXPR"});
    return expect(COMMA) & expr(e);
}

bool Parser::fparams(AST *fp) {
    if (!skipErrors({IDENTIFIER, EPSILON}, {RPAREN})) return false;

    if (peek(IDENTIFIER)) {
        auto param = new AST(ASTType::FPARAM, nexttok.line);
        auto id = new AST(ASTType::ID, nexttok.line);
        auto t = new AST(ASTType::TYPE, nexttok.line);
        auto as = new AST(ASTType::ARRAYSIZES, nexttok.line);

        insert_derivation({"id", ":", "TYPE", "ARRAYSIZES", "REPTFPARAMS"});
        fp->adopt(param);
        if (identifier(id) & expect(COLON) & type(t) & arraysizes(as) & reptfparams(fp)) {
            param->adopt({id, t, as});
            return true;
        }
        return false;
    } else if (peek(RPAREN)) {
        accept_epsilon();
        return true;
    }
    accept_epsilon();
    return false;
}

bool Parser::reptfparams(AST *fp) {
    if (!skipErrors({COMMA, EPSILON}, {RPAREN})) return false;

    if (peek(COMMA)) {
        insert_derivation({",", "id", ":", "TYPE", "ARRAYSIZES", "REPTFPARAMS"});
        auto param = new AST(ASTType::FPARAM, nexttok.line);
        auto id = new AST(ASTType::ID, nexttok.line);
        auto t = new AST(ASTType::TYPE, nexttok.line);
        auto as = new AST(ASTType::ARRAYSIZES, nexttok.line);
        fp->adopt(param);
        if (expect(COMMA) & identifier(id) & expect(COLON) & type(t) & arraysizes(as) & reptfparams(fp)) {
            param->adopt({id, t, as});
            return true;
        }
        return false;
    } else if (peek(RPAREN)) {
        accept_epsilon();
        return true;
    }
    accept_epsilon();
    return false;
}

bool Parser::relop(AST *r) {
    assert(r != nullptr);
    if (match(EQ) || match(NEQ) || match(LT) || match(GT) ||
        match(LTEQ) || match(GTEQ)) {
        r->str_value = curtok.value;
        r->line_number = curtok.line;
        return true;
    }
    error("relop");
    return false;
}

bool Parser::addop(AST *a) {
    assert(a != nullptr);
    if (match(ADD) || match(SUB) || match(OR)) {
        a->str_value = curtok.value;
        return true;
    }
    error("addop");
    return false;
}

bool Parser::multop(AST *m) {
    assert(m != nullptr);
    if (match(MUL) || match(DIV) || match(AND)) {
        m->str_value = curtok.value;
        return true;
    }
    error("multop");
    return false;
}

bool Parser::idorself(AST *i) {
    if (peek(IDENTIFIER)) {
        i->type = ASTType::ID;
        return identifier(i);
    }
    if (peek(SELF)) {
        i->type = ASTType::SELF;
        return expect(SELF);
    }
    error("id or self");
    return false;
}

bool Parser::intlit(ASTIntLit *i) {
    if (!expect(INTLIT)) {
        accept_epsilon();
        return false;
    }
    i->value = std::stoi(curtok.value);
    return true;
}

bool Parser::floatlit(ASTFloatLit *f) {
    if (!expect(FLOATLIT)) {
        accept_epsilon();
        return false;
    }
    f->value = std::stof(curtok.value);
    return true;
}

bool Parser::identifier(AST *id) {
    if (!expect(IDENTIFIER)) return false;
    id->str_value = curtok.value;
    return true;
}


bool Parser::isFactor() const {
    return token_in({INTLIT, FLOATLIT, IDENTIFIER, LBRACE, LPAREN, ADD, SUB, SELF});
}

bool Parser::isAddop() const {
    return token_in({ADD, SUB, OR});
}

bool Parser::isRelop() const {
    return token_in({EQ, NEQ, LT, GT, LTEQ, GTEQ});
}

bool Parser::isMultop() const {
    return token_in({MUL, DIV, AND});
}

bool Parser::isStatement() const {
    return token_in_values(statement_starters) || peek(IDENTIFIER);
}

#pragma clang diagnostic pop
