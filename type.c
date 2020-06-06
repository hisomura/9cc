#include "9cc.h"


Type *new_type(TypeKind kind) {
    Type *ty = malloc(sizeof(Type));
    ty->kind = kind;
    switch (kind) {
        case TY_CHAR:
            ty->size = 1;
            break;
        case TY_INT:
            ty->size = 4;
            break;
        default:
            error("kindに不正な値が入っている");
    }
    return ty;
}

Type *pointer_to(Type *base) {
    Type *ty = malloc(sizeof(Type));
    ty->kind = TY_PTR;
    ty->base = base;
    ty->size = 8;

    return ty;
}

Type *array_of(Type *base, int length) {
    Type *ty = calloc(1, sizeof(Type));
    ty->kind = TY_ARRAY;
    ty->base = base;
    ty->size = base->size * length;
    return ty;
}

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
        case ND_EXPR_STMT:
            return;
        case ND_STMT_EXPR: {
            Node *n = node->body;
            while (n->next) n = n->next;
            if (n->kind != ND_EXPR_STMT) {
                error("statement expressionのbodyの最後がexpression statementではない");
            }
            node->ty = n->lhs->ty;
            return;
        }
        case ND_ADD:
        case ND_SUB:
            if (node->lhs->ty->base && node->rhs->ty->base) {
                error("ポインタや配列同士の加算減算は存在しない %s", node->code);
            }
            // 右辺がポインタまたは配列の場合、両辺を入れ替え
            // codegenでも左辺がポインタ、配列である前提のコードがあるので入れ替えは必須
            if (node->rhs->ty->base) {
                Node *tmp = node->rhs;
                node->rhs = node->lhs;
                node->lhs = tmp;
            }
        case ND_MUL:
        case ND_DIV:
        case ND_MOD:
        case ND_ASSIGN:
            node->ty = node->lhs->ty;
            return;
        case ND_EQ:
        case ND_NE:
        case ND_LT:
        case ND_LE:
        case ND_NUM:
        case ND_VAR:
            return;
        case ND_ADDR: {
            node->ty = pointer_to(node->lhs->ty);
            return;
        }
        case ND_DEREF:
            switch (node->lhs->ty->kind) {
                case TY_PTR:
                case TY_ARRAY:
                    node->ty = node->lhs->ty->base;
                    return;
                default:
                    error("*に続く値がポインタでも配列ではない");
                    return;
            }
        case ND_FUNC_CALL:
            // FIXME func()
            // ノードには関数名しか保存されてなくて今のところ解決不能
            node->ty = new_type(TY_INT);
            return;
        case ND_SIZEOF:
            node->kind = ND_NUM;
            node->ty = new_type(TY_INT);

            node->val = node->lhs->ty->size;
            node->lhs = NULL;

            return;
    }
}

void add_type(Function *prog) {
    for (Function *fn = prog; fn; fn = fn->next)
        for (Node *node = fn->block->body; node; node = node->next)
            visit(node);
}