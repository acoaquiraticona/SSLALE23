#include <stdio.h>
#include "Strings.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 0;   // No hay argumentos
    }

    char *longest = argv[1];

    for (char **arg = argv + 2; *arg != NULL; arg++) {
        if (GetLength(*arg) > GetLength(longest)) {
            longest = *arg;
        }
    }

    printf("%s\n", longest);
    return 0;
}