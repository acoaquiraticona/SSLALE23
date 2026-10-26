/**
 * @file parser_test.c
 * @brief Pruebas del parser con un scanner de prueba que inyecta tokens y fallos.
 *
 * Este ejecutable define las funciones de la interfaz Scanner en lugar de
 * enlazar el scanner real. Así controla exactamente cuántas lecturas ocurren
 * y en qué momento aparece EOF, un error recuperable o un fallo fatal. La capa
 * semántica real permite inspeccionar los efectos confirmados de las sentencias.
 */
#include "Parser.h"
#include "Semantics.h"
#include "test_support.h"

/** @brief Selector del flujo simulado y cantidad de tokens solicitados. */
static int escenario;
static int leidos;

/** @brief Reinicia el contador de lecturas sin cambiar el escenario seleccionado. */
void ScannerReset(void) {
    leidos = 0;
}
/**
 * @brief Proporciona los nombres de token necesarios para las trazas de la suite.
 * @param tipo Clase del token simulado.
 * @return Nombre constante; las clases no enumeradas usan el valor por defecto EOF.
 */
const char *ScannerTokenName(TipoToken tipo) {
    switch (tipo) {
        case TOKEN_ERROR:
            return "ERROR";
        case TOKEN_NATURAL:
            return "NATURAL";
        case TOKEN_SEMICOLON:
            return "';'";
        default:
            return "EOF";
    }
}

/**
 * @brief Entrega el próximo token de una secuencia controlada por escenario.
 * @return Token con posición sintética y los atributos que necesita el caso.
 *
 * El escenario 0 admite una única lectura de EOF. Los escenarios 1 y 2 sitúan
 * un fallo fatal durante la recuperación de un error léxico. El 3 entrega una
 * asignación completa seguida de E_IO, para comprobar que su efecto ya quedó
 * confirmado al leer el punto y coma. Los restantes entregan una evaluación
 * simple usada para forzar fallos al escribir TRACE e INFO.
 */
Token GetNextToken(void) {
    Token token = {0};
    token.linea = 1;
    token.columna = leidos + 1;
    int indiceToken = leidos;
    leidos++;
    if (escenario == 0) {
        assert(indiceToken == 0); /* No se permite una segunda lectura de EOF. */
        token.tipo = TOKEN_EOF;
    } else if (escenario == 1 || escenario == 2) {
        assert(indiceToken < 2);
        token.tipo = TOKEN_ERROR;
        ErrorCode codigo;
        if (indiceToken == 0) {
            codigo = E_MISSINGNO;
        } else if (escenario == 1) {
            codigo = E_IO;
        } else {
            codigo = E_RESOURCE;
        }
        SourcePosition posicion = {token.linea, token.columna};
        token.diagnostico = MakeDiagnostic(codigo, posicion, "Fallo inyectado por la prueba.");
    } else if (escenario == 3) {
        static const TipoToken tipos[] = {TOKEN_CAPTURA, TOKEN_IDENTIFIER, TOKEN_EQUAL,
                                        TOKEN_NATURAL, TOKEN_SEMICOLON, TOKEN_ERROR};
        assert(indiceToken < 6);
        token.tipo = tipos[indiceToken];
        if (token.tipo == TOKEN_IDENTIFIER) {
            strcpy(token.lexema, "energia");
        }
        if (token.tipo == TOKEN_NATURAL) {
            token.valor = 2;
        }
        if (token.tipo == TOKEN_ERROR) {
            token.diagnostico = MakeDiagnostic(E_IO, (SourcePosition){1, 6}, "Fallo posterior al punto y coma.");
        }
    } else {
        assert(indiceToken < 3);
        if (indiceToken == 0) {
            token.tipo = TOKEN_NATURAL;
        } else if (indiceToken == 1) {
            token.tipo = TOKEN_SEMICOLON;
        } else {
            token.tipo = TOKEN_EOF;
        }
        token.valor = 2;
    }
    return token;
}

