/* errors.c - Unico escritor de registros de stderr; siempre ASCII y LF. */
#include "errors.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static const char *const nombres[] = {
    "E_NONE", "E_MISSINGNO", "E_LEXICAL", "E_LIMIT", "E_SYNTAX",
    "E_UNDEFINED", "E_RANGE", "E_IO", "E_RESOURCE", "E_USAGE"
};

const char *ErrorCodeName(ErrorCode codigo) {
    if ((unsigned)codigo >= sizeof nombres / sizeof nombres[0]) return "E_RESOURCE";
    return nombres[codigo];
}

ErrorCode ErrorCodeFromName(const char *nombre) {
    if (nombre != NULL)
        for (size_t i = 0; i < sizeof nombres / sizeof nombres[0]; ++i)
            if (strcmp(nombre, nombres[i]) == 0) return (ErrorCode)i;
    return E_RESOURCE;
}

Diagnostic MakeDiagnostic(ErrorCode codigo, SourcePosition posicion, const char *detalle) {
    Diagnostic resultado = {0};
    resultado.codigo = codigo;
    resultado.gravedad = (codigo == E_IO || codigo == E_RESOURCE || codigo == E_USAGE)
        ? ERROR_FATAL : ERROR_RECOVERABLE;
    resultado.posicion = posicion;
    int n = snprintf(resultado.detalle, sizeof resultado.detalle, "%s",
                     detalle != NULL && *detalle != '\0' ? detalle : "Fallo sin detalle adicional.");
    if (n >= (int)sizeof resultado.detalle) {
        static const char marca[] = "... (recortado)";
        memcpy(resultado.detalle + sizeof resultado.detalle - sizeof marca, marca, sizeof marca);
    }
    return resultado;
}

/* No se interpreta UTF-8: se codifica cada byte, como pide Calc.md 7.2. */
static bool TextoASCII(const char *texto, bool entreComillas) {
    const unsigned char *p = (const unsigned char *)(texto != NULL ? texto : "");
    for (; *p != 0; ++p) {
        const char *escape = NULL;
        switch (*p) {
            case '\n': escape = "\\n"; break;
            case '\r': escape = "\\r"; break;
            case '\t': escape = "\\t"; break;
            case '\\': if (entreComillas) escape = "\\\\"; break;
            case '"': if (entreComillas) escape = "\\\""; break;
            default: break;
        }
        if (escape != NULL) {
            if (fputs(escape, stderr) == EOF) return false;
        } else if (*p < 0x20 || *p > 0x7e) {
            if (fprintf(stderr, "\\x%02X", (unsigned)*p) < 0) return false;
        } else if (fputc(*p, stderr) == EOF) {
            return false;
        }
    }
    return true;
}

static bool FinRegistro(void) {
    return fputc('\n', stderr) != EOF && fflush(stderr) == 0;
}

bool ReportError(const Diagnostic *error) {
    if (fputs("ERROR", stderr) == EOF) return false;
    if (error->posicion.linea > 0 && error->posicion.columna > 0 &&
        fprintf(stderr, " linea=%d columna=%d", error->posicion.linea,
                error->posicion.columna) < 0) return false;
    if (fprintf(stderr, " codigo=%s mensaje=\"", ErrorCodeName(error->codigo)) < 0 ||
        !TextoASCII(error->detalle[0] ? error->detalle : "Fallo sin detalle adicional.", true) ||
        fputc('"', stderr) == EOF) return false;
    return FinRegistro();
}

bool ReportInfo(SourcePosition posicion, InfoAction accion, const char *nombre, int64_t valor) {
    static const char *const acciones[] = {"asignacion", "evaluacion", "liberacion"};
    if ((unsigned)accion >= sizeof acciones / sizeof acciones[0]) return false;
    if (fprintf(stderr, "INFO linea=%d columna=%d accion=%s", posicion.linea,
                posicion.columna, acciones[accion]) < 0) return false;
    if (accion != INFO_EVALUACION &&
        (fputs(" variable=", stderr) == EOF || !TextoASCII(nombre, false))) return false;
    if (accion != INFO_LIBERACION && fprintf(stderr, " valor=%" PRId64, valor) < 0) return false;
    return FinRegistro();
}

bool ReportTrace(const TraceRecord *traza) {
    if (fprintf(stderr, "TRACE linea=%d columna=%d token=", traza->posicion.linea,
                traza->posicion.columna) < 0 || !TextoASCII(traza->clase, false)) return false;
    switch (traza->atributo) {
        case TRACE_NATURAL:
            if (traza->fuera_de_rango) {
                if (fputs(" fuera_de_rango=1", stderr) == EOF) return false;
            } else if (fprintf(stderr, " valor=%" PRId64, traza->valor) < 0) return false;
            break;
        case TRACE_IDENTIFIER:
            if (fputs(" nombre=", stderr) == EOF || !TextoASCII(traza->nombre, false)) return false;
            break;
        case TRACE_ERROR:
            if (fprintf(stderr, " codigo=%s", ErrorCodeName(traza->codigo)) < 0) return false;
            break;
        case TRACE_NONE: break;
    }
    return FinRegistro();
}
