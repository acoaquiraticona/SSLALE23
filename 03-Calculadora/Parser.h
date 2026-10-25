/* PROBLEMA: el retorno booleano y el callback solo cubrian reconocimiento sintactico.
 * Se conserva el codigo original sin cambios. La version completa esta debajo. */
#if 0
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

#endif

#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>
#include "Scanner.h"
/* 0: EOF correcto; 1: EOF con rechazos; 2: error fatal. Tabla nueva por sesion. */
int ParserRun(bool trace);
#endif
