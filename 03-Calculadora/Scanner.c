#include "Scanner.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define VALOR_MAXIMO INT64_C(999999)
#define LONGITUD_MAXIMA_IDENTIFICADOR 31
#define CAPACIDAD_LEXEMA 128

/* Estado privado del scanner: un byte de anticipacion y su posicion. */
static int caracterAnticipado = -2;
static int lineaActual = 1;
static int columnaActual = 1;
static int anteriorFueCR = 0;
static int finDeArchivoAlcanzado = 0;
static int errorLecturaInformado = 0;

static int MirarCaracter(void) {
    if (caracterAnticipado == -2) {
        if (finDeArchivoAlcanzado) {
            caracterAnticipado = EOF;
        } else {
            caracterAnticipado = getchar();
            if (caracterAnticipado == EOF) {
                finDeArchivoAlcanzado = 1;
            }
        }
    }
    return caracterAnticipado;
}

static int ConsumirCaracter(void) {
    int caracter = MirarCaracter();
    caracterAnticipado = -2;

    if (caracter == '\r') {
        lineaActual++;
        columnaActual = 1;
        anteriorFueCR = 1;
    } else if (caracter == '\n') {
        if (!anteriorFueCR) {
            lineaActual++;
        }
        columnaActual = 1;
        anteriorFueCR = 0;
    } else if (caracter != EOF) {
        columnaActual++;
        anteriorFueCR = 0;
    }

    return caracter;
}

static Token CrearToken(TipoToken tipo, int linea, int columna) {
    Token token = {0};
    token.tipo = tipo;
    token.linea = linea;
    token.columna = columna;
    return token;
}

static Token CrearError(const char *codigo, const char *detalle,
                        const char *lexema, int linea, int columna) {
    Token token = CrearToken(TOKEN_ERROR, linea, columna);
    snprintf(token.codigo_error, sizeof token.codigo_error, "%s", codigo);
    snprintf(token.detalle, sizeof token.detalle, "%s", detalle);
    snprintf(token.lexema, sizeof token.lexema, "%s", lexema);
    return token;
}

static void AgregarCaracter(char *cadena, size_t capacidad,
                            size_t *longitud, int caracter) {
    if (*longitud + 1 < capacidad) {
        cadena[*longitud] = (char)caracter;
        (*longitud)++;
        cadena[*longitud] = '\0';
    }
}

static int EsLetra(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int EsDigito(int c) {
    return c >= '0' && c <= '9';
}

static int EsInicioIdentificador(int c) {
    return EsLetra(c) || c == '_';
}

static int EsCaracterIdentificador(int c) {
    return EsInicioIdentificador(c) || EsDigito(c);
}

static int EsEspacioHorizontal(int c) {
    return c == ' ' || c == '\t';
}

static void OmitirEspaciosYComentarios(void) {
    int c;
    for (;;) {
        c = MirarCaracter();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            ConsumirCaracter();
        } else if (c == '#') {
            while ((c = MirarCaracter()) != EOF && c != '\r' && c != '\n') {
                ConsumirCaracter();
            }
        } else {
            return;
        }
    }
}

static TipoToken PalabraReservada(const char *palabra) {
    if (strcmp(palabra, "plusle") == 0) return TOKEN_PLUSLE;
    if (strcmp(palabra, "minun") == 0) return TOKEN_MINUN;
    if (strcmp(palabra, "tandemaus") == 0) return TOKEN_TANDEMAUS;
    if (strcmp(palabra, "falinks") == 0) return TOKEN_FALINKS;
    if (strcmp(palabra, "ditto") == 0) return TOKEN_DITTO;
    if (strcmp(palabra, "captura") == 0) return TOKEN_CAPTURA;
    if (strcmp(palabra, "liberar") == 0) return TOKEN_LIBERAR;
    return TOKEN_IDENTIFIER;
}

