/**
 * @file errors_test.c
 * @brief Pruebas unitarias de diagnósticos y del formato de registros en stderr.
 *
 * Se invoca directamente el módulo errors para aislarlo del scanner y parser.
 * La salida esperada permite comprobar escapes de bytes, campos opcionales,
 * gravedad de errores, trazas y recorte explícito de mensajes extensos.
 */
#include "errors.h"
#include "test_support.h"

/**
 * @brief Ejecuta las comprobaciones del contrato común de errores y registros.
 * @return Cero si todas las aserciones se cumplen.
 *
 * stderr se redirige a un archivo binario para comparar los bytes escritos,
 * incluidos los escapes que representan controles y texto UTF-8 de la entrada.
 */
int main(void) {
    assert(freopen(".build/errors-output.txt", "wb", stderr) != NULL);
    /** Un error recuperable y los tres tipos de INFO ejercitan los campos de
     * posición, variable y valor que corresponde incluir en cada registro. */
    Diagnostic diagnostico = MakeDiagnostic(E_MISSINGNO, (SourcePosition){2, 4},
                                            "causa \303\251\n\r\t\\\"\001\177");
    assert(diagnostico.gravedad == ERROR_RECOVERABLE);
    assert(ReportError(&diagnostico));
    assert(ReportInfo((SourcePosition){3, 1}, INFO_ASIGNACION, "energia", -2));
    assert(ReportInfo((SourcePosition){4, 1}, INFO_EVALUACION, NULL, 0));
    assert(ReportInfo((SourcePosition){5, 1}, INFO_LIBERACION, "energia", 0));
    /** La misma estructura de traza cambia de atributo: natural representable,
     * natural fuera de rango, identificador, error y token sin atributo. */
    TraceRecord traza = {0};
    traza.posicion = (SourcePosition){6, 1};
    traza.clase = "NATURAL";
    traza.atributo = TRACE_NATURAL;
    traza.valor = 999999;
    assert(ReportTrace(&traza));
    traza.fuera_de_rango = true;
    assert(ReportTrace(&traza));
    traza.clase = "IDENTIFIER";
    traza.atributo = TRACE_IDENTIFIER;
    traza.nombre = "energia";
    assert(ReportTrace(&traza));
    traza.clase = "ERROR";
    traza.atributo = TRACE_ERROR;
    traza.codigo = E_LEXICAL;
    assert(ReportTrace(&traza));
    traza.clase = "EOF";
    traza.atributo = TRACE_NONE;
    assert(ReportTrace(&traza));
    /** Los fallos de uso son fatales y carecen de una posición en la fuente.
     * La comparación conjunta también fija el orden y formato de las líneas. */
    diagnostico = MakeDiagnostic(E_USAGE, (SourcePosition){0, 0}, "uso: pokecalc [--trace]");
    assert(diagnostico.gravedad == ERROR_FATAL && ReportError(&diagnostico));
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
    /** El contrato externo admite ASCII imprimible y saltos de línea. */
    for (const unsigned char *byte = (const unsigned char *)text; *byte != '\0'; byte++) {
        assert(*byte == '\n' || (*byte >= 32 && *byte <= 126));
    }
    free(text);
    /** Los nombres públicos del catálogo deben conservar su escritura exacta. */
    static const char *const nombresEsperados[] = {
        "E_NONE", "E_MISSINGNO", "E_LEXICAL", "E_LIMIT", "E_SYNTAX",
        "E_UNDEFINED", "E_RANGE", "E_IO", "E_RESOURCE", "E_USAGE"
    };
    for (int codigo = E_NONE; codigo <= E_USAGE; ++codigo) {
        TestEqual(ErrorCodeName((ErrorCode)codigo), nombresEsperados[codigo]);
    }
    /** Un detalle mayor que la capacidad interna conserva una marca visible de
     * recorte y la gravedad fatal asociada a la falta de recursos. */
    char detail[1000];
    memset(detail, 'x', sizeof detail - 1);
    detail[sizeof detail - 1] = '\0';
    diagnostico = MakeDiagnostic(E_RESOURCE, (SourcePosition){1, 1}, detail);
    assert(strstr(diagnostico.detalle, "(recortado)") != NULL && diagnostico.gravedad == ERROR_FATAL);
    puts("errors: assert OK");
    return 0;
}
