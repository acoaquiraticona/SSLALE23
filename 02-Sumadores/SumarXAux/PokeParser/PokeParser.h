#ifndef POKE_PARSER_H
#define POKE_PARSER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "ParserErrors.h"

/*
 * Longitud máxima razonable para un nombre de Pokémon.
 *
 * Esto NO limita la cantidad de operandos.
 * Solamente limita el tamaño individual del token.
 */
#define POKEMON_TOKEN_MAX 127

/**
 * @brief Callback utilizado para procesar cada operando encontrado.
 *
 * El parser llama a esta función una vez por cada Pokémon válido.
 *
 * @param value   Valor numérico del Pokémon [0, 150].
 * @param context Contexto definido por quien utiliza el parser.
 *
 * @return true si el operando fue procesado correctamente.
 *         false si ocurrió un error durante su procesamiento.
 */
typedef bool (*OperandConsumer)(int16_t value, void *context);

/**
 * @brief Resultado del análisis de una expresión.
 */
typedef struct {
    ParseStatus status;
    char error_message[256];
} ParseResult;

/**
 * @brief Procesa una expresión directamente desde un flujo.
 *
 * Formato:
 *
 *     Pokemon
 *     Pokemon + Pokemon
 *     Pokemon + Pokemon + Pokemon
 *     ...
 *
 * Los operandos se procesan uno por uno mediante el callback `consumer`.
 *
 * El parser NO almacena la expresión completa ni un array de operandos.
 *
 * Una expresión termina mediante:
 *
 *     '\n' -> siguiente expresión
 *     EOF  -> última expresión
 *
 * @param input    Flujo de entrada.
 * @param consumer Función que recibe cada operando.
 * @param context  Contexto pasado al callback.
 *
 * @return PARSE_OK si la expresión fue procesada correctamente.
 *         PARSE_EOF si no existe otra expresión.
 *         Otro código en caso de error.
 */
ParseResult parse_expression(FILE *input, OperandConsumer consumer, void *context);

#endif /* POKE_PARSER_H */