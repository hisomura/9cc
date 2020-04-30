#include "9cc.h"

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p)
{
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p)
    {
        // 空白文字をスキップ
        if (isspace(*p))
        {
            p++;
            continue;
        }

        if (!memcmp("==", p, 2) || !memcmp("!=", p, 2) || !memcmp("<=", p, 2) || !memcmp(">=", p, 2))
        {
            // FIXME
            cur = new_token(TK_RESERVED, cur, p++, 2);
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>')
        {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (isdigit(*p))
        {
            char* old_p = p;
            int val = strtol(p, &p, 10);
            cur = new_token(TK_NUM, cur, old_p, p - old_p);
            cur->val = val;
            continue;
        }

        error_at(token->str, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}