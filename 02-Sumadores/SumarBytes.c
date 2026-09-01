#include <stdio.h>
#include <limits.h>

// sumador de bytes
int main(void) {
    unsigned char suma = 0; // Acumulador de la suma de bytes
    int valor;

    for ( valor = getchar(); valor != EOF; valor = getchar()) {
        unsigned char byte = (unsigned char)valor;

        if (valor == '\n')
            continue;
        if (suma > UCHAR_MAX - byte) {
            fprintf(stderr, "Error: Overflow detectado. Suma se detiene en: %u\n", (unsigned int)suma);
            break;
        }

        suma += byte;
        printf("Suma parcial (bytes): %u\n", (unsigned int)suma);
    }

    printf("Suma final (bytes): %u\n", (unsigned int)suma);

    return 0;
}