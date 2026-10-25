/* errors.h - Contrato comun de diagnosticos y registros de PokeCalc.
 * Implementacion agregada; Calc.md, secciones 7, 8 y 9.3. */
#ifndef ERRORS_H
#define ERRORS_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    E_NONE, E_MISSINGNO, E_LEXICAL, E_LIMIT, E_SYNTAX, E_UNDEFINED,
    E_RANGE, E_IO, E_RESOURCE, E_USAGE
} ErrorCode;

typedef enum { ERROR_RECOVERABLE, ERROR_FATAL } ErrorSeverity;
typedef struct { int linea, columna; } SourcePosition;

typedef struct {
    ErrorCode codigo;
    ErrorSeverity gravedad;
    SourcePosition posicion;  /* {0, 0}: sin posicion de fuente. */
    char detalle[256];
} Diagnostic;

typedef enum { INFO_ASIGNACION, INFO_EVALUACION, INFO_LIBERACION } InfoAction;
typedef enum { TRACE_NONE, TRACE_NATURAL, TRACE_IDENTIFIER, TRACE_ERROR } TraceAttribute;
typedef struct {
    const char *clase;
    SourcePosition posicion;
    TraceAttribute atributo;
    int64_t valor;
    bool fuera_de_rango;
    const char *nombre;
    ErrorCode codigo;
} TraceRecord;

const char *ErrorCodeName(ErrorCode codigo);
/* Adaptador de los campos de texto ya existentes en Scanner y Semantics. */
ErrorCode ErrorCodeFromName(const char *nombre);
Diagnostic MakeDiagnostic(ErrorCode codigo, SourcePosition posicion, const char *detalle);

/* false significa fallo de stderr: el llamador termina con estado 2 y no
 * intenta escribir otro diagnostico en ese mismo flujo. No llaman a exit. */
bool ReportError(const Diagnostic *error);
bool ReportInfo(SourcePosition posicion, InfoAction accion, const char *nombre, int64_t valor);
bool ReportTrace(const TraceRecord *traza);

#endif
