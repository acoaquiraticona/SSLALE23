#include <stdio.h>
#include <stdint.h>
 
typedef _BitInt(N) intN_t;
 
int main(void) {
    int64_t suma = 0;
 
    for (intN_t valor; fread(&valor, sizeof(intN_t), 1, stdin) == 1;) {
        int64_t v = (int64_t)valor;
 
        if ((v > 0 && suma > INT64_MAX - v) ||
            (v < 0 && suma < INT64_MIN - v)) {
            fprintf(stderr, "Error: Overflow detectado. Suma se detiene en: %lld\n", (long long)suma);
            break;
        }
 
        suma += v;
        printf("Suma =: %lld\n", (long long)suma);
    }
 
    return 0;
}