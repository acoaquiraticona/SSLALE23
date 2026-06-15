#include <stdio.h>
#include "Strings.h"

int main(int, char *argv[]) {
    long suma = 0;
    for (char **arg = argv + 1; *arg != NULL; arg++) {     
        const char *numero = *arg;
        long signo = 1;

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
    printf("%ld\n", suma);

    return 0;
}