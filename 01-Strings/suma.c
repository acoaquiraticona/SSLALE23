#include <stdio.h>
#include <stdlib.h> 

int main(int, char *argv[]) {
    long suma = 0;
    for (char **arg = argv + 1; *arg != NULL; arg++) {     
        suma += strtol(*arg, NULL, 0);
    }
    printf("%ld\n", suma);

    return 0;
}