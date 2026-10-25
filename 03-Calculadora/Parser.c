/* PROBLEMA: el parser recibido solo reconocia sintaxis, incluia falinks y no evaluaba, recuperaba ni emitia registros.
 * Se conserva el codigo original sin cambios. La version completa esta debajo. */
#if 0
#include "Parser.h"

#include <stdlib.h>
#include <string.h>

static Token tokenActual;
static bool hayToken;
static char *sentenciaActual;
static size_t longitudSentencia;
static size_t capacidadSentencia;
static bool errorMemoria;
static ParserSentenceHandler recibirSentencia;

static bool AsegurarToken(void) {
    if (!hayToken) {
        tokenActual = GetNextToken();
        hayToken = true;
    }
    return true;
}

static bool AgregarLexema(const char *lexema) {
    size_t longitudLexema = strlen(lexema);
    size_t separador = longitudSentencia == 0 ? 0 : 1;
    size_t requerido = longitudSentencia + separador + longitudLexema + 1;

    if (requerido > capacidadSentencia) {
        size_t nuevaCapacidad = capacidadSentencia == 0 ? 128 : capacidadSentencia;
        while (nuevaCapacidad < requerido) {
            if (nuevaCapacidad > (size_t)-1 / 2) {
                errorMemoria = true;
                return false;
            }
            nuevaCapacidad *= 2;
        }

        char *nuevaSentencia = realloc(sentenciaActual, nuevaCapacidad);
        if (nuevaSentencia == NULL) {
            errorMemoria = true;
            return false;
        }
        sentenciaActual = nuevaSentencia;
        capacidadSentencia = nuevaCapacidad;
    }

    if (separador != 0)
        sentenciaActual[longitudSentencia++] = ' ';
    memcpy(sentenciaActual + longitudSentencia, lexema, longitudLexema);
    longitudSentencia += longitudLexema;
    sentenciaActual[longitudSentencia] = '\0';
    return true;
}

static bool ConsumirToken(void) {
    if (!AgregarLexema(tokenActual.lexema)) return false;
    hayToken = false;
    return true;
}

static bool Esperar(TipoToken esperado) {
    AsegurarToken();
    if (tokenActual.tipo != esperado) return false;
    return ConsumirToken();
}

static bool ParseTranslationUnit(void);
static bool ParseStatementList(void);
static bool ParseStatement(void);
static bool ParseExpression(void);
static bool ParseExpressionTail(void);
static bool ParseTerm(void);
static bool ParseTermTail(void);
static bool ParseUnary(void);
static bool ParsePrimary(void);

static bool ParseTranslationUnit(void) {
    return ParseStatementList();
}

static bool ParseStatementList(void) {
    for (;;) {
        AsegurarToken();
        if (tokenActual.tipo == TOKEN_EOF) return true;

        longitudSentencia = 0;
        if (sentenciaActual != NULL) sentenciaActual[0] = '\0';

        if (!ParseStatement() || errorMemoria) return false;
        if (recibirSentencia != NULL)
            recibirSentencia(sentenciaActual);
    }
}

static bool ParseStatement(void) {
    AsegurarToken();

    if (tokenActual.tipo == TOKEN_CAPTURA) {
        return ConsumirToken() &&
               Esperar(TOKEN_IDENTIFIER) &&
               Esperar(TOKEN_EQUAL) &&
               ParseExpression() &&
               Esperar(TOKEN_SEMICOLON);
    }

    if (tokenActual.tipo == TOKEN_LIBERAR) {
        return ConsumirToken() &&
               Esperar(TOKEN_IDENTIFIER) &&
               Esperar(TOKEN_SEMICOLON);
    }

    return ParseExpression() && Esperar(TOKEN_SEMICOLON);
}

static bool ParseExpression(void) {
    return ParseTerm() && ParseExpressionTail();
}

static bool ParseExpressionTail(void) {
    for (;;) {
        AsegurarToken();
        if (tokenActual.tipo != TOKEN_PLUSLE &&
            tokenActual.tipo != TOKEN_MINUN)
            return true;

        if (!ConsumirToken() || !ParseTerm()) return false;
    }
}

static bool ParseTerm(void) {
    return ParseUnary() && ParseTermTail();
}

