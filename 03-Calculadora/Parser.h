#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "Scanner.h"
typedef void (*ParserSentenceHandler)(const char *sentencia);

/* Entrega cada sentencia sintacticamente valida en forma normalizada.
 * La cadena solo es valida durante la llamada al handler. Devuelve false
 * al encontrar un error sintactico/lexico o si no puede reservar memoria. */
bool ParserRun(ParserSentenceHandler handler);

#endif
