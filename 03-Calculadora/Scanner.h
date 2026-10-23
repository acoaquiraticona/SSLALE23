
#ifndef SCANNER_H
#define SCANNER_H

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

    char lexema[128];
    char codigo_error[32];
    char detalle[256];
} Token;

Token GetNextToken(void);

#endif
