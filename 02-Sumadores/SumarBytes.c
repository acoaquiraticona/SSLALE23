#include <stdio.h>
#include <limits.h>
// SumarBytes.c
// Sumador de secuencia de bytes leidos desde stdin (Cap. 20 - Sumadores)
int main(void) {
    signed char suma = 0;   // Acumulador de la suma de bytes

    for (int valor; scanf("%d", &valor) == 1;) { // int en vez de signed char: %hhd truncaba antes de poder validar

        if (valor > SCHAR_MAX || valor < SCHAR_MIN) { // > y < (no >= ni <=): 127 y -128 son bytes válidos
            fprintf(stderr, "Error: Valor fuera del rango de un byte. Valor ingresado: %d\n", valor);
            continue;
        }

        if ((valor > 0 && suma > SCHAR_MAX - valor) || 
            (valor < 0 && suma < SCHAR_MIN + valor)) {
            fprintf(stderr, "Error: Overflow detectado. Suma se detiene en: %d\n", suma);
            break;
        }

        suma += (signed char)valor; // cast explícito: truncación ya validada como segura
        printf("Suma parcial (bytes): %d\n", suma);
    }
    printf("Suma final (bytes): %d\n", suma);
    
    return 0;
}
