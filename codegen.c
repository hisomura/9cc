#include "9cc.h"

static int labelSeq = 0;
static char *arg_reg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

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
            printf("  mov [rax], rdi\n");
            printf("  push rdi\n");
            printf("# end assign\n");
            return;
        case ND_FUNC_CALL: {
            // 引数の数だけスタックに値を積む
            int arg_count = 0;
            for (Node *arg = node->args; arg; arg = arg->next) {
                arg_count += 1;
                gen(arg);
            }
            for (int i = 0; i < arg_count; i++) {
                printf("  pop %s\n", arg_reg[arg_count - i - 1]);
            }

            // rspを16の倍数にするための処理 popやpushが8バイト事のアドレスの移動なので8で割り切れないことは無い想定
            int seq = labelSeq++;
            // 分岐
            printf("  # call %s\n", node->func_name);
            printf("  mov rax, rsp\n");
            printf("  and rax, 15\n");
            printf("  jnz .L.call.%d\n", seq);

            // 16で割り切れるとき
            printf("  mov rax, 0\n");
            printf("  call %s\n", node->func_name);
            printf("  jmp .L.end.%d\n", seq);

            // 割り切れないとき
            printf(".L.call.%d:\n", seq);
            printf("  sub rsp, 8\n");
            printf("  mov rax, 0\n");
            printf("  call %s\n", node->func_name);
            printf("  add rsp, 8\n"); // 元に戻す

            printf(".L.end.%d:\n", seq);
            printf("  push rax\n"); // raxに入ってる返り値をスタックに積む
            printf("  # end call %s\n", node->func_name);
            return;
        }
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
                printf("  je .L.else.if.%d\n", seq);
                gen_stmt(node->then);
                printf("  jmp .L.end.if.%d\n", seq);
                printf(".L.else.if.%d:\n", seq);
                gen_stmt(node->els);
            } else {
                printf("  je .L.end.if.%d\n", seq);
                gen_stmt(node->then);
            }
            printf(".L.end.if.%d:\n", seq);
            printf("# end if %d \n", seq);
            return;
        }
        case ND_WHILE: {
            int seq = labelSeq++;
            printf("# while %d \n", seq);
            printf(".L.begin.while.%d:\n", seq);
            gen(node->cond);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .L.end.while.%d\n", seq);
            gen_stmt(node->then);

            printf("  jmp .L.begin.while.%d\n", seq);
            printf(".L.end.while.%d:\n", seq);
            printf("# end while %d \n", seq);
            return;
        }
        case ND_FOR: {
            int seq = labelSeq++;
            printf("# for %d \n", seq);
            if (node->init) {
                gen(node->init);
                printf("  pop rax\n"); // スタック消費
            }
            printf(".L.begin.for.%d:\n", seq);
            if (node->cond) {
                gen(node->cond);
                printf("  pop rax\n");
                printf("  cmp rax, 0\n");
                printf("  je .L.end.for.%d\n", seq);
            }
            gen_stmt(node->then);
            if (node->inc) {
                gen(node->inc);
                printf("  pop rax\n"); // スタック消費
            }
            printf("  jmp .L.begin.for.%d\n", seq);
            printf(".L.end.for.%d:\n", seq);
            printf("# end for %d \n", seq);
            return;
        }
        case ND_BLOCK: {
            for (Node *n = node->body; n; n = n->next)
                gen_stmt(n);
            return;
        }
        default:
            gen(node);
            printf("  pop rax\n"); // スタック溢れ防止のポップ
    }
}
