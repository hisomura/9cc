#include "9cc.h"

static int label_seq = 0;
static char *arg_reg64[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
static char *arg_reg32[] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
static char *arg_reg8[] = {"dil", "sil", "dl", "cl", "r8b", "r9b"};

Function *current_fn;

void gen_expr(Node *node);

void gen_stmt(Node *node);

void gen_addr(Node *node) {
    if (node->code) printf("# code addr: %s \n", node->code);

    if (node->kind == ND_DEREF) {
        gen_expr(node->lhs);
        return;
    }

    if (node->var->is_local) {
        printf("  lea rax, [rbp-%d]\n", node->var->offset);
    } else {
        printf("  mov rax, offset %s\n", node->var->name);
    }
    printf("  push rax\n");
}

/**
 *  スタックの先頭にあるアドレスからtyのサイズに応じてデータを読む。tyが配列の場合は何も読まない。
 * @param ty
 */
void load(Type *ty) {
    // 変数が配列の時はレジスタに読み込めないので何もしない。結果的にアドレスが返る。
    if (ty->kind == TY_ARRAY) return;

    printf("  pop rax\n");
    switch (size_of(ty)) {
        case 1:
            // charの場合は正負不要だと思うのでmovzxを使う
            // レジスタがeaxでいいのかは要検討
            printf("  movzx eax, BYTE PTR [rax]\n");
            break;
        case 4:
            printf("  mov eax, [rax]\n");
            break;
        case 8:
            printf("  mov rax, [rax]\n");
            break;
    }
    printf("  push rax\n");
}

int size_of(Type *type) {
    switch (type->kind) {
        case TY_CHAR:
            return 1;
        case TY_INT:
            return 4;
        case TY_PTR:
            return 8;
        case TY_ARRAY:
            return size_of(type->base) * type->array_length;
        default:
            error("kindに不正な値が入っている");
    }
}

void gen_expr(Node *node) {
    if (node->code) {
        printf("# code expr: %s \n", node->code);
    }
    switch (node->kind) {
        case ND_NUM:
            printf("  push %d\n", node->val);
            return;
        case ND_VAR:
            gen_addr(node);
            load(node->ty);
            return;
        case ND_DEREF:
            gen_expr(node->lhs);
            load(node->ty);
            return;
        case ND_ADDR:
            gen_addr(node->lhs);
            return;
        case ND_ASSIGN:
            if (node->ty->kind == TY_ARRAY) error("配列への値の保存");

            gen_addr(node->lhs);
            gen_expr(node->rhs);

            printf("  pop rdi\n");
            printf("  pop rax\n");

            switch (size_of(node->ty)) {
                case 1:
                    printf("  mov [rax], dil\n");
                    break;
                case 4:
                    printf("  mov [rax], edi\n");
                    break;
                case 8:
                    printf("  mov [rax], rdi\n");
                    break;
            }
            printf("  push rdi\n");
            return;
        case ND_FUNC_CALL: {
            // 引数の数だけスタックに値を積む
            int arg_count = 0;
            for (Node *arg = node->args; arg; arg = arg->next) {
                arg_count += 1;
                gen_expr(arg);
            }
            for (int i = 0; i < arg_count; i++) {
                printf("  pop %s\n", arg_reg64[arg_count - i - 1]);
            }

            // rspを16の倍数にするための処理 popやpushが8バイト事のアドレスの移動なので8で割り切れないことは無い想定
            int seq = label_seq++;
            // 分岐
            printf("# call %s\n", node->func_name);
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
            printf("# end call %s\n", node->func_name);
            return;
        }
        default:;
    }

    gen_expr(node->lhs);
    gen_expr(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
        case ND_ADD: {
            if (node->ty->base)
                printf("  imul rdi, %d\n", size_of(node->ty->base));
            printf("  add rax, rdi\n");
            break;
        }
        case ND_SUB:
            if (node->ty->base)
                printf("  imul rdi, %d\n", size_of(node->ty->base));
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
    if (node->kind != ND_LVAR_DEF && node->code) {
        printf("# code stmt: %s \n", node->code);
    }
    switch (node->kind) {
        case ND_RETURN: {
            gen_expr(node->lhs);
            printf("  pop rax\n");
            printf("  jmp .L.return.%s\n", current_fn->name);
            return;
        }
        case ND_IF: {
            int seq = label_seq++;
            printf("# if %d \n", seq);
            gen_expr(node->cond);
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
            int seq = label_seq++;
            printf("# while %d \n", seq);
            printf(".L.begin.while.%d:\n", seq);
            gen_expr(node->cond);
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
            int seq = label_seq++;
            printf("# for %d \n", seq);
            if (node->init) {
                gen_expr(node->init);
                printf("  pop rax\n"); // スタック消費
            }
            printf(".L.begin.for.%d:\n", seq);
            if (node->cond) {
                gen_expr(node->cond);
                printf("  pop rax\n");
                printf("  cmp rax, 0\n");
                printf("  je .L.end.for.%d\n", seq);
            }
            gen_stmt(node->then);
            if (node->inc) {
                gen_expr(node->inc);
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
        case ND_LVAR_DEF:
            return;
        default:
            gen_expr(node);
            printf("  pop rax\n"); // スタック溢れ防止のポップ
    }
}

int locals_count(Function *func) {
    int count = 0;
    for (Var *var = func->locals; var; var = var->next) {
        count += 1;
    }

    return count;
}

int local_stack_size(Function *func) {
    int size = 0;
    for (Var *var = func->locals; var; var = var->next) {
        size += size_of(var->ty);
    }

    return size;
}

void codegen(Program *pg) {
    printf(".intel_syntax noprefix\n");

    printf(".data\n");
    for (Var *var = pg->globals; var; var = var->next) {
        printf("%s:\n", var->name);
        printf("  .zero %d\n", size_of(var->ty));
    }
    printf(".text\n");

    for (Function *func = pg->functions; func; func = func->next) {
        printf(".global %s\n", func->name);
        printf("%s:\n", func->name);
        current_fn = func;

        // プロローグ
        printf("  push rbp\n");
        printf("  mov rbp, rsp\n");

        // ローカル変数の領域確保
        printf("  sub rsp, %d\n", local_stack_size(func));
        int i = 0;
        for (Var *arg = func->args; arg; arg = arg->next) {
            switch (size_of(arg->ty)) {
                case 1:
                    printf("  mov [rbp-%d], %s\n", arg->offset, arg_reg8[i]);
                    break;
                case 4:
                    printf("  mov [rbp-%d], %s\n", arg->offset, arg_reg32[i]);
                    break;
                case 8:
                    printf("  mov [rbp-%d], %s\n", arg->offset, arg_reg64[i]);
                    break;
            }
            i += 1;
        }

        // 先頭の式から順にコード生成
        for (Node *st = func->block->body; st; st = st->next) {
            gen_stmt(st);
        }

        // エピローグ
        // 最後の式の結果がRAXに残っているのでそれが返り値になる
        printf(".L.return.%s:\n", func->name);
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
    }
}
