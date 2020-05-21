#include "9cc.h"

void visit(Node *node) {
    if (!node)
        return;

    visit(node->lhs);
    visit(node->rhs);
    visit(node->cond);
    visit(node->then);
    visit(node->els);
    visit(node->init);
    visit(node->inc);

    for (Node *n = node->body; n; n = n->next)
        visit(n);
    for (Node *n = node->args; n; n = n->next)
        visit(n);

    node->ty = calloc(1, sizeof(Type));
    node->ty = INT;
}

void add_type(Function *prog) {
    for (Function *fn = prog; fn; fn = fn->next)
        for (Node *node = fn->block->body; node; node = node->next)
            visit(node);
}