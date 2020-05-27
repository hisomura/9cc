#include "9cc.h"

// 現在着目しているトークン
Token *token;

// ローカル変数
static Var *locals;
static Var *globals;

Function *function();

Node *stmt();

Node *expr();

Node *assign();

Node *equality();

Node *relational();

Node *add();

Node *mul();

Node *unary();

static Node *postfix();

Node *primary();

Var *find_var(Token *tok);

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

Node *new_node_var(Var *var) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_VAR;
    node->var = var;
    return node;
}

static Var *add_global_var(char *name, Type *ty) {
    Var *var = calloc(1, sizeof(Var));
    var->name = name;
    var->ty = ty;
    var->is_local = false;
    var->next = globals;
    globals = var;
    return var;
}

static char *new_global_var_name(void) {
    static int cnt = 0;
    char *buf = malloc(20);
    sprintf(buf, ".L.data.%d", cnt++);
    return buf;
}

static Var *new_string_literal(char *p, int len) {
    new_type(TY_CHAR);
    Type *ty = array_of(new_type(TY_CHAR), len + 1); //\nが追加されるので+1
    Var *var = add_global_var(new_global_var_name(), ty);
    var->init_data = strndup(p, len); // \n追加もやってくれる
    return var;
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

Token *consume_literal() {
    if (token->kind != TK_STR)
        return false;
    Token *current = token;
    token = token->next;
    return current;
}

bool token_is(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return false;
    return true;
}

// 次のトークンが期待したものでなければエラー報告
void assert_token(char *op) {
    if (!token_is(op))
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
    Type *base;
    if (consume("int")) {
        base = new_type(TY_INT);
    } else if (consume("char")) {
        base = new_type(TY_CHAR);
    } else {
        return NULL;
    }

    while (consume("*")) {
        Type *new_type = pointer_to(base);
        base = new_type;
    }

    return base;
}

Function *function(Type *ret_type, Token *tok) {
    locals = NULL;
    Function *func;
    expect("(");

    Var head = {};
    Var *cur = &head;
    while (!consume(")")) {
        Type *arg_type = basetype();
        Token *ident = consume_ident();
        cur->next = calloc(1, sizeof(Var));
        cur->next->name = strndup(ident->str, ident->len);
        cur->next->ty = arg_type;
        cur->next->is_local = true;
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

static Type *type_suffix(Type *ty) {
    if (consume("[")) {
        int num = expect_number();
        expect("]");

        Type *base = type_suffix(ty);
        return array_of(base, num);
    }
    return ty;
}

Program *program() {
    Function head = {};
    Function *cur = &head;
    globals = NULL;

    while (!at_eof()) {
        Type *base = basetype();
        Token *tok = expect_ident();
        if (token_is("(")) {
            cur->next = function(base, tok);
            cur = cur->next;
            continue;
        }

        Type *ty = type_suffix(base);
        expect(";");
        add_global_var(strndup(tok->str, tok->len), ty);
    }

    Program *pg = calloc(1, sizeof(Program));
    pg->functions = head.next;
    pg->globals = globals;

    return pg;
}

void copy_code(Node *node, char *code_start) {
    node->code = strndup(code_start, token->str - code_start);
}

Node *stmt() {
    char *node_start = token->str;
    Node *node = NULL;

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

        copy_code(node, node_start);
        return node;
    }
    if (consume("while")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();

        copy_code(node, node_start);
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

        copy_code(node, node_start);
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

        copy_code(node, node_start);
        return node;
    }

    Type *ty = basetype();
    if (ty) { // 変数定義
        Token *ident = expect_ident();
        if (find_var(ident)) error_at(ident->str, "定義済みの変数が定義されています");

        Var *lvar = calloc(1, sizeof(Var));
        lvar->next = locals;
        lvar->name = strndup(ident->str, ident->len);
        lvar->ty = type_suffix(ty);
        lvar->is_local = true;
        locals = lvar;

        if (consume("=")) {
            node = new_node(ND_ASSIGN, new_node_var(lvar), assign());
        } else {
            node = new_node(ND_LVAR_DEF, NULL, NULL); // 不要だと思うけどnullにすると他が面倒なので残す
        }
        expect(";");

        copy_code(node, node_start);
        return node;
    }

    if (consume("return")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
    } else {
        node = expr();
    }
    if (!consume(";"))
        error_at(token->str, "';'ではないトークンです");

    copy_code(node, node_start);
    return node;
}

Node *expr() {
    char *node_start = token->str;
    Node *node = assign();
    copy_code(node, node_start);
    return node;
}


Node *assign() {
    char *node_start = token->str;
    Node *node = equality();
    if (consume("="))
        node = new_node(ND_ASSIGN, node, assign());
    copy_code(node, node_start);
    return node;
}

Node *equality() {
    char *node_start = token->str;
    Node *node = relational();

    for (;;) {
        if (consume("=="))
            node = new_node(ND_EQ, node, relational());
        else if (consume("!="))
            node = new_node(ND_NE, node, relational());
        else {
            copy_code(node, node_start);
            return node;
        }
    }
}

Node *relational() {
    char *node_start = token->str;
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
            copy_code(node, node_start);
        return node;
    }
}

Node *add() {
    char *node_start = token->str;
    Node *node = mul();

    for (;;) {
        if (consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else {
            copy_code(node, node_start);
            return node;
        }
    }
}

Node *mul() {
    char *node_start = token->str;
    Node *node = unary();

    for (;;) {
        if (consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else {
            copy_code(node, node_start);
            return node;
        }
    }
}

Node *unary() {
    char *node_start = token->str;
    Node *node;

    if (consume("+")) {
        node = postfix();
    } else if (consume("-")) {
        node = new_node(ND_SUB, new_node_num(0), postfix());
    } else if (consume("&")) {
        node = new_node(ND_ADDR, unary(), NULL);
    } else if (consume("*")) {
        node = new_node(ND_DEREF, unary(), NULL);
    } else if (consume("sizeof")) {
        Node *tmp = unary();
        if (!tmp) error_at(token->str, "sizeofに値が指定されていません");
        node = new_node(ND_SIZEOF, tmp, NULL);
    } else {
        node = postfix();
    }

    copy_code(node, node_start);
    return node;
}

static Node *postfix() {
    char *node_start = token->str;
    Node *node = primary();

    while (consume("[")) {
        Node *idx = expr();
        expect("]");

        Node *add_node = new_node(ND_ADD, node, idx);
        node = new_node(ND_DEREF, add_node, NULL);
    }

    copy_code(node, node_start);
    return node;
}

Node *primary() {
    char *node_start = token->str;
    Token *tok = NULL;

    tok = consume_literal();
    if (tok) {
        // 文字列リテラルはグローバル変数として実装
        // ドット入りの名前を付けているので意図的に上書きは不可能
        // 宣言した箇所でだけ "hello" = "world" のように書けるが、文字列リテラルは配列なので結局上書きはされない
        Node *node = new_node_var(new_string_literal(tok->str + 1, tok->len - 2)); // ダブルクオート外し
        copy_code(node, node_start);
        return node;
    }

    // 次のトークンが"("なら、"(" expr ")"のはず
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        copy_code(node, node_start);
        return node;
    }

    tok = consume_ident();
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

            copy_code(node, node_start);
            return node;
        }

        Var *var = find_var(tok);
        if (!var) error_at(tok->str, "定義されていない変数を利用しています");

        // 変数の処理
        Node *node = new_node_var(var);

        copy_code(node, node_start);
        return node;
    }

    // そうでなければ数値のはず
    return new_node_num(expect_number());
}

// 変数を名前で検索する。見つからなかった場合はNULLを返す。
Var *find_var(Token *tok) {
    for (Var *var = locals; var; var = var->next)
        if (strlen(var->name) == tok->len && !memcmp(tok->str, var->name, strlen(var->name)))
            return var;

    for (Var *var = globals; var; var = var->next)
        if (strlen(var->name) == tok->len && !memcmp(tok->str, var->name, strlen(var->name)))
            return var;

    return NULL;
}