/**
 * @brief Selecciona un flujo simulado y captura las salidas de una sesión.
 * @param caso Escenario que consultará GetNextToken.
 * @param trace Indica si ParserRun debe emitir trazas.
 * @return Código de terminación devuelto por ParserRun.
 *
 * Las redirecciones aíslan cada ejecución. El propio ParserRun debe reiniciar
 * scanner y semántica; las pruebas verifican esos efectos por separado.
 */
static int Run(int caso, bool trace) {
    escenario = caso;
    assert(freopen(".build/parser-output.txt", "wb", stdout) != NULL);
    assert(freopen(".build/parser-log.txt", "wb", stderr) != NULL);
    return ParserRun(trace);
}

/**
 * @brief Comprueba finalización, recuperación y confirmación de efectos del parser.
 * @return Cero si todas las aserciones se cumplen.
 */
int main(void) {
    /** Una sesión nueva descarta las variables previas y solicita EOF una sola
     * vez, incluso con la traza habilitada. */
    puts("Parser: pruebas de EOF, descarte fatal y fallos de reporte con assert");
    assert(fflush(stdout) == 0);
    assert(SemAsignar("previa", 1).ok);
    assert(Run(0, true) == 0 && leidos == 1);
    assert(!SemVariable("previa").ok);
    char *log = TestReadFile(".build/parser-log.txt");
    TestEqual(log, "TRACE linea=1 columna=1 token=EOF\n");
    free(log);
    /** Durante el descarte hasta el delimitador, un fallo de lectura o recursos
     * termina la sesión y se informa una sola vez tras el error recuperable. */
    for (int caso = 1; caso <= 2; ++caso) {
        assert(Run(caso, true) == 2 && leidos == 2);
        log = TestReadFile(".build/parser-log.txt");
        const char *codigo;
        if (caso == 1) {
            codigo = "codigo=E_IO";
        } else {
            codigo = "codigo=E_RESOURCE";
        }
        assert(strstr(log, "ERROR linea=1 columna=1 codigo=E_MISSINGNO") != NULL);
        const char *fatal = strstr(log, "ERROR linea=1 columna=2 ");
        assert(fatal != NULL && strstr(fatal, codigo) != NULL);
        assert(strstr(fatal + 1, "ERROR linea=") == NULL);
        free(log);
    }
    /** Un fatal que aparece después de ; no revierte la asignación terminada:
     * el registro INFO y el valor de energia deben existir antes del diagnóstico. */
    assert(Run(3, false) == 2 && leidos == 6);
    assert(SemVariable("energia").ok && SemVariable("energia").valor == 2);
    log = TestReadFile(".build/parser-log.txt");
    assert(strncmp(log, "INFO linea=1 columna=1 accion=asignacion variable=energia valor=2\n", 63) == 0);
    assert(strstr(log, "ERROR linea=1 columna=6 codigo=E_IO") != NULL);
    free(log);
#ifndef _WIN32
    /** /dev/full simula un destino que no acepta escritura. Si está disponible,
     * cada clase de registro fallido debe detener la sesión sin nuevas lecturas;
     * esta comprobación depende de una facilidad de los sistemas POSIX. */
    FILE *full = fopen("/dev/full", "wb");
    if (full != NULL) {
        assert(fclose(full) == 0);
        /* TRACE fallido: termina sin pedir otro token. */
        escenario = 4;
        assert(freopen("/dev/full", "wb", stderr) != NULL);
        assert(ParserRun(true) == 2 && leidos == 1);
        /* INFO fallido: no pide el token posterior al ;. */
        assert(freopen("/dev/full", "wb", stderr) != NULL);
        assert(ParserRun(false) == 2 && leidos == 2);
        /* ERROR fallido: no intenta recuperarse ni emitir otro registro. */
        escenario = 1;
        assert(freopen("/dev/full", "wb", stderr) != NULL);
        assert(ParserRun(false) == 2 && leidos == 1);
    }
#endif
    return 0;
}
