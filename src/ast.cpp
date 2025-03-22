#include "ast.h"

#include "visitor.h"


void AST::accept(Visitor &v) {
    v.visit(this);
}
