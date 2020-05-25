#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Type Type;
typedef struct Token Token;
typedef struct Var Var;
typedef struct Node Node;
typedef struct Function Function;

// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_IDENT,    // 識別子
    TK_NUM,      // 整数トークン
    TK_EOF       // 入力の終わりを表すトークン
} TokenKind;

// 抽象構文木のノードの種類
typedef enum {
    // statement
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_RETURN,
    ND_BLOCK,
    ND_LVAR_DEF,

    // expression
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_ASSIGN, // 代入 =

    ND_EQ,  // ==
    ND_NE,  // !=
    ND_LE,  // <=
    ND_LT,  // <
    ND_NUM, // 整数

    ND_VAR,   // ローカル変数
    ND_ADDR,
    ND_DEREF,
    ND_FUNC_CALL,

    ND_SIZEOF // sizeof add_type()で置き換えるのでcodegenでは出てこないはず
} NodeKind;

// トークン型
struct Token {
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合、その数値
    char *str;      // トークン文字列
    int len;        // トークンの長さ
};

// 変数の型
struct Var {
    Var *next;     // 次の変数かNULL
    char *name;    // 変数の名前
    Type *ty;      // 型
    int offset;    // RBPからのオフセット
    bool is_local;
};

// 抽象構文木のノードの型
struct Node {
    NodeKind kind; // ノードの型
    Node *next;   // リストの次のノード

    Node *lhs;     // 左辺
    Node *rhs;     // 右辺
    int val;       // kindがND_NUMの場合のみ使う
    int offset;    // kindがND_LVARの場合のみ使う
    Var *var;

    // if, while, for用
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;

    Node *body;   // ステートメントのリスト

    char *func_name;
    Node *args;   // 引数のリスト

    Type *ty;

    char *code;   // 該当箇所のコード
};

struct Function {
    Function *next;
    char *name;
    Node *block;  // ND_BLOCKのNodeへのポインタ
    Var *locals; // ローカル変数のリスト（引数含む）
    Var *args;   // 引数のリスト
    Type *ret_ty; // 戻り値の型
};


typedef struct {
    Var *globals;
    Function *functions;
} Program;

typedef enum {
    TY_CHAR,
    TY_INT,
    TY_PTR,
    TY_ARRAY
} TypeKind;

struct Type {
    TypeKind kind;

    Type *base;

    int array_length;
};


// parse.c
Program *program();

void error(char *fmt, ...);

void error_at(char *loc, char *fmt, ...);

void codegen(Program *pg);

Token *tokenize(char *p);

// type.c
void add_type(Function *prog);
Type *array_of(Type *base, int length);

// codegen.c
int size_of(Type *type);

/**
 * グローバル変数
 */
// 現在着目しているトークン
extern Token *token;

// 入力プログラム
extern char *user_input;
