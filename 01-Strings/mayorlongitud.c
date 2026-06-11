#include <stdio.h>
#include "Strings.h"

int main (int argc, char *argv[]){
    char *longest = argv;
    for(char **arg = argv + 1; *arg != NULL; arg++){
        if(GetLength(*arg) > GetLength(*longest))
            longest = *arg;
    }
    printf("%s\n", longest);
    return 0;
}