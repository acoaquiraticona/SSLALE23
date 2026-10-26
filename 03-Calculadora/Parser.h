/**
 * @file Parser.h
 * @brief Interfaz del analizador sintáctico y coordinador de la interpretación.
 *
 * El parser solicita tokens al scanner, comprueba la gramática y llama al módulo
 * semántico para evaluar expresiones y administrar variables. Cada invocación
 * interpreta una sesión completa desde la posición actual de `stdin` hasta EOF
 * o hasta un error fatal; los resultados van a `stdout` y los registros a `stderr`.
 */

#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>
/**
 * @brief Interpreta una sesión con una tabla de variables inicialmente vacía.
 *
 * Reinicia los estados del scanner, del parser y del módulo semántico. Ante un
 * error recuperable rechaza la sentencia, busca su punto y coma y continúa con
 * la siguiente. Un error fatal detiene la sesión. No reposiciona `stdin`.
 *
 * @param trace Si es `true`, registra cada token solicitado, incluido EOF.
 * @retval 0 Se alcanzó EOF sin rechazar sentencias.
 * @retval 1 Se alcanzó EOF después de uno o más errores recuperables.
 * @retval 2 La interpretación se interrumpió por un error fatal.
 * @note El estado interno es compartido; no admite sesiones concurrentes ni
 *       llamadas recursivas a esta función.
 */
int ParserRun(bool trace);
#endif
