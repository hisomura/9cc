#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_IDENT,    // 識別子
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合、その数値
    char *str;      // トークン文字列
    int len;        // トークンの長さ
};

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
    LVar *next; // 次の変数かNULL
    char *name; // 変数の名前
    int len;    // 名前の長さ
    int offset; // RBPからのオフセット
};

// 抽象構文木のノードの種類
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // 整数
    ND_EQ,  // ==
    ND_NE,  // !=
    ND_LE,  // <=
    ND_LT,  // <
    ND_ASSIGN, // 代入 =
    ND_LVAR,   // ローカル変数
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BLOCK,
    ND_FUNC_CALL
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind; // ノードの型
    Node *next;   // リストの次のノード

    Node *lhs;     // 左辺
    Node *rhs;     // 右辺
    int val;       // kindがND_NUMの場合のみ使う
    int offset;    // kindがND_LVARの場合のみ使う

    // if, while, for用
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;

    Node *body;   // ステートメントのリスト

    char *func_name;
    Node *args;   // 引数のリスト
};

typedef struct Function Function;

struct Function {
    Function *next;
    char *name;
    Node *block; // ND_BLOCKのNodeへのポインタ
    LVar *locals; // ローカル変数のリスト
    int stack_size;
};

// parse.c
Function *function() ;

void program();

Node *stmt();

Node *expr();

Node *assign();

Node *expr();

Node *equality();

Node *relational();

Node *add();

Node *mul();

Node *unary();

Node *primary();

LVar *find_lvar(Token *tok);

void error(char *fmt, ...);

void error_at(char *loc, char *fmt, ...);

void gen_expr(Node *node);

void gen_stmt(Node *node);

Token *tokenize(char *p);


/**
 * グローバル変数
 */
// 現在着目しているトークン
extern Token *token;

// 入力プログラム
extern char *user_input;

extern Node *code[100];

// ローカル変数
extern LVar *locals;
