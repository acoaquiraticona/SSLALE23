#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "SumarXAux/PokeParser/PokeParser.h"
#include "SumarXAux/PokeArithmetic/PokeArithmetic.h"


/**
 * @brief Callback utilizado por PokeParser.
 *
 * Cada vez que el parser encuentra un Pokémon válido, entrega su valor
 * directamente al acumulador.
 */
static bool accumulate_operand(int16_t value, void *context){
    
    PokemonNumber *result = context;

    return add_digit_to_number(result, value);
}


int main(void){
    PokemonNumber *result = create_number();

    if (!result) {
        fprintf(stderr, "ERROR: memoria insuficiente\n");
        return 1;
    }

    while (true) {
        /*
         * El parser consume stdin directamente.
         *
         * Cada operando encontrado se entrega inmediatamente a
         * accumulate_operand().
         *
         * El acumulador se conserva entre expresiones, por lo que cada nueva
         * expresión suma sobre el resultado previo.
         */
        ParseResult parsed = parse_expression(stdin, accumulate_operand, result);

        /*
         * EOF sin una nueva expresión:
         * terminamos normalmente.
         */
        if (parsed.status == PARSE_EOF) {
            break;
        }

        /*
         * Error durante el parsing o procesamiento.
         */
        if (parsed.status != PARSE_OK) {
            fprintf(stderr, "ERROR: %s\n", parsed.error_message);
            continue;
        }

        /*
         * Eliminamos ceros no significativos.
         */
        normalize_number(result);

        /*
         * Mostramos el resultado acumulado actual.
         */
        print_number(result);
    }

    free_number(result);

    return 0;
}