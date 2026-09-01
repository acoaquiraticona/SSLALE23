/*
 * 
 *
 *   B=0 Bulbasaur   M=5 Meowth
 *   C=1 Charmander  E=6 Eevee
 *   S=2 Squirtle    G=7 Gengar
 *   P=3 Pikachu     O=8 Onix
 *   J=4 Jigglypuff  N=9 Ninetales
*/

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
static int PokeDigitValue(int c) {
    switch (toupper(c)) {
        case 'B': return 0;
        case 'C': return 1;
        case 'S': return 2;
        case 'P': return 3;
        case 'J': return 4;
        case 'M': return 5;
        case 'E': return 6;
        case 'G': return 7;
        case 'O': return 8;
        case 'N': return 9;
        default:  return -1;
    }
}


bool GetNextX(FILE *s, int *p) {
    int c;
    int signo = 1;
    long acumulado;
    int valor;

start:
    c = fgetc(s);
    if (c == EOF) return false;         
    if (isspace(c)) goto start;          

    if (c == '+' || c == '-') {
        signo = (c == '-') ? -1 : 1;
        goto sign_leido;
    }
    goto primer_digito;                

sign_leido:
    c = fgetc(s);
    goto primer_digito;

primer_digito:
    valor = PokeDigitValue(c);
    if (valor == -1) {
        if (c != EOF) ungetc(c, s);
        return false;
    }
    acumulado = valor;
    goto acumulando;

acumulando:
    c = fgetc(s);
    valor = PokeDigitValue(c);
    if (valor != -1) {
        if (acumulado > LONG_MAX / 10 ||
            (acumulado == LONG_MAX / 10 && valor > LONG_MAX % 10)) {
            fprintf(stderr, "SumarX: No caben los pokemones en la pokebola (Overflow D:)\n");
            return false;
        }
        acumulado = acumulado * 10 + valor;
        goto acumulando;
    }
    if (c != EOF) ungetc(c, s);
    *p = (int)(signo * acumulado);
    return true;
}

int main(void) {
    int numero;
    long total = 0;
    int cantidad = 0;

    while (GetNextX(stdin, &numero)) {
        if (numero > 0) {
            if (total > LONG_MAX - numero) {
                fprintf(stderr,
                        "SumarX: No caben los pokemones en la pokebola (Overflow D:)\n");
                return 1;
            }
        } else if (numero < 0) {
            if (total < LONG_MIN - numero) {
                fprintf(stderr,
                        "SumarX: No caben los pokemones en la pokebola (Overflow D:)\n");
                return 1;
            }
        }
        total += numero;
        cantidad++;
        printf("%d\t%ld\n", numero, total);
    }

    if (!feof(stdin)) {
        fprintf(stderr,
                "SumarX: entrada mal formada (token invalido), se detiene "
                "el procesamiento.\n");
        return 1;
    }

    printf("Total: %ld (sumando %d numero%s)\n",
           total, cantidad, cantidad == 1 ? "" : "s");
    return 0;
}
