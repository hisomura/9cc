#include "9cc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        error_at(token->str, "引数の個数が正しくありません");
        return 1;
    }

    // トークナイズしてパースする
    user_input = argv[1];
    token = tokenize(user_input);
    Program *pg = program();
    add_type(pg->functions);

    // オフセットの決定
    for (Function *fn = pg->functions; fn; fn = fn->next) {
        int offset = 0;
        for (Var *var = fn->locals; var; var = var->next) {
            offset += var->ty->size;
            var->offset = offset;
        }
    }

    codegen(pg);

    return 0;
}
