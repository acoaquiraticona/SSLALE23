#include "Scanner.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define VALOR_MAXIMO INT64_C(999999)
#define LONGITUD_MAXIMA_IDENTIFICADOR 31
#define CAPACIDAD_LEXEMA 128

/* Estado privado del scanner: un byte de anticipacion y su posicion. */
static bool limitePosicion;
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

/* PROBLEMA: el incremento de posiciones no controlaba el agotamiento de int.
 * Se conserva intacta la version recibida; la correccion esta debajo. */
#if 0
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


#endif
/* Version corregida. */

static int ConsumirCaracter(void) {
    int caracter = MirarCaracter();
    caracterAnticipado = -2;
    if (caracter == '\r') {
        if (lineaActual == INT_MAX) limitePosicion = true;
        else ++lineaActual;
        columnaActual = 1;
        anteriorFueCR = 1;
    } else if (caracter == '\n') {
        if (!anteriorFueCR) {
            if (lineaActual == INT_MAX) limitePosicion = true;
            else ++lineaActual;
        }
        columnaActual = 1;
        anteriorFueCR = 0;
    } else if (caracter != EOF) {
        if (columnaActual == INT_MAX) limitePosicion = true;
        else ++columnaActual;
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

/* PROBLEMA: faltaban los datos compartidos de errors.h y el detalle de la entrada invalida.
 * Se conserva intacta la version recibida; la correccion esta debajo. */
#if 0
static Token CrearError(const char *codigo, const char *detalle,
                        const char *lexema, int linea, int columna) {
    Token token = CrearToken(TOKEN_ERROR, linea, columna);
    snprintf(token.codigo_error, sizeof token.codigo_error, "%s", codigo);
    snprintf(token.detalle, sizeof token.detalle, "%s", detalle);
    snprintf(token.lexema, sizeof token.lexema, "%s", lexema);
    return token;
}


#endif
/* Version corregida. */

static Token CrearError(ErrorCode codigo, const char *detalle,
                       const char *lexema, int linea, int columna) {
    Token token = CrearToken(TOKEN_ERROR, linea, columna);
    char mensaje[512];
    snprintf(mensaje, sizeof mensaje, "%s Entrada: \"%s\"", detalle, lexema);
    token.diagnostico = MakeDiagnostic(codigo, (SourcePosition){linea, columna}, mensaje);
    snprintf(token.codigo_error, sizeof token.codigo_error, "%s", ErrorCodeName(codigo));
    snprintf(token.detalle, sizeof token.detalle, "%s", token.diagnostico.detalle);
    snprintf(token.lexema, sizeof token.lexema, "%s", lexema);
    token.lexema_recortado = strstr(lexema, "(recortado)") != NULL;
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

/* PROBLEMA: falinks no es una palabra reservada en Calc.md 2.2 y 3.2.
 * Se conserva intacta la version recibida; la correccion esta debajo. */
#if 0
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


#endif
/* Version corregida. */

static TipoToken PalabraReservada(const char *palabra) {
    static const struct { const char *nombre; TipoToken tipo; } palabras[] = {
        {"plusle", TOKEN_PLUSLE}, {"minun", TOKEN_MINUN},
        {"tandemaus", TOKEN_TANDEMAUS}, {"ditto", TOKEN_DITTO},
        {"captura", TOKEN_CAPTURA}, {"liberar", TOKEN_LIBERAR}
    };
    for (size_t i = 0; i < sizeof palabras / sizeof palabras[0]; ++i)
        if (strcmp(palabra, palabras[i].nombre) == 0) return palabras[i].tipo;
    return TOKEN_IDENTIFIER;
}

/* PROBLEMA: la correspondencia de los digitos 4 a 9 no coincide con Calc.md 2.1.
 * Se conserva intacta la version recibida; la correccion esta debajo. */
#if 0
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


#endif
/* Version corregida. */

static int ObtenerDigitoPokemon(const char *palabra) {
    static const char *const nombres[] = {
        "mew", "bulbasaur", "ivysaur", "venusaur", "charmander",
        "charmeleon", "charizard", "squirtle", "wartortle", "blastoise"
    };
    for (int i = 0; i < 10; ++i)
        if (strcmp(palabra, nombres[i]) == 0) return i;
    return -1;
}

/* El almacenamiento del lexema es acotado; el reconocimiento no lo es. */
static void GuardarByte(char *texto, size_t capacidad, size_t *longitud,
                        bool *recortado, int c) {
    if (*longitud + 1 >= capacidad) *recortado = true;
    else AgregarCaracter(texto, capacidad, longitud, c);
}

static void MarcarRecorte(char *texto, size_t capacidad, bool recortado) {
    static const char marca[] = "... (recortado)";
    if (recortado && capacidad >= sizeof marca)
        memcpy(texto + capacidad - sizeof marca, marca, sizeof marca);
}

/* PROBLEMA: el recorte del lexema se ocultaba y los errores no usaban el contrato comun.
 * Se conserva intacta la version recibida; la correccion esta debajo. */
#if 0
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


#endif
/* Version corregida. */

static Token LeerPalabra(void) {
    char palabra[CAPACIDAD_LEXEMA] = "";
    size_t longitud = 0;
    bool recortado = false;
    int linea = lineaActual, columna = columnaActual;
    while (EsCaracterIdentificador(MirarCaracter()))
        GuardarByte(palabra, sizeof palabra, &longitud, &recortado, ConsumirCaracter());
    MarcarRecorte(palabra, sizeof palabra, recortado);
    if (recortado || longitud > LONGITUD_MAXIMA_IDENTIFICADOR)
        return CrearError(E_LIMIT, "El identificador supera los 31 caracteres.", palabra, linea, columna);
    TipoToken tipo = PalabraReservada(palabra);
    if (tipo == TOKEN_IDENTIFIER && ObtenerDigitoPokemon(palabra) >= 0)
        return CrearError(E_MISSINGNO, "Un digito Pokemon requiere corchetes.", palabra, linea, columna);
    Token token = CrearToken(tipo, linea, columna);
    snprintf(token.lexema, sizeof token.lexema, "%s", palabra);
    return token;
}

/* PROBLEMA: un literal valido se rechazaba por ocupar mas de 127 bytes; eso no es un limite del lenguaje. La conversion debe seguir sin desbordar y separar rango de forma lexica.
 * Se conserva intacta la version recibida; la correccion esta debajo. */
#if 0
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


#endif
/* Version corregida. */

/* BAD_PNUM termina antes de ;, CR, LF o EOF y puede consumir un ]. */
static Token LeerNumeroPokemon(void) {
    char lexema[CAPACIDAD_LEXEMA] = "";
    size_t longitud = 0;
    bool recortado = false, invalido = false, cerrado = false, tieneDigito = false;
    bool fuera = false;
    int64_t valor = 0;
    int linea = lineaActual, columna = columnaActual;
    GuardarByte(lexema, sizeof lexema, &longitud, &recortado, ConsumirCaracter());
    for (;;) {
        while (EsEspacioHorizontal(MirarCaracter()))
            GuardarByte(lexema, sizeof lexema, &longitud, &recortado, ConsumirCaracter());
        int c = MirarCaracter();
        if (c == ']') {
            GuardarByte(lexema, sizeof lexema, &longitud, &recortado, ConsumirCaracter());
            cerrado = true;
            break;
        }
        if (c == EOF || c == ';' || c == '\r' || c == '\n') break;
        char palabra[32] = "";
        size_t n = 0;
        bool palabraInvalida = false;
        while ((c = MirarCaracter()) != EOF && c != ']' && c != ';' &&
               c != '\r' && c != '\n' && !EsEspacioHorizontal(c)) {
            c = ConsumirCaracter();
            GuardarByte(lexema, sizeof lexema, &longitud, &recortado, c);
            if (c < 'a' || c > 'z' || n + 1 >= sizeof palabra) palabraInvalida = true;
            else AgregarCaracter(palabra, sizeof palabra, &n, c);
        }
        int digito = palabraInvalida ? -1 : ObtenerDigitoPokemon(palabra);
        if (digito < 0) invalido = true;
        else {
            tieneDigito = true;
            if (!fuera) {
                if (valor > (VALOR_MAXIMO - digito) / 10) fuera = true;
                else valor = valor * 10 + digito;
            }
        }
    }
    MarcarRecorte(lexema, sizeof lexema, recortado);
    if (!cerrado || invalido || !tieneDigito)
        return CrearError(E_MISSINGNO, "MissingNo: numero Pokemon mal formado.", lexema, linea, columna);
    Token token = CrearToken(TOKEN_NATURAL, linea, columna);
    token.valor = valor;
    token.fuera_de_rango = fuera;
    token.lexema_recortado = recortado;
    snprintf(token.lexema, sizeof token.lexema, "%s", lexema);
    return token;
}

/* PROBLEMA: el recorte de numeros arabigos largos no se indicaba y faltaba el codigo comun.
 * Se conserva intacta la version recibida; la correccion esta debajo. */
#if 0
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


#endif
/* Version corregida. */

static Token LeerNumeroDecimal(void) {
    char numero[CAPACIDAD_LEXEMA] = "";
    size_t longitud = 0;
    bool recortado = false;
    int linea = lineaActual, columna = columnaActual;
    while (EsDigito(MirarCaracter()))
        GuardarByte(numero, sizeof numero, &longitud, &recortado, ConsumirCaracter());
    MarcarRecorte(numero, sizeof numero, recortado);
    return CrearError(E_MISSINGNO, "Los numeros arabigos no estan permitidos.", numero, linea, columna);
}

/* PROBLEMA: un fallo de lectura durante un token podia aparecer como error lexico antes del fatal; faltaba reinicializacion y uso de diagnosticos comunes.
 * Se conserva intacta la version recibida; la correccion esta debajo. */
#if 0
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

#endif
/* Version corregida. */

Token GetNextToken(void) {
    OmitirEspaciosYComentarios();
    int c = MirarCaracter();
    int linea = lineaActual, columna = columnaActual;
    Token token;
    if (c == EOF) token = CrearToken(TOKEN_EOF, linea, columna);
    else if (EsInicioIdentificador(c)) token = LeerPalabra();
    else if (c == '[') token = LeerNumeroPokemon();
    else if (EsDigito(c)) token = LeerNumeroDecimal();
    else {
        TipoToken tipo = TOKEN_ERROR;
        switch (c) {
            case '=': tipo = TOKEN_EQUAL; break;
            case ';': tipo = TOKEN_SEMICOLON; break;
            case '(': tipo = TOKEN_LPAREN; break;
            case ')': tipo = TOKEN_RPAREN; break;
            default: break;
        }
        ConsumirCaracter();
        if (tipo == TOKEN_ERROR) {
            char detalle[64], simbolo[2] = {(char)c, '\0'};
            snprintf(detalle, sizeof detalle, "Byte no reconocido (0x%02X).", (unsigned)c);
            token = CrearError(E_LEXICAL, detalle, simbolo, linea, columna);
        } else {
            token = CrearToken(tipo, linea, columna);
            token.lexema[0] = (char)c;
        }
    }
    if (ferror(stdin) && !errorLecturaInformado) {
        errorLecturaInformado = 1;
        return CrearError(E_IO, "Fallo al leer stdin.", "", linea, columna);
    }
    if (limitePosicion)
        return CrearError(E_RESOURCE, "Se agoto el rango de posiciones de fuente.", "", linea, columna);
    return token;
}

void ScannerReset(void) {
    caracterAnticipado = -2;
    lineaActual = columnaActual = 1;
    anteriorFueCR = finDeArchivoAlcanzado = errorLecturaInformado = 0;
    limitePosicion = false;
}

const char *ScannerTokenName(TipoToken tipo) {
    switch (tipo) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        case TOKEN_NATURAL: return "NATURAL";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_PLUSLE: return "PLUSLE";
        case TOKEN_MINUN: return "MINUN";
        case TOKEN_TANDEMAUS: return "TANDEMAUS";
        case TOKEN_DITTO: return "DITTO";
        case TOKEN_CAPTURA: return "CAPTURA";
        case TOKEN_LIBERAR: return "LIBERAR";
        case TOKEN_EQUAL: return "'='";
        case TOKEN_SEMICOLON: return "';'";
        case TOKEN_LPAREN: return "'('";
        case TOKEN_RPAREN: return "')'";
        default: return "ERROR"; /* TOKEN_FALINKS: legado que nunca se emite. */
    }
}

