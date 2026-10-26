/**
 * @file errors.h
 * @brief Tipos comunes de diagnóstico y contrato de los registros de PokéCalc.
 *
 * Scanner, parser y semántica detectan los problemas de su etapa y transportan
 * los datos mediante estos tipos. Este módulo centraliza sus nombres, gravedad
 * y escritura en stderr; el parser o main deciden cuándo informar, recuperar
 * una sentencia o terminar la sesión. Ninguna función de este módulo llama a exit.
 *
 * Los registros ERROR, INFO y TRACE contienen ASCII imprimible y un LF final.
 * Los textos variables se escapan por byte, sin interpretar UTF-8. En Windows,
 * main configura stderr en modo binario para conservar ese LF sin traducción.
 * Las reglas completas del lenguaje se encuentran en Calc.md, secciones 7 a 9.
 */
#ifndef ERRORS_H
#define ERRORS_H

#include <stdbool.h>
#include <stdint.h>

/** @brief Códigos compartidos para identificar la causa sin analizar el mensaje. */
typedef enum {
    E_NONE,      /**< Ausencia de error; valor inicial de estructuras sin diagnóstico. */
    E_MISSINGNO, /**< Representación numérica no admitida o literal Pokémon mal formado. */
    E_LEXICAL,   /**< Byte que no pertenece a ninguna unidad léxica reconocida. */
    E_LIMIT,     /**< Nombre de más de 31 caracteres o tabla de 32 variables llena. */
    E_SYNTAX,    /**< Secuencia de tokens que no satisface la gramática. */
    E_UNDEFINED, /**< Consulta o liberación de una variable que no existe. */
    E_RANGE,     /**< Literal o resultado aritmético fuera de [-999999, 999999]. */
    E_IO,        /**< Fallo fatal al configurar, leer o escribir un flujo. */
    E_RESOURCE,  /**< Recurso de ejecución agotado o fallo interno sin código reconocido. */
    E_USAGE      /**< Argumentos del ejecutable que no admite su interfaz. */
} ErrorCode;

/** @brief Clasificación que permite al llamador decidir si continúa la sesión. */
typedef enum {
    ERROR_RECOVERABLE, /**< Se rechaza la sentencia; el parser puede recuperar hasta ; o EOF. */
    ERROR_FATAL        /**< La sesión debe terminar con estado 2. */
} ErrorSeverity;

/**
 * @brief Ubicación de una unidad de la fuente, con numeración desde uno.
 * @details La columna cuenta bytes; {0, 0} indica que el problema no tiene
 * posición en la entrada, como ocurre con los argumentos del ejecutable.
 */
typedef struct {
    int linea;   /**< Línea inicial; cero si no corresponde. */
    int columna; /**< Columna inicial en bytes; cero si no corresponde. */
} SourcePosition;

/**
 * @brief Descripción autocontenida de un error, sin escribir todavía en ningún flujo.
 * @details El detalle tiene almacenamiento propio: puede sobrevivir a los
 * búferes temporales del detector. MakeDiagnostic determina gravedad a partir
 * del código; ReportError imprime código, posición y detalle, sin serializar
 * el campo gravedad.
 */
typedef struct {
    ErrorCode codigo;          /**< Causa común del problema. */
    ErrorSeverity gravedad;   /**< Fatal para E_IO, E_RESOURCE y E_USAGE; recuperable para el resto. */
    SourcePosition posicion;  /**< Inicio del problema; {0, 0} omite la ubicación del ERROR. */
    char detalle[256];        /**< Mensaje terminado en NUL, con marca visible si fue recortado. */
} Diagnostic;

/** @brief Acción confirmada por una sentencia correcta y registrada como INFO. */
typedef enum {
    INFO_ASIGNACION, /**< Captura completada: se informa variable y valor. */
    INFO_EVALUACION, /**< Expresión evaluada: se informa sólo su valor. */
    INFO_LIBERACION  /**< Variable eliminada: se informa sólo su nombre. */
} InfoAction;

/** @brief Selección de los atributos que se agregan a una línea TRACE. */
typedef enum {
    TRACE_NONE,       /**< Token sin atributos adicionales, incluido EOF. */
    TRACE_NATURAL,    /**< Valor decimal o marca fuera_de_rango=1. */
    TRACE_IDENTIFIER, /**< Nombre del identificador. */
    TRACE_ERROR       /**< Código del token de error, sin su detalle. */
} TraceAttribute;

/**
 * @brief Datos de un token para escribir su traza sin depender de Scanner.h.
 * @details El llamador debe hacer coincidir clase y atributo: el serializador
 * no los deduce ni comprueba su coherencia. Los punteros apuntan a texto ajeno
 * y sólo deben permanecer válidos durante la llamada a ReportTrace. Un campo
 * const char * guarda la dirección de una cadena, no una copia de sus letras;
 * const indica que esta interfaz no modifica ese texto. La memoria permanece a
 * cargo de quien proporciona la cadena: ReportTrace no llama a free sobre ella.
 */
