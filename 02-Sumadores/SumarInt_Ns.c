#include <stdio.h>
#include <stdint.h>

// N debe ser multiplo de 8, ya que asi lo exige _BitInt()

// Si N no esta definido de antemano, se define como 32
#ifndef N
#define N 32
#endif

typedef _BitInt(N) intN_t;
 
int main(void) {
    int64_t suma = 0;
 
    for (intN_t valor; fread(&valor, sizeof(intN_t), 1, stdin) == 1;) {
        int64_t v = (int64_t)valor; // guardamos el valor en un int64_t
 
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