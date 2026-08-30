#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

// Definimos los estados del autómata para el sumador
typedef enum {
    STATE_START,   // Estado inicial, aca limpiamos espacios y vemos con qué arranca el numero
    STATE_SIGN,    // Leimos un + o un - y ahora obligatoriamente necesitamos un dígito
    STATE_DIGITS,  // Estado de acumulacion: procesamos los numeros secuencialmente
    STATE_SUCCESS, // Estado de aceptacion: encontramos un numero valido completo
    STATE_FAIL     // Estado de rechazo: algo se rompio o no es un numero valido
} State;

bool GetNextDecimal(FILE* flujo, int *numero_leido) {
    State estado_actual = STATE_START;
    int caracter;
    int valor_acumulado = 0;
    int signo = 1;

    while (estado_actual != STATE_SUCCESS && estado_actual != STATE_FAIL) {
        caracter = fgetc(flujo);
        switch (estado_actual) {
            case STATE_START:
                if (caracter == EOF) {
                    estado_actual = STATE_FAIL;
                } else if (isspace(caracter)) {
                   // Ignoramos los espacios molestos
                } else if (caracter == '+' || caracter == '-') {
                    signo = (caracter == '-') ? -1 : 1;
                    estado_actual = STATE_SIGN;
                } else if (caracter >= '1' && caracter <= '9') { // Comprobación de rango manual
                    valor_acumulado = caracter - '0';
                    estado_actual = STATE_DIGITS;
                } else {
                    ungetc(caracter, flujo); // Devolvemos el caracter no numérico al flujo
                    estado_actual = STATE_FAIL;
                }
                break;

            case STATE_SIGN:
                if (caracter >= '1' && caracter <= '9') {
                    valor_acumulado = caracter - '0';
                    estado_actual = STATE_DIGITS;
                } else if (caracter == EOF) {
                    estado_actual = STATE_FAIL;
                } else {
                    ungetc(caracter, flujo); // Devolvemos el intruso que vino después del signo
                    estado_actual = STATE_FAIL;
                }
                break;

            case STATE_DIGITS:
                if (caracter >= '0' && caracter <= '9') {
                    valor_acumulado = valor_acumulado * 10 + (caracter - '0');
                } else if (caracter == EOF) {
                    estado_actual = STATE_SUCCESS;
                } else {
                    ungetc(caracter, flujo); // Devolver el intruso que frena la acumulación
                    estado_actual = STATE_SUCCESS;
                }
                break;

            default:
                estado_actual = STATE_FAIL;
                break;
        }
    }

    if (estado_actual == STATE_SUCCESS) {
        *numero_leido = signo * valor_acumulado;
        return true;
    } else {
        return false;
    }
}

int main(void) {
    int suma = 0;
    int valor;

    // Bucle para procesar y consumir nuestro autómata
    while (GetNextDecimal(stdin, &valor)) {
        suma += valor;
        printf("Suma parcial (Decimales): %d\n", suma);
    }

    printf("Suma total final: %d\n", suma);
    return 0;
}