static bool ParseTermTail(void) {
    for (;;) {
        AsegurarToken();
        if (tokenActual.tipo != TOKEN_TANDEMAUS &&
            tokenActual.tipo != TOKEN_FALINKS)
            return true;

        if (!ConsumirToken() || !ParseUnary()) return false;
    }
}

static bool ParseUnary(void) {
    AsegurarToken();
    if (tokenActual.tipo == TOKEN_MINUN)
        return ConsumirToken() && ParseUnary();
    return ParsePrimary();
}

static bool ParsePrimary(void) {
    AsegurarToken();

    switch (tokenActual.tipo) {
        case TOKEN_NATURAL:
        case TOKEN_IDENTIFIER:
            return ConsumirToken();

        case TOKEN_LPAREN:
            return ConsumirToken() &&
                   ParseExpression() &&
                   Esperar(TOKEN_RPAREN);

        case TOKEN_DITTO:
            return ConsumirToken() &&
                   Esperar(TOKEN_LPAREN) &&
                   ParseExpression() &&
                   Esperar(TOKEN_RPAREN);

        default:
            return false;
    }
}

bool ParserRun(ParserSentenceHandler handler) {
    free(sentenciaActual);
    sentenciaActual = NULL;
    longitudSentencia = 0;
    capacidadSentencia = 0;
    hayToken = false;
    errorMemoria = false;
    recibirSentencia = handler;

    bool resultado = ParseTranslationUnit() && !errorMemoria;
    free(sentenciaActual);
    sentenciaActual = NULL;
    longitudSentencia = 0;
    capacidadSentencia = 0;
    recibirSentencia = NULL;
    return resultado;
}

#endif

/* Implementacion de Calc.md 5-9. Un analizador por cada no terminal.
 * El bloque anterior se conserva como evidencia del trabajo recibido. */
#include "Parser.h"
#include "Semantics.h"
#include "errors.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Limite de pila de ejecucion, no del lexico ni del dominio numerico.
 * Evita desbordar la pila de C con unarias/parentesis anidados. */
enum { MAX_PROFUNDIDAD = 256 };
static struct {
    Token actual;
    bool hayToken, trace, huboError, sentenciaFallida, fatal;
    unsigned profundidad;
} sesion;

static SourcePosition Posicion(void) {
    return (SourcePosition){sesion.actual.linea, sesion.actual.columna};
}

static bool Informar(Diagnostic error) {
    if (sesion.fatal) return false;
    if (!sesion.sentenciaFallida || error.gravedad == ERROR_FATAL) {
        if (!ReportError(&error)) sesion.fatal = true;
    }
    sesion.huboError = sesion.sentenciaFallida = true;
    if (error.gravedad == ERROR_FATAL) sesion.fatal = true;
    return false;
}

static bool Fallar(ErrorCode codigo, SourcePosition posicion, const char *detalle) {
    return Informar(MakeDiagnostic(codigo, posicion, detalle));
}

static bool AsegurarToken(void) {
    if (sesion.fatal) return false;
    if (!sesion.hayToken) {
        sesion.actual = GetNextToken();
        sesion.hayToken = true;
        if (sesion.trace) {
            TraceRecord traza = {0};
            traza.posicion = Posicion();
            traza.clase = ScannerTokenName(sesion.actual.tipo);
            if (sesion.actual.tipo == TOKEN_NATURAL) {
                traza.atributo = TRACE_NATURAL;
                traza.valor = sesion.actual.valor;
                traza.fuera_de_rango = sesion.actual.fuera_de_rango;
            } else if (sesion.actual.tipo == TOKEN_IDENTIFIER) {
                traza.atributo = TRACE_IDENTIFIER;
                traza.nombre = sesion.actual.lexema;
            } else if (sesion.actual.tipo == TOKEN_ERROR) {
                traza.atributo = TRACE_ERROR;
                traza.codigo = sesion.actual.diagnostico.codigo;
            }
            if (!ReportTrace(&traza)) {
                sesion.fatal = true;
                return false;
            }
        }
        /* Tambien se verifica durante la recuperacion. */
        if (sesion.actual.tipo == TOKEN_ERROR &&
            sesion.actual.diagnostico.gravedad == ERROR_FATAL)
            return Informar(sesion.actual.diagnostico);
    }
    return true;
}

static bool Disponible(void) {
    if (!AsegurarToken()) return false;
    if (sesion.actual.tipo == TOKEN_ERROR) return Informar(sesion.actual.diagnostico);
    return true;
}