static int ObtenerDigitoPokemon(const char *palabra) {
    if (strcmp(palabra, "mew") == 0) return 0;
    if (strcmp(palabra, "bulbasaur") == 0) return 1;
    if (strcmp(palabra, "ivysaur") == 0) return 2;
    if (strcmp(palabra, "venusaur") == 0) return 3;
    if (strcmp(palabra, "squirtle") == 0) return 4;
    if (strcmp(palabra, "wartortle") == 0) return 5;
    if (strcmp(palabra, "blastoise") == 0) return 6;
    if (strcmp(palabra, "charmander") == 0) return 7;
    if (strcmp(palabra, "charmeleon") == 0) return 8;
    if (strcmp(palabra, "charizard") == 0) return 9;
    return -1;
}

static Token LeerPalabra(void) {
    char palabra[CAPACIDAD_LEXEMA] = "";
    size_t longitud = 0;
    int demasiadoLarga = 0;
    int lineaInicio = lineaActual;
    int columnaInicio = columnaActual;
    int c;
    TipoToken tipo;
    Token token;

    while (EsCaracterIdentificador(MirarCaracter())) {
        c = ConsumirCaracter();
        if (longitud < sizeof palabra - 1) {
            AgregarCaracter(palabra, sizeof palabra, &longitud, c);
        } else {
            demasiadoLarga = 1;
        }
    }

    if (demasiadoLarga || longitud > LONGITUD_MAXIMA_IDENTIFICADOR) {
        return CrearError("E_LIMIT", "El identificador supera los 31 caracteres.",
                          palabra, lineaInicio, columnaInicio);
    }

    tipo = PalabraReservada(palabra);
    token = CrearToken(tipo, lineaInicio, columnaInicio);
    snprintf(token.lexema, sizeof token.lexema, "%s", palabra);

    if (tipo == TOKEN_IDENTIFIER && ObtenerDigitoPokemon(palabra) >= 0) {
        return CrearError("E_MISSINGNO",
                          "Un digito Pokemon debe escribirse entre corchetes.",
                          palabra, lineaInicio, columnaInicio);
    }

    return token;
}

/* Lee un literal entre corchetes. En un error conserva ;, CR, LF o EOF. */
static Token LeerNumeroPokemon(void) {
    char lexema[CAPACIDAD_LEXEMA] = "";
    char palabra[CAPACIDAD_LEXEMA] = "";
    size_t longitudLexema = 0;
    size_t longitudPalabra;
    int lineaInicio = lineaActual;
    int columnaInicio = columnaActual;
    int c, digito;
    int cantidadDigitos = 0;
    int invalido = 0;
    int demasiadoLargo = 0;
    int cerrado = 0;
    int fueraDeRango = 0;
    int64_t valor = 0;

    c = ConsumirCaracter(); /* '[' */
    AgregarCaracter(lexema, sizeof lexema, &longitudLexema, c);

    while (1) {
        while (EsEspacioHorizontal(MirarCaracter())) {
            c = ConsumirCaracter();
            AgregarCaracter(lexema, sizeof lexema, &longitudLexema, c);
            if (longitudLexema >= sizeof lexema - 1) demasiadoLargo = 1;
        }

        c = MirarCaracter();
        if (c == ']') {
            c = ConsumirCaracter();
            AgregarCaracter(lexema, sizeof lexema, &longitudLexema, c);
            cerrado = 1;
            break;
        }

        /* El delimitador queda disponible para el siguiente token. */
        if (c == EOF || c == '\r' || c == '\n' || c == ';') {
            break;
        }

        longitudPalabra = 0;
        palabra[0] = '\0';
        while (EsLetra(MirarCaracter())) {
            c = ConsumirCaracter();
            AgregarCaracter(palabra, sizeof palabra, &longitudPalabra, c);
            AgregarCaracter(lexema, sizeof lexema, &longitudLexema, c);
            if (longitudLexema >= sizeof lexema - 1) demasiadoLargo = 1;
        }

        if (longitudPalabra == 0) {
            invalido = 1;
            /* Avanza sobre el byte invalido para evitar bucles infinitos. */
            c = MirarCaracter();
            if (c != EOF && c != '\r' && c != '\n' && c != ';' && c != ']') {
                c = ConsumirCaracter();
                AgregarCaracter(lexema, sizeof lexema, &longitudLexema, c);
                if (longitudLexema >= sizeof lexema - 1) demasiadoLargo = 1;
            }
            continue;
        }

        digito = ObtenerDigitoPokemon(palabra);
        if (digito < 0) {
            invalido = 1;
        } else {
            cantidadDigitos++;
            if (valor > (INT64_MAX - digito) / 10) {
                fueraDeRango = 1;
                valor = INT64_MAX;
            } else if (valor != INT64_MAX) {
                valor = valor * 10 + digito;
            }
            if (valor > VALOR_MAXIMO) fueraDeRango = 1;
        }

        c = MirarCaracter();
        if (!EsEspacioHorizontal(c) && c != ']' && c != EOF &&
            c != '\r' && c != '\n' && c != ';') {
            invalido = 1;
        }
    }

    if (!cerrado) {
        /* Consume el resto del literal mal formado, sin consumir el delimitador. */
        while ((c = MirarCaracter()) != EOF && c != '\r' && c != '\n' && c != ';') {
            c = ConsumirCaracter();
            AgregarCaracter(lexema, sizeof lexema, &longitudLexema, c);
            if (longitudLexema >= sizeof lexema - 1) demasiadoLargo = 1;
        }
        invalido = 1;
    }

    if (invalido || cantidadDigitos == 0 || demasiadoLargo) {
        return CrearError("E_MISSINGNO",
                          "Numero Pokemon mal formado; se esperaba una lista de digitos validos entre corchetes.",
                          lexema, lineaInicio, columnaInicio);
    }

    Token token = CrearToken(TOKEN_NATURAL, lineaInicio, columnaInicio);
    token.valor = valor;
    token.fuera_de_rango = fueraDeRango;
    snprintf(token.lexema, sizeof token.lexema, "%s", lexema);
    return token;
}

