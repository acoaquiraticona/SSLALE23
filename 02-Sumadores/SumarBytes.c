#include <stdio.h>

//sumador de bytes
int main(void) {
    signed char suma = 0;
    for (signed char valor; scanf("%hhd", &valor) == 1;) {
        suma += valor;
        printf("Suma parcial (bytes): %d\n",suma);
    }
    return 0;
}