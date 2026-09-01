/*
 * ============================================================================
 * Tests de integración — SumadorX de Pokémon
 * ============================================================================
 *
 * Flujo probado:
 *
 *     Expresión
 *         ↓
 *     PokeParser
 *         ↓
 *     Pokedex
 *         ↓
 *     OperandConsumer
 *         ↓
 *     PokeArithmetic
 *         ↓
 *     PokemonNumber
 *
 * Estos tests verifican la interacción entre los módulos principales.
 * Los tests unitarios de cada módulo se mantienen separados.
 * ============================================================================
 */

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "../../SumarXAux/PokeParser/PokeParser.h"
#include "../../SumarXAux/PokeArithmetic/PokeArithmetic.h"
#include "../../SumarXAux/Pokedex/Pokedex.h"


/* ========================================================================== */
/* Consumer                                                                   */
/* ========================================================================== */

/*
 * Consumer utilizado por el parser.
 *
 * Cada Pokémon convertido a su valor numérico es enviado directamente
 * al acumulador del módulo PokeArithmetic.
 */
static bool add_operand(int16_t value, void *context){
    PokemonNumber *result = context;

    return add_digit_to_number(result, value);
}


/* ========================================================================== */
/* Helper                                                                     */
/* ========================================================================== */

/*
 * Procesa una expresión completa utilizando el parser y el sumador real.
 */
static ParseResult calculate_expression(const char *expression, PokemonNumber *result){
    FILE *stream = tmpfile();

    assert(stream != NULL);

    fputs(expression, stream);
    rewind(stream);

    ParseResult parse_result = parse_expression(
        stream,
        add_operand,
        result
    );

    fclose(stream);

    return parse_result;
}


/* ========================================================================= */
/* Casos válidos                                                              */
/* ========================================================================= */

/*
 * Pikachu = 25
 *
 * Verifica el caso básico:
 *
 *     Pikachu → 25
 */
static void test_single_pokemon(void){
    PokemonNumber *result = create_number();

    assert(result != NULL);

    ParseResult parse_result =
        calculate_expression("Pikachu\n", result);

    assert(parse_result.status == PARSE_OK);

    assert(result->size == 1);
    assert(result->digits[0] == 25);

    free_number(result);
}


/*
 * Pikachu + Charmander
 *
 *     25 + 4 = 29
 *
 * El resultado entra en un único dígito de base 151.
 */
static void test_two_pokemon(void){
    PokemonNumber *result = create_number();

    assert(result != NULL);

    ParseResult parse_result =
        calculate_expression(
            "Pikachu + Charmander\n",
            result
        );

    assert(parse_result.status == PARSE_OK);

    assert(result->size == 1);
    assert(result->digits[0] == 29);

    free_number(result);
}


/*
 * Bulbasaur + Charmander + Pikachu
 *
 *     1 + 4 + 25 = 30
 */
static void test_multiple_pokemon(void){
    PokemonNumber *result = create_number();

    assert(result != NULL);

    ParseResult parse_result =
        calculate_expression(
            "Bulbasaur + Charmander + Pikachu\n",
            result
        );

    assert(parse_result.status == PARSE_OK);

    assert(result->size == 1);
    assert(result->digits[0] == 30);

    free_number(result);
}


/*
 * Prueba de overflow de un dígito.
 *
 *     Mewtwo = 150
 *     Pikachu = 25
 *
 *     150 + 25 = 175
 *
 * En base 151:
 *
 *     175 = 1 * 151 + 24
 *
 * Por lo tanto:
 *
 *     digits = [24, 1]
 */
static void test_base_151_carry(void){
    PokemonNumber *result = create_number();

    assert(result != NULL);

    ParseResult parse_result =
        calculate_expression(
            "Mewtwo + Pikachu\n",
            result
        );

    assert(parse_result.status == PARSE_OK);

    assert(result->size == 2);

    /* Least Significant Digit */
    assert(result->digits[0] == 24);

    /* Carry */
    assert(result->digits[1] == 1);

    free_number(result);
}


/*
 * Mew representa el valor 0.
 *
 * La suma de Mew no debe modificar el resultado.
 */
static void test_mew_as_zero(void){
    PokemonNumber *result = create_number();

    assert(result != NULL);

    ParseResult parse_result =
        calculate_expression(
            "Mew + Pikachu\n",
            result
        );

    assert(parse_result.status == PARSE_OK);

    assert(result->size == 1);
    assert(result->digits[0] == 25);

    free_number(result);
}


/* ========================================================================= */
/* Casos inválidos                                                            */
/* ========================================================================= */

/*
 * Un '+' al final de la expresión debe ser rechazado.
 */
static void test_trailing_plus(void){
    PokemonNumber *result = create_number();

    assert(result != NULL);

    ParseResult parse_result =
        calculate_expression(
            "Pikachu +\n",
            result
        );

    assert(parse_result.status == PARSE_TRAILING_PLUS);

    free_number(result);
}


/*
 * Un Pokémon inexistente debe producir un error.
 */
static void test_invalid_pokemon(void){
    PokemonNumber *result = create_number();

    assert(result != NULL);

    ParseResult parse_result =
        calculate_expression(
            "Pikachu + Lucario\n",
            result
        );

    assert(parse_result.status == PARSE_INVALID_POKEMON);

    free_number(result);
}


/*
 * Dos operadores consecutivos representan un operando vacío.
 */
static void test_consecutive_plus(void){
    PokemonNumber *result = create_number();

    assert(result != NULL);

    ParseResult parse_result =
        calculate_expression(
            "Pikachu ++ Charmander\n",
            result
        );

    assert(parse_result.status == PARSE_EMPTY_OPERAND);

    free_number(result);
}


/* ========================================================================= */
/* Main                                                                      */
/* ========================================================================= */

int main(void){
    /* Casos válidos */
    test_single_pokemon();
    test_two_pokemon();
    test_multiple_pokemon();
    test_base_151_carry();
    test_mew_as_zero();

    /* Casos inválidos */
    test_trailing_plus();
    test_invalid_pokemon();
    test_consecutive_plus();

    puts("Todos los poketests de integración pasaron correctamente !");

    return 0;
}
