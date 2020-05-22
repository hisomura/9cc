#include "9cc.h"

// ローカル変数
static LVar *locals;

Function *function();

Node *stmt();

Node *expr();

Node *assign();

Node *equality();

Node *relational();

Node *add();

Node *mul();

Node *unary();

Node *primary();

LVar *find_lvar(Token *tok);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

Token *consume_ident() {
    if (token->kind != TK_IDENT)
        return 0;
    Token *current = token;
    token = token->next;
    return current;
}

// 次のトークンが期待したものでなければエラー報告
void assert_token(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        error_at(token->str, "'%s'ではありません", op);
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op) {
    assert_token(op);
    token = token->next;
}

Token *expect_ident() {
    if (token->kind != TK_IDENT)
        error_at(token->str, "識別子ではありません");
    Token *current = token;
    token = token->next;
    return current;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// 型があったらtokenを消費してTypeを作成。無ければNULLを返す。
Type *basetype() {
    if (!consume("int")) return NULL;

    Type *head = calloc(1, sizeof(int));
    head->kind = TY_INT;

    while (consume("*")) {
        Type *new_head = calloc(1, sizeof(int));
        new_head->kind = TY_PTR;
        new_head->base = head;
        head = new_head;
    }

    return head;
}

Function *function() {
    locals = NULL;
    Function *func;

    Type *ret_type = basetype();
    Token *tok = expect_ident();
    expect("(");

    LVar head = {};
    LVar *cur = &head;
    while (!consume(")")) {
        Type *arg_type = basetype();
        Token *ident = consume_ident();
        cur->next = calloc(1, sizeof(LVar));
        cur->next->name = strndup(ident->str, ident->len);
        cur->next->offset = cur->offset + 8;
        cur->next->ty = arg_type;
        cur = cur->next;
        consume(",");
    }
    locals = head.next;

    assert_token("{");
    Node *block = stmt();
    if (block->kind != ND_BLOCK) {
        error_at(token->str, "関数の中身が得られませんでした");
    }

    func = calloc(1, sizeof(Function));
    func->name = strndup(tok->str, tok->len);
    func->block = block;
    func->locals = locals;
    func->args = head.next;
    func->ret_ty = ret_type;
    // 引数は右に、変数は左に伸びるのでargsからnextをたどれば引数だけ取得できる
    // localsからnextをたどるとローカル変数と引数の両方を取得できる

    return func;
}


Function *program() {
    Function *head = function();
    Function *cur = head;
    while (!at_eof()) {
        cur->next = function();
        cur = cur->next;
    }

    return head;
}

Node *stmt() {
    Node *node;

    if (consume("if")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        if (consume("else")) {
            node->els = stmt();
        }
        return node;
    }
    if (consume("while")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();

        return node;
    }
    if (consume("for")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_FOR;
        expect("(");

        if (!consume(";")) { // init
            node->init = expr();
            expect(";");
        }
        if (!consume(";")) { // cond
            node->cond = expr();
            expect(";");
        }
        if (!consume(")")) { // increment
            node->inc = expr();
            expect(")");
        }
        node->then = stmt();

        return node;
    }
    if (consume("{")) {
        Node head = {};
        Node *cur = &head;
        while (!consume("}")) {
            cur = cur->next = stmt();
        }
        node = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        node->body = head.next;

        return node;
    }

    Type *ty = basetype();
    if (ty) { // 変数定義
        Token *ident = expect_ident();
        if (find_lvar(ident)) error_at(ident->str, "定義済みの変数が定義されています");

        LVar *lvar = calloc(1, sizeof(LVar));
        lvar->next = locals;
        lvar->name = strndup(ident->str, ident->len);

        if (consume("[")) {
            int num = expect_number();
            expect("]");
            // FIXME オフセット決め打ち
            lvar->offset = locals ? locals->offset + 4 * num : 8;
            lvar->ty = array_of(ty, num);
        } else {
            lvar->offset = locals ? locals->offset + 8 : 8;
            lvar->ty = ty;
        }
        locals = lvar;

        node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR_DEF;
        node->offset = lvar->offset;
        node->lvar = lvar;
    } else if (consume("return")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
    } else {
        node = expr();
    }
    if (!consume(";"))
        error_at(token->str, "';'ではないトークンです");

    return node;
}

Node *expr() {
    return assign();
}


Node *assign() {
    Node *node = equality();
    if (consume("="))
        node = new_node(ND_ASSIGN, node, assign());
    return node;
}

Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("=="))
            node = new_node(ND_EQ, node, relational());
        else if (consume("!="))
            node = new_node(ND_NE, node, relational());
        else
            return node;
    }
}

Node *relational() {
    Node *node = add();
    for (;;) {
        if (consume("<="))
            node = new_node(ND_LE, node, add());
        else if (consume(">="))
            node = new_node(ND_LE, add(), node);
        else if (consume("<"))
            node = new_node(ND_LT, node, add());
        else if (consume(">"))
            node = new_node(ND_LT, add(), node);
        else
            return node;
    }
}

Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *unary() {
    if (consume("+"))
        return primary();
    if (consume("-"))
        return new_node(ND_SUB, new_node_num(0), primary());
    if (consume("&"))
        return new_node(ND_ADDR, unary(), NULL);
    if (consume("*"))
        return new_node(ND_DEREF, unary(), NULL);
    if (consume("sizeof")) {
        expect("(");
        Node *tmp = expr();
        expect(")");
        if (!tmp) error_at(token->str, "sizeofに値が指定されていません");
        return new_node(ND_SIZEOF, tmp, NULL);
    }

    return primary();
}

Node *primary() {
    // 次のトークンが"("なら、"(" expr ")"のはず
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();
    if (tok) {
        // 関数の処理
        if (consume("(")) {
            Node head = {};
            Node *cur = &head;
            while (!consume(")")) {
                cur = cur->next = expr();
                consume(",");
            }

            Node *node = calloc(1, sizeof(Node));
            node->kind = ND_FUNC_CALL;
            node->func_name = strndup(tok->str, tok->len);
            node->args = head.next;
            return node;
        }

        LVar *lvar = find_lvar(tok);
        if (!lvar) error_at(tok->str, "定義されていない変数を利用しています");

        // 変数の処理
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;
        node->offset = lvar->offset;
        node->lvar = lvar;

        return node;
    }

    // そうでなければ数値のはず
    return new_node_num(expect_number());
}

// 変数を名前で検索する。見つからなかった場合はNULLを返す。
LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next)
        if (strlen(var->name) == tok->len && !memcmp(tok->str, var->name, strlen(var->name)))
            return var;
    return NULL;
}
