#include <stdio.h>
#include <stdint.h>

int main() {
    int64_t a = 10 + 3;
    int64_t b = 10 * 3;
    int64_t c = 10 + 2 * 3;
    int64_t d = 2 * (10 + 3);
    int64_t e = 10 + 3 * 2 + 9 / 3 - 23 % (10 - 2) + 10 * 2 / 5;
    int64_t f = a + b - 10;
    int64_t g = 4 | 3;
    int64_t h = 7 & 2;

    printf("%ld\n", a);
    printf("%ld\n", b);
    printf("%ld\n", c);
    printf("%ld\n", d);
    printf("%ld\n", e);
    printf("%ld\n", f);
    printf("%ld\n", g);
    printf("%ld\n", h);
    return 0;
}
