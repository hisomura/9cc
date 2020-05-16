#include "9cc.h"

static int labelSeq = 0;

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

void gen(Node *node) {
    switch (node->kind) {
        case ND_NUM:
            printf("  push %d\n", node->val);
            return;
        case ND_LVAR:
            gen_lval(node);
            printf("  pop rax\n");
            printf("  mov rax, [rax]\n");
            printf("  push rax\n");
            return;
        case ND_ASSIGN:
            printf("# start assign\n");
            gen_lval(node->lhs);
            gen(node->rhs);

            printf("  pop rdi\n");
            printf("  pop rax\n");
            printf("# hello, world\n");
            printf("  mov [rax], rdi\n");
            printf("  push rdi\n");
            printf("# end assign\n");
            return;
        default:;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
        case ND_ADD:
            printf("  add rax, rdi\n");
            break;
        case ND_SUB:
            printf("  sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("  imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("  cqo\n");
            printf("  idiv rdi\n");
            break;
        case ND_EQ:
            printf("  cmp rax, rdi\n");
            printf("  sete al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_NE:
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LE:
            printf("  cmp rax, rdi\n");
            printf("  setle al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LT:
            printf("  cmp rax, rdi\n");
            printf("  setl al\n");
            printf("  movzb rax, al\n");
            break;
        default:;
    }

    printf("  push rax\n");
}


void gen_stmt(Node *node) {
    switch (node->kind) {
        case ND_RETURN: {
            printf("# return \n");
            gen(node->lhs);
            printf("  pop rax\n");
            printf("  mov rsp, rbp\n");
            printf("  pop rbp\n");
            printf("  ret\n");
            printf("# end return \n");
            return;
        }
        case ND_IF: {
            int seq = labelSeq++;
            printf("# if %d \n", seq);
            gen(node->cond);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            if (node->els) {
                printf("  je .L.else.%d\n", seq);
                gen_stmt(node->then);
                printf("  jmp .L.end.%d\n", seq);
                printf(".L.else.%d:\n", seq);
                gen_stmt(node->els);
                printf(".L.end.%d:\n", seq);
            } else {
                printf("  je .L.end.%d\n", seq);
                gen_stmt(node->then);
                printf(".L.end.%d:\n", seq);
            }
            printf("# end if %d \n", seq);
            return;
        }
        case ND_WHILE: {
            int seq = labelSeq++;
            printf(".L.beginWhile.%d:\n", seq);
            gen(node->cond);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .L.endWhile.%d\n", seq);
            gen_stmt(node->then);

            printf("  jmp .L.beginWhile.%d\n", seq);
            printf(".L.endWhile.%d:\n", seq);
            return;
        }
        default:
            gen(node);
            printf("  pop rax\n"); // スタック溢れ防止のポップ
    }
}
