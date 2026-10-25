
#ifndef SCANNER_H
#define SCANNER_H

#include "errors.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    TOKEN_EOF,
    TOKEN_ERROR,
    TOKEN_NATURAL,
    TOKEN_IDENTIFIER,
    TOKEN_PLUSLE,
    TOKEN_MINUN,
    TOKEN_TANDEMAUS,
    /* Legado conservado: el scanner nunca emite este token; falinks es un ID. */
    TOKEN_FALINKS,
    TOKEN_DITTO,
    TOKEN_CAPTURA,
    TOKEN_LIBERAR,
    TOKEN_EQUAL,
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN
} TipoToken;

typedef struct {
    TipoToken tipo;
    int linea;
    int columna;

    int64_t valor;
    bool fuera_de_rango;

    /* Datos comunes; se conservan los campos de texto anteriores como espejos. */
    Diagnostic diagnostico;
    bool lexema_recortado;
    char lexema[128];
    char codigo_error[32];
    char detalle[256];
} Token;

/* Reinicia el estado privado, sin leer ni reposicionar stdin. */
void ScannerReset(void);
const char *ScannerTokenName(TipoToken tipo);
Token GetNextToken(void);

#endif
