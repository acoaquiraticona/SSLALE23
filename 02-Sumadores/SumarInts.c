#include <stdio.h>
#include <limits.h>

int main(void) {
    int suma = 0;

    
    for (int valor; fread(&valor, sizeof(int), 1, stdin) == 1;) {
        if ((valor > 0 && suma > INT_MAX - valor) || 
            (valor < 0 && suma < INT_MIN + valor)) {
            fprintf(stderr, "Error: Overflow detectado. Suma se detiene en: %d\n", suma);
            break;
        }
        
        suma += valor;
        printf("Suma =: %d\n", suma);
    }
    
    return 0;
}
