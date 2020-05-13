#include "9cc.h"

int is_alnum(char c) {
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') ||
           (c == '_');
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

static bool startswith(char *p, char *q) {
    return strncmp(p, q, strlen(q)) == 0;
}


// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p++, 2);
            p++;
            continue;
        }

        if (ispunct(*p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (isdigit(*p)) {
            char *old_p = p;
            int val = strtol(p, &p, 10);
            cur = new_token(TK_NUM, cur, old_p, p - old_p);
            cur->val = val;
            continue;
        }

        if ('a' <= *p && *p <= 'z') {
            if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
                cur = new_token(TK_RETURN, cur, p, 6);
                p += 6;
                continue;
            }
            if (cur->kind == TK_IDENT) {
                cur->len += 1;
                p++;
                continue;
            }
            cur = new_token(TK_IDENT, cur, p++, 1);
            continue;
        }
        error_at(token->str, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}