static Token LeerNumeroDecimal(void) {
    char numero[CAPACIDAD_LEXEMA] = "";
    size_t longitud = 0;
    int lineaInicio = lineaActual;
    int columnaInicio = columnaActual;
    int c;

    while (EsDigito(MirarCaracter())) {
        c = ConsumirCaracter();
        AgregarCaracter(numero, sizeof numero, &longitud, c);
    }

    return CrearError("E_MISSINGNO",
                      "La representacion numerica decimal no esta permitida; use nombres Pokemon entre corchetes.",
                      numero, lineaInicio, columnaInicio);
}

Token GetNextToken(void) {
    int c;
    int lineaInicio, columnaInicio;
    Token token;

    OmitirEspaciosYComentarios();
    c = MirarCaracter();
    lineaInicio = lineaActual;
    columnaInicio = columnaActual;

    if (c == EOF) {
        if (ferror(stdin) && !errorLecturaInformado) {
            errorLecturaInformado = 1;
            return CrearError("E_IO", "Fallo al leer la entrada estandar.", "",
                              lineaInicio, columnaInicio);
        }
        return CrearToken(TOKEN_EOF, lineaInicio, columnaInicio);
    }

    if (EsInicioIdentificador(c)) return LeerPalabra();
    if (c == '[') return LeerNumeroPokemon();
    if (EsDigito(c)) return LeerNumeroDecimal();

    switch (c) {
        case '=':
            ConsumirCaracter();
            token = CrearToken(TOKEN_EQUAL, lineaInicio, columnaInicio);
            strcpy(token.lexema, "=");
            return token;
        case ';':
            ConsumirCaracter();
            token = CrearToken(TOKEN_SEMICOLON, lineaInicio, columnaInicio);
            strcpy(token.lexema, ";");
            return token;
        case '(':
            ConsumirCaracter();
            token = CrearToken(TOKEN_LPAREN, lineaInicio, columnaInicio);
            strcpy(token.lexema, "(");
            return token;
        case ')':
            ConsumirCaracter();
            token = CrearToken(TOKEN_RPAREN, lineaInicio, columnaInicio);
            strcpy(token.lexema, ")");
            return token;
        default: {
            char simbolo[2] = { (char)c, '\0' };
            ConsumirCaracter();
            return CrearError("E_LEXICAL", "Byte no reconocido por el scanner.",
                              simbolo, lineaInicio, columnaInicio);
        }
    }
}
