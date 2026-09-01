#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
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
    long long valor_acumulado = 0; // long long: permite acumular sin overflow del propio acumulador
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
                } else if (caracter >= '0' && caracter <= '9') { // '0' incluido: "0" es decimal válido para scanf %d
                    valor_acumulado = caracter - '0';
                    estado_actual = STATE_DIGITS;
                } else {
                    ungetc(caracter, flujo); // Devolvemos el caracter no numérico al flujo
                    estado_actual = STATE_FAIL;
                }
                break;
            case STATE_SIGN:
                if (caracter >= '0' && caracter <= '9') { // '0' incluido: "+0"/"-0" son válidos
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
                    // Límite de magnitud según el signo: |INT_MIN| > INT_MAX, por eso no se puede
                    // acumular siempre en positivo y aplicar el signo al final sin distinguir esto
                    long long limite = (signo == 1) ? (long long)INT_MAX : -(long long)INT_MIN;
                    long long candidato = valor_acumulado * 10 + (caracter - '0'); // seguro en long long: valor_acumulado ya está acotado por 'limite'
                    if (candidato > limite) {
                        fprintf(stderr, "Error: Overflow al leer el número\n");
                        ungetc(caracter, flujo); // Evita perder el dígito que disparó el overflow
                        estado_actual = STATE_FAIL;
                        break;
                    }
                    valor_acumulado = candidato;
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
        *numero_leido = (int)(signo * valor_acumulado); // cast seguro: valor_acumulado ya validado dentro de rango int
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
        // Comparación sin sumar antes: "suma + valor" podía desbordar y el propio chequeo sería UB
        if ((valor > 0 && suma > INT_MAX - valor) ||
            (valor < 0 && suma < INT_MIN - valor)) {
            fprintf(stderr, "Error: Overflow al sumar %d a la suma actual %d\n", valor, suma);
            return 1;
        }
        suma += valor;
        printf("Suma parcial (Decimales): %d\n", suma);
    }
    printf("Suma total final: %d\n", suma);
    return 0;
}
