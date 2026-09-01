#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>

bool GetNextDecimal(FILE *flujo, int *numero_leido) {
    int c;
    int signo = 1;
    unsigned long magnitud = 0;
    int digito;

start:
    c = fgetc(flujo);
    if (c == EOF) goto fail;
    if (isspace((unsigned char)c)) goto start;
    if (c == '+' || c == '-') {
        signo = (c == '-') ? -1 : 1;
        goto sign_leido;
    }
    if (c >= '1' && c <= '9') {
        magnitud = (unsigned long)(c - '0');
        goto acumulando;
    }
    if (c != EOF) ungetc(c, flujo);
    goto fail;

sign_leido:
    c = fgetc(flujo);
    if (c == EOF) goto fail;
    if (c >= '1' && c <= '9') {
        magnitud = (unsigned long)(c - '0');
        goto acumulando;
    }
    if (c != EOF) ungetc(c, flujo);
    goto fail;

acumulando:
    c = fgetc(flujo);
    if (c >= '0' && c <= '9') {
        digito = c - '0';
        if (magnitud > (ULONG_MAX - (unsigned long)digito) / 10UL) {
            fprintf(stderr, "Error: Overflow al leer el número\n");
            return false;
        }
        magnitud = magnitud * 10UL + (unsigned long)digito;
        goto acumulando;
    }
    if (c != EOF) ungetc(c, flujo);

    if (signo > 0) {
        if (magnitud > (unsigned long)INT_MAX) {
            fprintf(stderr, "Error: Overflow al leer el número\n");
            return false;
        }
        *numero_leido = (int)magnitud;
        return true;
    }

    if (magnitud > ((unsigned long)INT_MAX + 1UL)) {
        fprintf(stderr, "Error: Overflow al leer el número\n");
        return false;
    }

    *numero_leido = (int)(-(long)magnitud);
    return true;

fail:
    return false;
}

int main(void) {
    int suma = 0;
    int valor;

    while (GetNextDecimal(stdin, &valor)) {
        if (valor > 0 && suma > INT_MAX - valor) {
            fprintf(stderr, "Error: Overflow al sumar %d a la suma actual %d\n", valor, suma);
            return 1;
        }
        if (valor < 0 && suma < INT_MIN - valor) {
            fprintf(stderr, "Error: Overflow al sumar %d a la suma actual %d\n", valor, suma);
            return 1;
        }

        suma += valor;
        printf("Suma parcial (Decimales): %d\n", suma);
    }

    printf("Suma total final: %d\n", suma);
    return 0;
}