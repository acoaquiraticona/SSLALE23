#include <stdio.h>
#include "Strings.h"

int main(int argc, char *argv[]) {
    for (char **arg = argv + 1; *arg != NULL; arg++)
        printf("%s\n", *arg);
    
    return 0;
}