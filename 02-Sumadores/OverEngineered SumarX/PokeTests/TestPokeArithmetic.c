#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "../../SumarXAux/PokeArithmetic/PokeArithmetic.h"
#include "../../SumarXAux/Pokedex/Pokedex.h"


/* ========================================================================== */
/* Helpers                                                                    */
/* ========================================================================== */

static void assert_number(const PokemonNumber *number, const int16_t *expected, size_t expected_size){
    assert(number != NULL);
    assert(number->digits != NULL);

    if (number->size != expected_size) {
        fprintf(
            stderr,
            "\nSize mismatch!\n"
            "Expected: %zu\n"
            "Actual:   %zu\n",
            expected_size,
            number->size
        );

        fprintf(stderr, "Actual digits: ");

        for (size_t i = 0; i < number->size; ++i) 
            fprintf(stderr, "%d ", number->digits[i]);
        
        fprintf(stderr, "\nExpected digits: ");

        for (size_t i = 0; i < expected_size; ++i)
            fprintf(stderr, "%d ", expected[i]);
        

        fprintf(stderr, "\n\n");
    }

    assert(number->size == expected_size);

    for (size_t i = 0; i < expected_size; ++i)
        assert(number->digits[i] == expected[i]);
    
}


/* ========================================================================== */
/* Creación                                                                   */
/* ========================================================================== */

static void test_create_number(void){
    PokemonNumber *number = create_number();

    assert(number != NULL);
    assert(number->digits != NULL);
    assert(number->size == 1);
    assert(number->digits[0] == 0);

    free_number(number);
}


/* ========================================================================== */
/* Mew = 0                                                                    */
/* ========================================================================== */

static void test_add_mew(void){
    PokemonNumber *number = create_number();

    assert(number != NULL);

    assert(add_digit_to_number(number, 0));

    const int16_t expected[] = {0};

    assert_number(number, expected, 1);

    free_number(number);
}


/* ========================================================================== */
/* Suma simple                                                                */
/* ========================================================================== */

static void test_simple_addition(void){
    PokemonNumber *number = create_number();

    assert(number != NULL);

    assert(add_digit_to_number(number, 25));    // Pikachu = 25

    const int16_t expected[] = {25};

    assert_number(number, expected, 1);

    free_number(number);
}


/* ========================================================================== */
/* Múltiples operandos                                                        */
/* ========================================================================== */

static void test_multiple_operands(void){
    PokemonNumber *number = create_number();

    assert(number != NULL);

    assert(add_digit_to_number(number, 25)); /* Pikachu */
    assert(add_digit_to_number(number, 4));  /* Charmander */
    assert(add_digit_to_number(number, 7));  /* Squirtle */

    const int16_t expected[] = {36};    // 25 + 4 + 7 = 36

    assert_number(number, expected, 1);

    free_number(number);
}


/* ========================================================================== */
/* Primer overflow                                                            */
/* ========================================================================== */

static void test_first_carry(void){
    PokemonNumber *number = create_number();

    assert(number != NULL);

    /*
     * Mewtwo = 150
     *
     * 150 + 150 = 300
     *
     * 300 = 1 * 151 + 149
     *
     * Resultado:
     *
     * [149, 1]
     */
    assert(add_digit_to_number(number, 150));
    assert(add_digit_to_number(number, 150));

    const int16_t expected[] = {149, 1};

    assert_number(number, expected, 2);

    free_number(number);
}


/* ========================================================================== */
/* Carry múltiple                                                              */
/* ========================================================================== */

static void test_multiple_carries(void){
    PokemonNumber *number = create_number();

    assert(number != NULL);

    /*
     * 150 * 3 = 450
     *
     * 450 = 2 * 151 + 148
     *
     * Resultado:
     *
     * [148, 2]
     */
    assert(add_digit_to_number(number, 150));
    assert(add_digit_to_number(number, 150));
    assert(add_digit_to_number(number, 150));

    const int16_t expected[] = {148, 2};

    assert_number(number, expected, 2);

    free_number(number);
}


/* ========================================================================== */
/* Carry hacia una tercera posición                                           */
/* ========================================================================== */

static void test_carry_into_third_digit(void){
    PokemonNumber *number = create_number();

    assert(number != NULL);

    /*
     * 153 * 150 = 22950
     *
     * 151² = 22801
     *
     * 22950 = 1 * 151² + 0 * 151 + 149
     *
     * Resultado:
     *
     * [149, 0, 1]
     */
    for (int i = 0; i < 153; ++i)
        assert(add_digit_to_number(number, 150));

    const int16_t expected[] = {149, 0, 1};

    assert_number(number, expected, 3);

    free_number(number);
}


/* ========================================================================== */
/* Operando mínimo                                                             */
/* ========================================================================== */

static void test_zero_after_large_number(void){
    PokemonNumber *number = create_number();

    assert(number != NULL);

    assert(add_digit_to_number(number, 150));
    assert(add_digit_to_number(number, 150));
    assert(add_digit_to_number(number, 0));

    const int16_t expected[] = {149, 1};

    assert_number(number, expected, 2);

    free_number(number);
}


/* ========================================================================== */
/* Normalización                                                               */
/* ========================================================================== */

static void test_normalization(void){
    PokemonNumber *number = create_number();

    assert(number != NULL);

    /*
     * Creamos:
     *
     * [149, 1]
     */
    assert(add_digit_to_number(number, 150));
    assert(add_digit_to_number(number, 150));

    /*
     * Forzamos una posición superior en cero.
     */
    assert(number->size == 2);

    assert(number->digits[1] == 1);

    normalize_number(number);

    assert(number->size == 2);

    free_number(number);
}


/* ========================================================================== */
/* Operando inválido                                                           */
/* ========================================================================== */

static void test_invalid_digit(void){
    PokemonNumber *number = create_number();

    assert(number != NULL);

    assert(!add_digit_to_number(number, -1));
    assert(!add_digit_to_number(number, 151));
    assert(!add_digit_to_number(number, 200));

    /*
     * El número no debe haber sido modificado.
     */
    assert(number->size == 1);
    assert(number->digits[0] == 0);

    free_number(number);
}


/* ========================================================================== */
/* Capacidad dinámica                                                          */
/* ========================================================================== */

static void test_dynamic_capacity(void){
    PokemonNumber *number = create_number();

    assert(number != NULL);

    /*
     * Forzamos el crecimiento del Big Number.
     *
     * Se necesitan varios dígitos para representar una suma suficientemente
     * grande en base 151.
     */
    for (int i = 0; i < 10000; ++i)
        assert(add_digit_to_number(number, 150));
    
    assert(number->size > 1);
    assert(number->capacity >= number->size);

    /*
     * Todos los dígitos deben ser válidos en base 151.
     */
    for (size_t i = 0; i < number->size; ++i) {
        assert(number->digits[i] >= 0);
        assert(number->digits[i] < POKEDEX_SIZE);
    }

    free_number(number);
}


/* ========================================================================== */
/* Main                                                                       */
/* ========================================================================== */

int main(void){
    test_create_number();

    test_add_mew();

    test_simple_addition();

    test_multiple_operands();

    test_first_carry();

    test_multiple_carries();

    test_carry_into_third_digit();

    test_zero_after_large_number();

    test_normalization();

    test_invalid_digit();

    test_dynamic_capacity();

    printf("Todos los tests aritmeticos fueron superados exitosamente !\n");

    return 0;
}