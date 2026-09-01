#ifndef POKEDEX_H
#define POKEDEX_H

#include <stddef.h>
#include <stdint.h>

#define POKEDEX_SIZE 151                 // Definimos el tamaño de la pokédex actual a los primeros 151 pokémons
#define INVALID_POKEMON ((int16_t)-1)    // Definimos nuestro valor de invalidación 

/**
 * @brief Convierte un nombre de Pokémon a su índice en la Pokédex.
 *
 * @pre  name != NULL
 *
 * @post El contenido apuntado por name no es modificado.
 *
 * @param name Cadena con el nombre del Pokémon.
 * @return Índice (0–150) si el nombre es válido,
 *         INVALID_POKEMON si no existe.
 */
int16_t pokemon_to_value(const char *name);

/**
 * @brief Convierte un valor (0–150) al nombre del Pokémon correspondiente.
 *
 * @param value Índice de la Pokédex.
 * @return Nombre del Pokémon, o NULL si el valor es inválido.
 */
const char *value_to_pokemon(int16_t value);

#endif