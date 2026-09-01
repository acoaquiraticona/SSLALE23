#include <stdio.h>
#include <float.h>

int main (void) {
    double suma = 0;
    for (double valor; scanf("%lf", &valor) == 1;) {
        {
            if ((valor > 0 && suma > DBL_MAX - valor) ||
                (valor < 0 && suma < DBL_MIN + valor)) {
                    fprintf(stderr, "Error: Stack Overflow detectado (Como hiciste?). Suma se detiene en: %lf\n", suma);
                    break;
                }
        }
    suma += valor;
    printf("Suma parcial (decimales): %lf\n", suma); }
    return 0;
}