/**
 * %演算子の代わり
 * @param num
 * @param divider
 * @return
 */
int getMod(int num, int divider) {
    int quotient = num / divider;
    return num - quotient * divider;
}

/**
 * &&演算子の代わり
 * @param left
 * @param right
 * @return
 */
int getAnd(int left, int right) {
    if (right > 0) if (left > 0) return 1;
    return 0;
}

int main() {
    // 行コメントの確認コメントのテスト INT I = 1; int times3; int times5; for (i = 1; i <= 100; i = i + 1) { times3 = getMod(i, 3) == 0
    int i = 1;
    int times3;
    int times5;
    for (i = 1; i <= 100; i = i + 1) {
        times3 = getMod(i, 3) == 0;
        times5 = getMod(i, 5) == 0;
        if (getAnd(times3, times5)) {
            print("FizzBuzz");
        } else if (times3) {
            print("Fizz");
        } else if (times5) {
            print("Buzz");
        } else {
            printNum(i);
        }
    }

    return 0;
}