/* Consumir no pide anticipacion: INFO y los efectos preceden al proximo token. */
static void ConsumirToken(void) { sesion.hayToken = false; }

static bool Comprobar(TipoToken esperado) {
    if (!Disponible()) return false;
    if (sesion.actual.tipo != esperado) {
        char detalle[160];
        snprintf(detalle, sizeof detalle, "Se esperaba %s; se encontro %s.",
                 ScannerTokenName(esperado), ScannerTokenName(sesion.actual.tipo));
        return Fallar(E_SYNTAX, Posicion(), detalle);
    }
    return true;
}

static bool Esperar(TipoToken esperado) {
    if (!Comprobar(esperado)) return false;
    ConsumirToken();
    return true;
}

static bool UsarResultado(ResultadoSem resultado, SourcePosition posicion, int64_t *valor) {
    if (!resultado.ok) {
        resultado.diagnostico.posicion = posicion;
        return Informar(resultado.diagnostico);
    }
    if (valor != NULL) *valor = resultado.valor;
    return true;
}

static bool ParseTranslationUnit(void);
static bool ParseStatementList(void);
static bool ParseStatement(void);
static bool ParseExpression(int64_t *valor);
static bool ParseExpressionTail(int64_t *acumulado);
static bool ParseTerm(int64_t *valor);
static bool ParseTermTail(int64_t *acumulado);
static bool ParseUnary(int64_t *valor);
static bool ParsePrimary(int64_t *valor);

static bool Recuperar(void) {
    for (;;) {
        if (!AsegurarToken()) return false;
        if (sesion.actual.tipo == TOKEN_EOF) return true;
        if (sesion.actual.tipo == TOKEN_SEMICOLON) {
            ConsumirToken();
            return true;
        }
        /* Los ERROR recuperables se trazaron, pero no se vuelven a informar. */
        ConsumirToken();
    }
}

static bool ParseTranslationUnit(void) {
    return ParseStatementList() && AsegurarToken() && sesion.actual.tipo == TOKEN_EOF;
}

static bool ParseStatementList(void) {
    for (;;) {
        if (!AsegurarToken()) return false;
        if (sesion.actual.tipo == TOKEN_EOF) return true;
        sesion.sentenciaFallida = false;
        if (!ParseStatement()) {
            if (sesion.fatal || !Recuperar()) return false;
        }
    }
}

static bool ParseStatement(void) {
    if (!Disponible()) return false;
    SourcePosition inicio = Posicion(), nombrePos = inicio;
    char nombre[SEM_MAX_NOMBRE + 1] = "";
    InfoAction accion = INFO_EVALUACION;
    int64_t valor = 0;
    if (sesion.actual.tipo == TOKEN_CAPTURA) {
        accion = INFO_ASIGNACION;
        ConsumirToken();
        if (!Comprobar(TOKEN_IDENTIFIER)) return false;
        nombrePos = Posicion();
        memcpy(nombre, sesion.actual.lexema, strlen(sesion.actual.lexema) + 1);
        ConsumirToken();
        if (!Esperar(TOKEN_EQUAL) || !ParseExpression(&valor)) return false;
    } else if (sesion.actual.tipo == TOKEN_LIBERAR) {
        accion = INFO_LIBERACION;
        ConsumirToken();
        if (!Comprobar(TOKEN_IDENTIFIER)) return false;
        nombrePos = Posicion();
        memcpy(nombre, sesion.actual.lexema, strlen(sesion.actual.lexema) + 1);
        ConsumirToken();
    } else if (!ParseExpression(&valor)) return false;

    /* El ; sigue como anticipacion mientras se aplican los efectos. Asi un
     * E_LIMIT/E_UNDEFINED se recupera sin descartar la sentencia siguiente. */
    if (!Comprobar(TOKEN_SEMICOLON)) return false;
    if (accion == INFO_ASIGNACION) {
        if (!UsarResultado(SemAsignar(nombre, valor), nombrePos, NULL)) return false;
    } else if (accion == INFO_LIBERACION) {
        if (!UsarResultado(SemLiberar(nombre), nombrePos, NULL)) return false;
    } else {
        char pokemon[128];
        SemAPokemon(valor, pokemon, sizeof pokemon);
        if (fprintf(stdout, "%s (%" PRId64 ")\n", pokemon, valor) < 0 || fflush(stdout) != 0)
            return Fallar(E_IO, inicio, "Fallo al escribir el resultado en stdout.");
    }
    if (!ReportInfo(inicio, accion, nombre, valor)) {
        sesion.fatal = true;
        return false;
    }
    ConsumirToken();
    return true;
}

