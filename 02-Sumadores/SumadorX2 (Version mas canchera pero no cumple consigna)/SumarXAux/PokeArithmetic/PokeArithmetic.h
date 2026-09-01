#ifndef POKE_ARITHMETIC_H
#define POKE_ARITHMETIC_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Número de precisión arbitraria representado en base 151.
 *
 * digits[0]      = dígito menos significativo (LSD)
 * digits[size-1] = dígito más significativo (MSD)
 *
 * Cada dígito pertenece al intervalo [0, 150].
 */
typedef struct {
    int16_t *digits;
    size_t size;
    size_t capacity;
} PokemonNumber;

/**
 * @brief Crea un número inicializado en cero (Mew).
 *
 * @return Puntero a PokemonNumber o NULL si falla la memoria.
 */
PokemonNumber *create_number(void);

/**
 * @brief Libera un PokemonNumber.
 *
 * @param num Número que se desea liberar.
 */
void free_number(PokemonNumber *num);

/**
 * @brief Suma un único operando Pokémon al acumulador.
 *
 * El valor recibido debe pertenecer al intervalo [0, 150].
 *
 * La operación utiliza aritmética en base 151 y propaga el carry por
 * todas las posiciones necesarias.
 *
 * @param result Número acumulador.
 * @param digit  Valor numérico del Pokémon.
 *
 * @return true si la suma fue exitosa.
 *         false si hubo un error.
 */
bool add_digit_to_number(
    PokemonNumber *result,
    int16_t digit
);

/**
 * @brief Normaliza la representación del número.
 *
 * Elimina dígitos Mew (0) no significativos de las posiciones superiores.
 */
void normalize_number(PokemonNumber *num);

/**
 * @brief Imprime el número en formato Pokémon.
 *
 * Los dígitos se imprimen desde el más significativo al menos significativo.
 */
void print_number(const PokemonNumber *num);

#endif