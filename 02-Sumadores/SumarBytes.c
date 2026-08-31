#include <stdio.h>
#include <limits.h>

//sumador de bytes
int main(void) {

    signed char suma = 0;   // Acumulador de la suma de bytes

    for (signed char valor; scanf("%hhd", &valor) == 1;) {
        if (valor >= SCHAR_MAX || valor <= SCHAR_MIN) {
            fprintf(stderr, "Error: Valor fuera del rango de un byte. Valor ingresado: %d\n", valor);
            continue;
        }
        if ((valor > 0 && suma > SCHAR_MAX - valor) || 
            (valor < 0 && suma < SCHAR_MIN + valor)) {
            fprintf(stderr, "Error: Overflow detectado. Suma se detiene en: %d\n", suma);
            break;
        }
        suma += valor;
        printf("Suma parcial (bytes): %d\n",suma);
    }

    printf("Suma final (bytes): %d\n", suma);
    
    return 0;
}