/**
 * @file Scanner.h
 * @brief Contrato del analizador léxico de la PokéCalculadora.
 *
 * El scanner transforma los bytes de stdin en tokens: reconoce los nombres de
 * los operadores, los identificadores y los números escritos con Pokémon.
 * Cada token conserva su posición inicial y los atributos que necesita el
 * parser para interpretar la entrada o informar un error. No evalúa expresiones
 * ni modifica variables; esas tareas corresponden al parser y a Semantics.
 *
 * @note La lectura usa estado global privado y una sola entrada estándar:
 * las llamadas pertenecen a una misma sesión y no son reentrantes.
 */
#ifndef SCANNER_H
#define SCANNER_H

#include "errors.h"
#include <stdint.h>
#include <stdbool.h>

/** @brief Categorías léxicas que el parser distingue al aplicar la gramática. */
typedef enum {
    TOKEN_EOF,          /**< Fin de entrada; no representa un error por sí mismo. */
    TOKEN_ERROR,        /**< Entrada inválida o fallo fatal descrito en diagnostico. */
    TOKEN_NATURAL,      /**< Literal Pokémon entre corchetes, sin signo. */
    TOKEN_IDENTIFIER,   /**< Nombre de variable que respeta la forma y longitud permitidas. */
    TOKEN_PLUSLE,       /**< Operador binario de suma. */
    TOKEN_MINUN,        /**< Resta binaria o negación unaria, según el contexto sintáctico. */
    TOKEN_TANDEMAUS,    /**< Operador binario de multiplicación. */
    TOKEN_DITTO,        /**< Nombre de la función que duplica una expresión. */
    TOKEN_CAPTURA,      /**< Inicio de una asignación o declaración implícita. */
    TOKEN_LIBERAR,      /**< Inicio de una eliminación de variable. */
    TOKEN_EQUAL,        /**< Separador entre nombre y expresión en una captura. */
    TOKEN_SEMICOLON,    /**< Cierre de sentencia y punto de recuperación del parser. */
    TOKEN_LPAREN,       /**< Apertura de agrupación o de argumento de ditto. */
    TOKEN_RPAREN        /**< Cierre de agrupación o de argumento de ditto. */
} TipoToken;

/**
 * @brief Unidad reconocida, con su ubicación, texto y atributos de interpretación.
 *
 * Los campos se inicializan a cero al crear el token y se completan según tipo.
 * Para TOKEN_NATURAL, valor sólo representa el número completo si
 * fuera_de_rango es false. Para TOKEN_ERROR, diagnostico es la descripción
 * estructurada que comparten los módulos e incluye código, posición y detalle.
 *
 * Los arreglos de char guardan cadenas de C: el byte '\0' marca su final.
 * Sus capacidades incluyen ese terminador, de modo que lexema, por ejemplo,
 * puede conservar como máximo 127 bytes de texto y el NUL de cierre.
 */
typedef struct {
    TipoToken tipo;       /**< Categoría que guía las decisiones del parser. */
    int linea;           /**< Línea inicial, numerada desde uno. */
    int columna;         /**< Columna inicial en bytes, numerada desde uno. */

    int64_t valor;        /**< Acumulador decimal del literal Pokémon. */
    bool fuera_de_rango; /**< Indica un natural mayor que 999999, aunque su forma sea válida. */

    Diagnostic diagnostico; /**< Código, posición y detalle común de un error. */
    bool lexema_recortado;  /**< El texto guardado es una muestra abreviada del lexema real. */
    char lexema[128];       /**< Texto reconocido, terminado en NUL; incluye marca si se recorta. */
} Token;

/**
 * @brief Inicia una nueva sesión léxica con posición 1:1 y sin anticipación.
 *
 * Descarta el byte guardado y las marcas internas de EOF, error y límites.
 * No lee, reposiciona ni limpia los indicadores de stdin: quien cambie la
 * fuente debe preparar ese flujo antes de comenzar a pedir tokens.
 */
void ScannerReset(void);

/**
 * @brief Obtiene una etiqueta estable para diagnósticos y trazas de tokens.
 * @param tipo Categoría que se desea nombrar.
 * @return Cadena estática que no debe liberarse; ERROR para valores sin nombre.
 */
const char *ScannerTokenName(TipoToken tipo);

/**
 * @brief Reconoce el próximo token de stdin y avanza sobre su texto.
 *
 * Omite espacios y comentarios de línea antes de reconocer una unidad.
 * Distingue los errores de forma léxica de los naturales bien formados pero
 * fuera de rango; estos últimos conservan TOKEN_NATURAL para que la semántica
 * resuelva su rechazo. Un fallo de lectura o de posición tiene prioridad
 * sobre el token que se estaba construyendo.
 *
 * @return Token con posición inicial; TOKEN_EOF al terminar la entrada o
 * TOKEN_ERROR con diagnóstico cuando el reconocimiento falla.
 */
Token GetNextToken(void);

#endif
