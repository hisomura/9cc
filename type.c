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

    switch (node->kind) {
        case ND_IF:
        case ND_WHILE:
        case ND_FOR:
        case ND_RETURN:
        case ND_BLOCK:
        case ND_LVAR_DEF:
            return;
        case ND_ADD:
        case ND_SUB:
        case ND_MUL:
        case ND_DIV:
        case ND_ASSIGN:
            node->ty = node->lhs->ty;
            return;
        case ND_EQ:
        case ND_NE:
        case ND_LT:
        case ND_LE:
        case ND_NUM:
            node->ty = calloc(1, sizeof(Type));
            node->ty->kind = TY_INT;
            return;
        case ND_LVAR:
            node->ty = node->lvar->ty;
            return;
        case ND_ADDR:
            // FIXME &
            return;
        case ND_DEREF:
            // FIXME *
            return;
        case ND_FUNC_CALL:
            // FIXME func()
            // ノードには関数名しか保存されてなくて今のところ解決不能
            return;
    }
}

void add_type(Function *prog) {
    for (Function *fn = prog; fn; fn = fn->next)
        for (Node *node = fn->block->body; node; node = node->next)
            visit(node);
}