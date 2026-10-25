/* El escritor se prueba directamente, con controles y UTF-8 sin normalizar. */
#include "errors.h"
#include "test_support.h"

int main(void) {
    assert(freopen(".build/errors-output.txt", "wb", stderr) != NULL);
    Diagnostic d = MakeDiagnostic(E_MISSINGNO, (SourcePosition){2, 4},
                                  "causa \303\251\n\r\t\\\"\001\177");
    assert(d.gravedad == ERROR_RECOVERABLE);
    assert(ReportError(&d));
    assert(ReportInfo((SourcePosition){3, 1}, INFO_ASIGNACION, "energia", -2));
    assert(ReportInfo((SourcePosition){4, 1}, INFO_EVALUACION, NULL, 0));
    assert(ReportInfo((SourcePosition){5, 1}, INFO_LIBERACION, "energia", 0));
    TraceRecord t = {0};
    t.posicion = (SourcePosition){6, 1};
    t.clase = "NATURAL";
    t.atributo = TRACE_NATURAL;
    t.valor = 999999;
    assert(ReportTrace(&t));
    t.fuera_de_rango = true;
    assert(ReportTrace(&t));
    t.clase = "IDENTIFIER"; t.atributo = TRACE_IDENTIFIER; t.nombre = "energia";
    assert(ReportTrace(&t));
    t.clase = "ERROR"; t.atributo = TRACE_ERROR; t.codigo = E_LEXICAL;
    assert(ReportTrace(&t));
    t.clase = "EOF"; t.atributo = TRACE_NONE;
    assert(ReportTrace(&t));
    d = MakeDiagnostic(E_USAGE, (SourcePosition){0, 0}, "uso: pokecalc [--trace]");
    assert(d.gravedad == ERROR_FATAL && ReportError(&d));
    char *text = TestReadFile(".build/errors-output.txt");
    TestEqual(text,
        "ERROR linea=2 columna=4 codigo=E_MISSINGNO mensaje=\"causa \\xC3\\xA9\\n\\r\\t\\\\\\\"\\x01\\x7F\"\n"
        "INFO linea=3 columna=1 accion=asignacion variable=energia valor=-2\n"
        "INFO linea=4 columna=1 accion=evaluacion valor=0\n"
        "INFO linea=5 columna=1 accion=liberacion variable=energia\n"
        "TRACE linea=6 columna=1 token=NATURAL valor=999999\n"
        "TRACE linea=6 columna=1 token=NATURAL fuera_de_rango=1\n"
        "TRACE linea=6 columna=1 token=IDENTIFIER nombre=energia\n"
        "TRACE linea=6 columna=1 token=ERROR codigo=E_LEXICAL\n"
        "TRACE linea=6 columna=1 token=EOF\n"
        "ERROR codigo=E_USAGE mensaje=\"uso: pokecalc [--trace]\"\n");
    for (const unsigned char *p = (const unsigned char *)text; *p; ++p)
        assert(*p == '\n' || (*p >= 32 && *p <= 126));
    free(text);
    for (int i = E_NONE; i <= E_USAGE; ++i)
        assert(ErrorCodeFromName(ErrorCodeName((ErrorCode)i)) == (ErrorCode)i);
    char detail[1000];
    memset(detail, 'x', sizeof detail - 1); detail[sizeof detail - 1] = 0;
    d = MakeDiagnostic(E_RESOURCE, (SourcePosition){1, 1}, detail);
    assert(strstr(d.detalle, "(recortado)") != NULL && d.gravedad == ERROR_FATAL);
    puts("errors: assert OK");
    return 0;
}
