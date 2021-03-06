#include "9cc.h"

typedef struct VarScope VarScope;
struct VarScope {
    VarScope *next;
    int depth;
    char *name;
    Var *var;
};

// ローカル変数
static Var *locals;
static Var *globals;

static VarScope *var_scope; // 最後に着目したスコープ
static int scope_depth;     // 現在のスコープの深さ

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

static void enter_scope(void) {
    scope_depth++;
}

static void leave_scope(void) {
    scope_depth--;
    while (var_scope && var_scope->depth > scope_depth)
        var_scope = var_scope->next;
}

static VarScope *push_scope(char *name, Var *var) {
    VarScope *sc = calloc(1, sizeof(VarScope));
    sc->next = var_scope;
    sc->name = name;
    sc->var = var;
    sc->depth = scope_depth;
    var_scope = sc;
    return sc;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_binary(NodeKind kind, Node *lhs, Node *rhs, Token *start, Token *end) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    node->token_start = start;
    node->token_end = end;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    node->ty = new_type(TY_INT);
    return node;
}

Node *new_node_var(Var *var) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_VAR;
    node->var = var;
    node->ty = var->ty;
    return node;
}

static Var *new_local_var(char *name, Type *ty) {
    Var *var = calloc(1, sizeof(Var));
    var->name = name;
    var->ty = ty;
    var->is_local = true;
    var->next = locals;
    locals = var;
    push_scope(name, var);

    return var;
}

static Var *new_global_var(char *name, Type *ty) {
    Var *var = calloc(1, sizeof(Var));
    var->name = name;
    var->ty = ty;
    var->is_local = false;
    var->next = globals;
    globals = var;
    push_scope(name, var);

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
    Var *var = new_global_var(new_global_var_name(), ty);
    var->init_data = strndup(p, len); // \n追加もやってくれる
    return var;
}

Token *move_token_forward() {
    prev_token = token;
    token = token->next;
    return prev_token;
}

Token *consume(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return NULL;
    return move_token_forward();
}

Token *consume_ident() {
    if (token->kind != TK_IDENT)
        return 0;
    return move_token_forward();
}

Token *consume_literal() {
    if (token->kind != TK_STR)
        return false;
    return move_token_forward();
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
    move_token_forward();
}

Token *expect_ident() {
    if (token->kind != TK_IDENT)
        file_error_at(token->str, "識別子ではありません");
    return move_token_forward();
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
    if (token->kind != TK_NUM)
        file_error_at(token->str, "数ではありません");
    int val = token->val;
    move_token_forward();
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

    enter_scope();
    expect("(");
    while (!consume(")")) {
        Type *arg_type = basetype();
        Token *ident = consume_ident();
        new_local_var(strndup(ident->str, ident->len), arg_type);
        consume(",");
    }
    Var *args = locals;

    assert_token("{");
    Node *block = stmt();
    if (block->kind != ND_BLOCK) {
        file_error_at(token->str, "関数の中身が得られませんでした");
    }

    func = calloc(1, sizeof(Function));
    func->name = strndup(tok->str, tok->len);
    func->block = block;
    func->locals = locals;
    func->args = args;
    func->ret_ty = ret_type;
    // 引数は右に、変数は左に伸びるのでargsからnextをたどれば引数だけ取得できる
    // localsからnextをたどるとローカル変数と引数の両方を取得できる

    leave_scope();

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
    scope_depth = 0;
    var_scope = NULL;

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
        new_global_var(strndup(tok->str, tok->len), ty);
    }

    Program *pg = calloc(1, sizeof(Program));
    pg->functions = head.next;
    pg->globals = globals;

    return pg;
}

// compound-stmt = (declaration | stmt)* "}"
static Node *compound_stmt() {
    Node *node;
    Node head = {};
    Node *cur = &head;
    enter_scope();
    while (token && !token_is("}")) {
        cur = cur->next = stmt();
    }
    leave_scope();
    node = new_node(ND_BLOCK, NULL, NULL);
    node->body = head.next;

    return node;
}

Node *stmt() {
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

    // BLOCK
    if (consume("{")) {
        node = compound_stmt();
        expect("}");

        return node;
    }

    Type *ty = basetype();
    if (ty) { // 変数定義
        Token *ident = expect_ident();
        Var *lvar = new_local_var(strndup(ident->str, ident->len), type_suffix(ty));

        if (consume("=")) {
            node = new_node(ND_EXPR_STMT, new_node(ND_ASSIGN, new_node_var(lvar), assign()), NULL);
        } else {
            node = new_node(ND_LVAR_DEF, NULL, NULL); // 不要だと思うけどnullにすると他が面倒なので残す
        }
        expect(";");

        return node;
    }

    if (consume("return")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
    } else {
        node = new_node(ND_EXPR_STMT, expr(), NULL);
    }
    if (!consume(";"))
        file_error_at(token->str, "';'ではないトークンです");

    return node;
}

