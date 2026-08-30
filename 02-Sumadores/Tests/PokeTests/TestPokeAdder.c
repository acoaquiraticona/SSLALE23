#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "../../SumarXAux/PokeArithmetic/PokeArithmetic.h"
#include "../../SumarXAux/Pokedex/Pokedex.h"

int main(void){
    PokemonNumber *number = create_number();

    assert(number != NULL);

    /*
     * 1.000.000 operandos.
     *
     * Cada uno vale 150.
     *
     * Resultado:
     *
     * 150.000.000
     *
     * Lo importante en este test no es verificar manualmente cada dígito,
     * sino comprobar que:
     *
     * 1. No existe límite artificial de operandos.
     * 2. El acumulador crece correctamente.
     * 3. Todos los dígitos permanecen dentro de [0,150].
     */
    for (size_t i = 0; i < 1000000; ++i)
        assert(add_digit_to_number(number, 150));
    
    assert(number->size > 1);
    assert(number->capacity >= number->size);

    for (size_t i = 0; i < number->size; ++i) {
        assert(number->digits[i] >= 0);
        assert(number->digits[i] < POKEDEX_SIZE);
    }

    printf("Super PokeAdder paso con más de 1,000,000 operandos !\n");

    free_number(number);

    return 0;
}