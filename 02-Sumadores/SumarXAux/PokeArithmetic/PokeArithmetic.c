#include "PokeArithmetic.h"
#include "../Pokedex/Pokedex.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INITIAL_NUMBER_CAPACITY 10


/* ========================================================================== */
/* Creación y destrucción                                                     */
/* ========================================================================== */

PokemonNumber *create_number(void){
    
    // Creamos un nuevo PokemonNumber y verificamos la reserva de memoria
    PokemonNumber *num = malloc(sizeof(*num));

    if (!num) return NULL;  // Si falla la reserva de memoria, retornamos NULL
    
    // Inicializamos los campos del número
    num->capacity = INITIAL_NUMBER_CAPACITY;
    num->size = 1;

    // Reservamos memoria para los dígitos y verificamos la reserva
    num->digits = calloc(
        num->capacity,
        sizeof(*num->digits)
    );

    // Si falla la reserva de memoria para los dígitos, liberamos el número y retornamos NULL
    if (!num->digits) {
        free(num);
        return NULL;
    }

    /*
     * Mew representa el valor 0.
     */
    num->digits[0] = 0;

    return num;
}


// Libera la memoria asociada a un PokemonNumber
void free_number(PokemonNumber *num){
    if (!num) return;   // Si el puntero es NULL, no hacemos nada

    free(num->digits);  // Liberamos la memoria del arreglo de dígitos
    num->digits = NULL; // Evitamos dangling pointer
    free(num);          // Liberamos la estructura del número
}


/* ========================================================================== */
/* Gestión de capacidad                                                       */
/* ========================================================================== */

// Esta función asegura que el número tenga suficiente capacidad para almacenar al menos 'needed' dígitos.
// Es muy dificil la verdad, pero hago mi mejor intento para explicarlo...
static bool ensure_capacity(PokemonNumber *num, size_t needed){
    if (!num || !num->digits) return false; // Si el puntero es NULL o los dígitos son NULL, retornamos false
    if (needed <= num->capacity) return true; // Si ya tenemos suficiente capacidad, no hacemos nada

    size_t new_capacity = num->capacity; // Empezamos con la capacidad actual

    /*
     * Duplicamos la capacidad hasta disponer del espacio requerido.
     */
    while (new_capacity < needed) {

        /*
         * Evitamos overflow en size_t.
         */
        if (new_capacity > SIZE_MAX / 2) {
            new_capacity = needed;
            break;
        }

        new_capacity *= 2;
    }


    /*
     * Verificamos overflow en el cálculo del tamaño en bytes.
     */
    if (new_capacity > SIZE_MAX / sizeof(*num->digits)) return false; // Si hay overflow, retornamos false

    // Redimensionamos el arreglo de dígitos con realloc
    int16_t *new_digits = realloc(
        num->digits,
        new_capacity * sizeof(*num->digits)
    );

    // Si realloc falla, retornamos false
    if (!new_digits) return false;

    /*
     * Inicializamos la nueva región.
     */
    for (size_t i = num->capacity; i < new_capacity; ++i) {
        new_digits[i] = 0;
    }

    // Actualizamos el puntero y la capacidad del número
    num->digits = new_digits;
    num->capacity = new_capacity;


    return true;
}


/* ========================================================================== */
/* Suma                                                                       */
/* ========================================================================== */

// Suma un único dígito Pokémon al número acumulador.
bool add_digit_to_number(PokemonNumber *result, int16_t digit){
    
    if (!result || !result->digits) return false;                           // Validación de punteros
    if (result->size == 0 || result->size > result->capacity) return false; // Validación de tamaño
    if (digit < 0 || digit >= POKEDEX_SIZE) return false;                   // Validamos que el operando sea un dígito válido de base 151.

    /*
     * El operando entra como valor inicial a sumar.
     *
     * Ejemplo:
     *
     *     result = [149]
     *     digit  = 5
     *
     *     149 + 5 = 154
     *
     *     digit = 154 % 151 = 3
     *     carry = 154 / 151 = 1
     *
     *     result = [3, 1]
     */
    int carry = digit;

    size_t position = 0;

    while (carry > 0) {

        /*
         * Si llegamos a una posición que todavía no existe,
         * la agregamos.
         */
        if (position >= result->size) {

            if (!ensure_capacity(result, position + 1)) return false;
            
            result->digits[position] = 0;
            result->size++;
        }

        /*
         * Suma de la posición actual + carry.
         */
        int sum =
            result->digits[position] +
            carry;


        /*
         * Conversión a base 151:
         *
         * remainder = dígito actual
         * quotient  = carry
         */
        result->digits[position] = (int16_t)(sum % POKEDEX_SIZE);

        carry = sum / POKEDEX_SIZE;


        position++;
    }

    return true;
}


/* ========================================================================== */
/* Normalización                                                              */
/* ========================================================================== */

void normalize_number(PokemonNumber *num) {
    if (!num || !num->digits || num->size == 0) return; // Validación de punteros y tamaño
    
    /*
     * Eliminamos ceros no significativos.
     *
     * Nunca eliminamos el último dígito porque:
     *
     *     [0] = Mew = 0
     */
    while (num->size > 1 && num->digits[num->size - 1] == 0) num->size--;
    
}


/* ========================================================================== */
/* Impresión                                                                  */
/* ========================================================================== */

void print_number(const PokemonNumber *num){
    if (!num || !num->digits || num->size == 0) {
        fprintf(stderr, "ERROR: número corrupto\n");
        return;
    }

    if (num->size > num->capacity) {
        fprintf(stderr, "ERROR: número corrupto\n");
        return;
    }

    /*
     * Imprimimos MSD → LSD.
     */
    for (size_t i = num->size; i > 0; --i) {

        int16_t digit = num->digits[i - 1];

        if (digit < 0 || digit >= POKEDEX_SIZE) {
            fprintf(stderr, "ERROR: dígito inválido %d\n", digit);
            return;
        }

        const char *name = value_to_pokemon(digit);

        if (!name) {
            fprintf(stderr, "ERROR: dígito inválido %d\n", digit);
            return;
        }

        printf("%s", name);

        if (i > 1) printf(" | ");
    }

    printf("\n");
}