Node *expr() {
    Node *node = assign();
    return node;
}


Node *assign() {
    Token *start = token;
    Node *node = equality();
    if (consume("="))
        node = new_node_binary(ND_ASSIGN, node, assign(), start, prev_token);
    return node;
}

Node *equality() {

    Token *start = token;
    Node *node = relational();

    for (;;) {
        if (consume("=="))
            node = new_node_binary(ND_EQ, node, relational(), start, prev_token);
        else if (consume("!="))
            node = new_node_binary(ND_NE, node, relational(), start, prev_token);
        else {
            return node;
        }
    }
}

Node *relational() {
    Token *start = token;
    Node *node = add();
    for (;;) {
        if (consume("<="))
            node = new_node_binary(ND_LE, node, add(), start, prev_token);
        else if (consume(">="))
            node = new_node_binary(ND_LE, add(), node, start, prev_token);
        else if (consume("<"))
            node = new_node_binary(ND_LT, node, add(), start, prev_token);
        else if (consume(">"))
            node = new_node_binary(ND_LT, add(), node, start, prev_token);
        else
            return node;
    }
}

Node *add() {
    Token *start = token;
    Node *node = mul();

    for (;;) {
        if (consume("+"))
            node = new_node_binary(ND_ADD, node, mul(), start, prev_token);
        else if (consume("-"))
            node = new_node_binary(ND_SUB, node, mul(), start, prev_token);
        else {
            return node;
        }
    }
}

Node *mul() {
    Token *start = token;
    Node *node = unary();

    for (;;) {
        if ((start = consume("*"))) {
            node = new_node_binary(ND_MUL, node, unary(), start, prev_token);
        } else if ((start = consume("/"))) {
            node = new_node_binary(ND_DIV, node, unary(), start, prev_token);
        } else if ((start = consume("%"))) {
            node = new_node_binary(ND_MOD, node, unary(), start, prev_token);
        } else {
            return node;
        }
    }
}

Node *unary() {
    Token *start = token;
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
        if (!tmp) file_error_at(token->str, "sizeofに値が指定されていません");
        node = new_node(ND_SIZEOF, tmp, NULL);
    } else {
        node = postfix();
    }

    return node;
}

static Node *postfix() {
    Token *start = token;
    Node *node = primary();

    while (consume("[")) {
        Node *idx = expr();
        expect("]");

        Node *add_node = new_node(ND_ADD, node, idx);
        node = new_node(ND_DEREF, add_node, NULL);
    }

    return node;
}

Node *primary() {
    Token *start = token;
    Token *tok = NULL;

    tok = consume_literal();
    if (tok) {
        // 文字列リテラルはグローバル変数として実装
        // ドット入りの名前を付けているので意図的に上書きは不可能
        // 宣言した箇所でだけ "hello" = "world" のように書けるが、文字列リテラルは配列なので結局上書きはされない
        Node *node = new_node_var(new_string_literal(tok->str + 1, tok->len - 2)); // ダブルクオート外し
        return node;
    }

    // 次のトークンが"("なら、"(" expr ")" または"(" compound_statement ")" のはず
    if (consume("(")) {
        if (consume("{")) {
            Node *node = compound_stmt();
            expect("}");
            expect(")");

            node->kind = ND_STMT_EXPR;

            // 評価する値があるか確認
            Node *cur = node->body;
            while (cur->next)
                cur = cur->next;
            if (cur->kind != ND_EXPR_STMT)
                file_error_at(token->str, "statement expression が返すための値が存在しない");

            return node;
        }

        Node *node = expr();
        expect(")");
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

            return node;
        }

        Var *var = find_var(tok);
        if (!var) file_error_at(tok->str, "定義されていない変数を利用しています");

        // 変数の処理
        Node *node = new_node_var(var);

        return node;
    }

    // そうでなければ数値のはず
    return new_node_num(expect_number());
}

// 変数を名前で検索する。見つからなかった場合はNULLを返す。
Var *find_var(Token *tok) {
    for (VarScope *scope = var_scope; scope; scope = scope->next)
        if (strlen(scope->name) == tok->len && !memcmp(tok->str, scope->name, strlen(scope->name)))
            return scope->var;

    return NULL;
}