typedef struct {
    const char *clase;        /**< Nombre de clase, como NATURAL o el signo entre comillas simples. */
    SourcePosition posicion; /**< Ubicación inicial del token. */
    TraceAttribute atributo; /**< Decide qué campos siguientes se usan. */
    int64_t valor;            /**< Valor para TRACE_NATURAL si fuera_de_rango es false. */
    bool fuera_de_rango;      /**< En un natural, sustituye valor por la marca de rango. */
    const char *nombre;      /**< Texto utilizado únicamente por TRACE_IDENTIFIER. */
    ErrorCode codigo;        /**< Código utilizado únicamente por TRACE_ERROR. */
} TraceRecord;

/**
 * @brief Obtiene el nombre estable de un código para los registros externos.
 * @param codigo Código que se desea representar.
 * @return Cadena estática que no se libera; E_RESOURCE si el valor no está en el catálogo.
 */
const char *ErrorCodeName(ErrorCode codigo);

/**
 * @brief Adapta los códigos textuales anteriores al catálogo común.
 * @param nombre Nombre exacto terminado en NUL; se admite NULL.
 * @return Código correspondiente, o E_RESOURCE si nombre es NULL o desconocido.
 * @details La comparación distingue mayúsculas y conserva compatibilidad con
 * los campos de texto de Scanner y Semantics, sin mantener otro catálogo.
 */
ErrorCode ErrorCodeFromName(const char *nombre);

/**
 * @brief Construye un diagnóstico y conserva su mensaje en un búfer propio.
 * @param codigo Causa del catálogo común.
 * @param posicion Inicio del problema o {0, 0} si no existe ubicación en la fuente.
 * @param detalle Mensaje terminado en NUL; NULL o vacío selecciona un texto genérico.
 * @return Diagnóstico por valor, fatal para E_IO, E_RESOURCE o E_USAGE y
 * recuperable para los demás códigos.
 * @details Copia hasta la capacidad de detalle y reemplaza el final por
 * "... (recortado)" si no cabe el mensaje completo. El recorte se hace por byte;
 * no se interpreta ni valida la codificación del texto recibido.
 */
Diagnostic MakeDiagnostic(ErrorCode codigo, SourcePosition posicion, const char *detalle);

/**
 * @brief Escribe un diagnóstico ERROR en stderr y vacía el flujo.
 * @param error Diagnóstico válido; el puntero no puede ser NULL.
 * @return true si se escribió el registro completo; false ante un fallo de salida.
 * @details La forma es ERROR [linea=L columna=C] codigo=E_... mensaje="texto".
 * Sólo incluye ubicación si línea y columna son positivas. El mensaje usa
 * escapes ASCII por byte y protege comillas y barras inversas; un detalle vacío
 * se sustituye por una causa genérica. El registro termina en LF.
 * @note Si devuelve false, el llamador termina con estado 2 y evita intentar
 * otro diagnóstico sobre el mismo stderr que acaba de fallar.
 */
bool ReportError(const Diagnostic *error);

/**
 * @brief Registra una sentencia confirmada mediante una línea INFO en stderr.
 * @param posicion Inicio de la sentencia, siempre incluido en el registro.
 * @param accion Una de las acciones de InfoAction.
 * @param nombre Variable afectada en asignación/liberación; no se usa en evaluación.
 * @param valor Resultado en asignación/evaluación; no se usa en liberación.
 * @return true si se escribió y vació el registro; false ante fallo de salida
 * o una acción ajena a InfoAction.
 * @details Escribe INFO linea=L columna=C accion=asignacion|evaluacion|liberacion.
 * La asignación agrega variable y valor, la evaluación sólo valor y la
 * liberación sólo variable. Los textos se escapan en ASCII y se termina con LF.
 * @note Con argumentos válidos, false indica que se debe terminar con estado 2
 * sin volver a escribir un diagnóstico en stderr. Esta función no llama a exit.
 */
bool ReportInfo(SourcePosition posicion, InfoAction accion, const char *nombre, int64_t valor);

/**
 * @brief Escribe los datos seleccionados de un token como una línea TRACE.
 * @param traza Registro válido con clase y atributo coherentes; puntero no NULL.
 * @return true si se escribió y vació el registro completo; false ante fallo de salida.
 * @details La base es TRACE linea=L columna=C token=CLASE. TRACE_NATURAL agrega
 * valor o fuera_de_rango=1, TRACE_IDENTIFIER agrega nombre, TRACE_ERROR agrega
 * codigo y TRACE_NONE no agrega nada. Los campos no seleccionados se ignoran.
 * Escapa el texto por byte, termina con LF y no decide si la traza está habilitada:
 * el parser sólo la solicita cuando corresponde a la opción --trace.
 * @note Un false exige terminar con estado 2 sin intentar otro mensaje en el
 * mismo stderr. Esta función no termina el proceso por su cuenta.
 */
bool ReportTrace(const TraceRecord *traza);

#endif
