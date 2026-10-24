#include "Parser.h"

static void RecibirSentencia(const char *sentencia) {
    (void)sentencia;
}

int main(void) {
    return ParserRun(RecibirSentencia) ? 0 : 1;
}