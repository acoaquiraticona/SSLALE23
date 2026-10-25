/* Unidad del parser con scanner controlado: EOF unico, transacciones,
 * errores fatales durante descarte y fallos de cada clase de reporte. */
#include "Parser.h"
#include "Semantics.h"
#include "test_support.h"

static int escenario, leidos;

void ScannerReset(void) { leidos = 0; }
const char *ScannerTokenName(TipoToken t) {
    switch (t) {
        case TOKEN_ERROR: return "ERROR";
        case TOKEN_NATURAL: return "NATURAL";
        case TOKEN_SEMICOLON: return "';'";
        default: return "EOF";
    }
}

Token GetNextToken(void) {
    Token t = {0};
    t.linea = 1; t.columna = leidos + 1;
    int index = leidos++;
    if (escenario == 0) {
        assert(index == 0); /* No se permite una segunda lectura de EOF. */
        t.tipo = TOKEN_EOF;
    } else if (escenario == 1 || escenario == 2) {
        assert(index < 2);
        t.tipo = TOKEN_ERROR;
        t.diagnostico = MakeDiagnostic(index == 0 ? E_MISSINGNO :
                                      (escenario == 1 ? E_IO : E_RESOURCE),
                                      (SourcePosition){t.linea, t.columna}, "Fallo inyectado por la prueba.");
    } else if (escenario == 3) {
        static const TipoToken tipos[] = {TOKEN_CAPTURA, TOKEN_IDENTIFIER, TOKEN_EQUAL,
                                        TOKEN_NATURAL, TOKEN_SEMICOLON, TOKEN_ERROR};
        assert(index < 6);
        t.tipo = tipos[index];
        if (t.tipo == TOKEN_IDENTIFIER) strcpy(t.lexema, "energia");
        if (t.tipo == TOKEN_NATURAL) t.valor = 2;
        if (t.tipo == TOKEN_ERROR)
            t.diagnostico = MakeDiagnostic(E_IO, (SourcePosition){1, 6}, "Fallo posterior al punto y coma.");
    } else {
        assert(index < 3);
        t.tipo = index == 0 ? TOKEN_NATURAL : (index == 1 ? TOKEN_SEMICOLON : TOKEN_EOF);
        t.valor = 2;
    }
    return t;
}

static int Run(int caso, bool trace) {
    escenario = caso;
    assert(freopen(".build/parser-output.txt", "wb", stdout) != NULL);
    assert(freopen(".build/parser-log.txt", "wb", stderr) != NULL);
    return ParserRun(trace);
}

int main(void) {
    puts("Parser: pruebas de EOF, descarte fatal y fallos de reporte con assert");
    assert(fflush(stdout) == 0);
    assert(SemAsignar("previa", 1).ok);
    assert(Run(0, true) == 0 && leidos == 1);
    assert(!SemVariable("previa").ok);
    char *log = TestReadFile(".build/parser-log.txt");
    TestEqual(log, "TRACE linea=1 columna=1 token=EOF\n"); free(log);
    for (int caso = 1; caso <= 2; ++caso) {
        assert(Run(caso, true) == 2 && leidos == 2);
        log = TestReadFile(".build/parser-log.txt");
        const char *codigo = caso == 1 ? "codigo=E_IO" : "codigo=E_RESOURCE";
        assert(strstr(log, "ERROR linea=1 columna=1 codigo=E_MISSINGNO") != NULL);
        const char *fatal = strstr(log, "ERROR linea=1 columna=2 ");
        assert(fatal != NULL && strstr(fatal, codigo) != NULL);
        assert(strstr(fatal + 1, "ERROR linea=") == NULL);
        free(log);
    }
    assert(Run(3, false) == 2 && leidos == 6);
    assert(SemVariable("energia").ok && SemVariable("energia").valor == 2);
    log = TestReadFile(".build/parser-log.txt");
    assert(strncmp(log, "INFO linea=1 columna=1 accion=asignacion variable=energia valor=2\n", 63) == 0);
    assert(strstr(log, "ERROR linea=1 columna=6 codigo=E_IO") != NULL); free(log);
#ifndef _WIN32
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
