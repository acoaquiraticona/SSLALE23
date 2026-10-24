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