static bool ParseExpression(int64_t *valor) {
    return ParseTerm(valor) && ParseExpressionTail(valor);
}

static bool ParseExpressionTail(int64_t *acumulado) {
    for (;;) {
        if (!Disponible()) return false;
        TipoToken operador = sesion.actual.tipo;
        if (operador != TOKEN_PLUSLE && operador != TOKEN_MINUN) {
            if (operador == TOKEN_SEMICOLON || operador == TOKEN_RPAREN) return true;
            return Fallar(E_SYNTAX, Posicion(), "Se esperaba plusle, minun, ')' o ';'.");
        }
        SourcePosition posicion = Posicion();
        ConsumirToken();
        int64_t derecha;
        if (!ParseTerm(&derecha)) return false;
        ResultadoSem resultado = operador == TOKEN_PLUSLE
            ? SemSumar(*acumulado, derecha) : SemRestar(*acumulado, derecha);
        if (!UsarResultado(resultado, posicion, acumulado)) return false;
    }
}

static bool ParseTerm(int64_t *valor) {
    return ParseUnary(valor) && ParseTermTail(valor);
}

static bool ParseTermTail(int64_t *acumulado) {
    for (;;) {
        if (!Disponible()) return false;
        if (sesion.actual.tipo != TOKEN_TANDEMAUS) {
            TipoToken t = sesion.actual.tipo;
            if (t == TOKEN_PLUSLE || t == TOKEN_MINUN || t == TOKEN_SEMICOLON || t == TOKEN_RPAREN)
                return true;
            return Fallar(E_SYNTAX, Posicion(), "Se esperaba un operador, ')' o ';'.");
        }
        SourcePosition posicion = Posicion();
        ConsumirToken();
        int64_t derecha;
        if (!ParseUnary(&derecha)) return false;
        if (!UsarResultado(SemMultiplicar(*acumulado, derecha), posicion, acumulado)) return false;
    }
}

static bool ParseUnary(int64_t *valor) {
    if (!Disponible()) return false;
    if (sesion.profundidad >= MAX_PROFUNDIDAD)
        return Fallar(E_RESOURCE, Posicion(), "Se agoto la pila disponible para expresiones anidadas.");
    ++sesion.profundidad;
    bool ok;
    if (sesion.actual.tipo == TOKEN_MINUN) {
        SourcePosition operador = Posicion();
        ConsumirToken();
        ok = ParseUnary(valor);
        if (ok) ok = UsarResultado(SemNegar(*valor), operador, valor);
    } else ok = ParsePrimary(valor);
    --sesion.profundidad;
    return ok;
}

static bool ParsePrimary(int64_t *valor) {
    if (!Disponible()) return false;
    SourcePosition posicion = Posicion();
    switch (sesion.actual.tipo) {
        case TOKEN_NATURAL: {
            int64_t natural = sesion.actual.valor;
            bool fuera = sesion.actual.fuera_de_rango;
            ConsumirToken();
            return UsarResultado(SemNatural(natural, fuera), posicion, valor);
        }
        case TOKEN_IDENTIFIER: {
            ResultadoSem resultado = SemVariable(sesion.actual.lexema);
            ConsumirToken();
            return UsarResultado(resultado, posicion, valor);
        }
        case TOKEN_LPAREN:
            ConsumirToken();
            return ParseExpression(valor) && Esperar(TOKEN_RPAREN);
        case TOKEN_DITTO:
            ConsumirToken();
            if (!Esperar(TOKEN_LPAREN) || !ParseExpression(valor) || !Esperar(TOKEN_RPAREN)) return false;
            return UsarResultado(SemDuplicar(*valor), posicion, valor);
        default:
            return Fallar(E_SYNTAX, posicion, "Se esperaba un literal, variable, minun, '(' o ditto.");
    }
}

int ParserRun(bool trace) {
    memset(&sesion, 0, sizeof sesion);
    sesion.trace = trace;
    ScannerReset();
    SemReset();
    if (!ParseTranslationUnit() || sesion.fatal) return 2;
    return sesion.huboError ? 1 : 0;
}
