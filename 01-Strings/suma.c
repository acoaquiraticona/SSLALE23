#include <stdio.h>
#include "Conversion.h"

int main(int, char *argv[]) {
    int suma = 0;
    for (char **arg = argv + 1; *arg != NULL; arg++) {     
        const char *numero = *arg;
        int signo = 1;

        //Manejamos el signo
        if(*numero == '-'){
            signo = -1;
            numero++;
        } else if (*numero == '+'){
            numero++;
        }
        
        //Summamos solo argumentos y no signos
        if(!IsEmpty(numero) && AreDecimalDigits(numero)){
            suma += signo * ToInteger(numero);
        }
    }
    printf("%d\n", suma);

    return 0;
}