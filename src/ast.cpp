#include "ast.h"

#include "visitor/visitor.h"


void AST::accept(Visitor &v) {
    v.visit(this);
}
