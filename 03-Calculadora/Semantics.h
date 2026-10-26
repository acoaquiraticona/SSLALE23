/**
 * @file Semantics.h
 * @brief Contrato de evaluación, variables y representación de resultados.
 *
 * El parser reconoce la estructura de las sentencias y delega aquí su
 * significado: validar naturales, resolver variables, operar con enteros y
 * convertirlos a nombres Pokémon. Los valores del lenguaje pertenecen a
 * [-999999, 999999]; las operaciones reciben operandos previamente validados.
 * Este módulo devuelve resultados y diagnósticos, sin escribir en los flujos.
 *
 * La tabla de símbolos es privada y pertenece a una sesión. El parser llama
 * a SemReset() al comenzar y posterga las modificaciones hasta comprobar el
 * punto y coma de una sentencia válida.
 */
#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "errors.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** @brief Mayor entero admitido como literal o resultado. */
#define SEM_VALOR_MAXIMO  999999
/** @brief Menor entero admitido como resultado. */
#define SEM_VALOR_MINIMO  (-999999)
/** @brief Cantidad máxima de variables simultáneamente ocupadas. */
#define SEM_MAX_VARIABLES 32
/** @brief Longitud máxima de un identificador, sin su terminador nulo. */
#define SEM_MAX_NOMBRE    31

/**
 * @brief Resultado de una operación o de un acceso a la tabla de símbolos.
 *
 * El llamador consulta primero @c ok. Ante un fallo, el diagnóstico se crea
 * sin posición de fuente ({0, 0}): el parser agrega la posición del literal,
 * identificador u operador responsable antes de informarlo.
 */
typedef struct {
    bool ok;                    /**< Indica si la operación fue aceptada. */
    int64_t valor;               /**< Resultado utilizable cuando @c ok es true. */
    Diagnostic diagnostico;     /**< Error común compartido con scanner y parser. */
}ResultadoSem;

/**
 * @brief Valida el dominio de un literal natural reconocido por el scanner.
 * @param valor Magnitud acumulada al leer los dígitos Pokémon.
 * @param fuera_de_rango Marca del scanner que conserva un exceso de rango,
 *        aunque @p valor ya no represente el número completo de la entrada.
 * @return El natural validado o E_RANGE si la marca está activa, es negativo
 *         o supera 999999.
 */
ResultadoSem SemNatural(int64_t valor, bool fuera_de_rango);
/**
 * @brief Consulta una variable sin modificar la tabla.
 * @param nombre Identificador terminado en nulo; distingue mayúsculas.
 * @return Su valor actual o E_UNDEFINED si no hay una entrada ocupada con ese nombre.
 */
ResultadoSem SemVariable(const char *nombre);

/**
 * @brief Evalúa el operador binario @c plusle.
 * @param primerSumando Operando izquierdo dentro del dominio del lenguaje.
 * @param segundoSumando Operando derecho dentro del dominio del lenguaje.
 * @return La suma o E_RANGE si el resultado sale del dominio.
 */
ResultadoSem SemSumar(int64_t primerSumando, int64_t segundoSumando);
/**
 * @brief Evalúa el operador binario @c minun.
 * @param minuendo Operando izquierdo dentro del dominio del lenguaje.
 * @param sustraendo Operando derecho que se resta al izquierdo.
 * @pre Ambos operandos pertenecen al dominio del lenguaje.
 * @return La diferencia o E_RANGE si el resultado sale del dominio.
 */
ResultadoSem SemRestar(int64_t minuendo, int64_t sustraendo);
/**
 * @brief Evalúa el operador binario @c tandemaus.
 * @param multiplicando Operando izquierdo dentro del dominio del lenguaje.
 * @param multiplicador Operando derecho dentro del dominio del lenguaje.
 * @return El producto o E_RANGE si el resultado sale del dominio.
 */
ResultadoSem SemMultiplicar(int64_t multiplicando, int64_t multiplicador);
/**
 * @brief Aplica @c minun unario, invirtiendo el signo de un valor validado.
 * @param operando Entero dentro del dominio del lenguaje.
 * @return El opuesto validado; el cero sigue representando cero.
 */
ResultadoSem SemNegar(int64_t operando);
/**
 * @brief Implementa @c ditto, que duplica su argumento.
 * @param operando Entero dentro del dominio del lenguaje.
 * @return El doble o E_RANGE si excede el dominio permitido.
 */
ResultadoSem SemDuplicar(int64_t operando);

/**
 * @brief Crea una variable o reemplaza el valor de una existente.
 * @param nombre Identificador válido de hasta SEM_MAX_NOMBRE caracteres.
 * @param valor Valor ya validado de la expresión a asignar.
 * @pre El parser comprobó la sentencia completa, incluido su punto y coma.
 * @return El valor guardado o E_LIMIT si un nombre nuevo encuentra la tabla llena.
 *
 * Una reasignación reutiliza su entrada y puede realizarse con la tabla llena.
 * La expresión se evalúa antes de llamar a esta función, por lo que puede leer
 * el valor anterior de la misma variable.
 */
ResultadoSem SemAsignar(const char *nombre, int64_t valor);

/**
 * @brief Libera la entrada ocupada por una variable.
 * @param nombre Identificador que se desea eliminar.
 * @pre El parser comprobó la sentencia completa, incluido su punto y coma.
 * @return Éxito con valor cero o E_UNDEFINED si la variable no existe.
 * @note El espacio liberado puede reutilizarse en una asignación posterior.
 */
ResultadoSem SemLiberar(const char *nombre);

/** @brief Vacía la tabla para que una nueva sesión no herede variables. */
void SemReset(void);

/**
 * @brief Escribe un entero en la representación canónica del lenguaje.
 * @param valor Entero dentro de [-999999, 999999].
 * @param[out] destino Búfer que recibe nombres entre corchetes y, si el valor
 *        es negativo, el prefijo @c minun seguido de un espacio.
 * @param tamanio Capacidad del destino en bytes, incluido el terminador nulo.
 * @pre @p destino permite escribir @p tamanio bytes; puede ser NULL si es cero.
 *
 * No agrega el valor decimal ni un salto de línea. El cero se escribe
 * como [mew], sin ceros iniciales. Si falta espacio, snprintf limita la salida
 * y la termina en nulo cuando la capacidad es positiva; no se informa el recorte.
 */
void SemAPokemon(int64_t valor, char *destino, size_t tamanio);
#endif
