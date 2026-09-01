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

    while (true) {

        /*
        * DECISIÓN DE DISEÑO:
        * Cada expresión debe procesarse de forma independiente, por lo que el
        * acumulador se crea dentro del ciclo y no se reutiliza entre expresiones.
        *
        * Esto garantiza el aislamiento de estado: el resultado de una expresión
        * no afecta a la siguiente y, ante un error de parsing o procesamiento,
        * cualquier estado parcial se descarta junto con el acumulador.
        *
        * Por este motivo, result debe liberarse en todos los caminos de ejecución
        * antes de continuar con la siguiente expresión.
        */
        PokemonNumber *result = create_number();

        if (!result) {
            fprintf(stderr,"ERROR: memoria insuficiente\n");
            return 1;
        }

        /*
         * El parser consume stdin directamente.
         *
         * Cada operando encontrado se entrega inmediatamente a
         * accumulate_operand().
         */
        ParseResult parsed = parse_expression(stdin, accumulate_operand, result);

        /*
        * El acumulador pertenece exclusivamente a la expresión actual.
        * Aunque no haya resultado que procesar, debe liberarse antes de finalizar.
        */
        if (parsed.status == PARSE_EOF) {
            free_number(result);
            break;
        }

        /*
        * El acumulador se descarta ante cualquier error para evitar que un estado
        * parcial de la expresión actual contamine el procesamiento de la siguiente.
        */
        if (parsed.status != PARSE_OK) {
            fprintf(stderr, "ERROR: %s\n", parsed.error_message);
            free_number(result);

            continue;
        }


        /*
         * Eliminamos ceros no significativos.
         */
        normalize_number(result);


        /*
         * Mostramos el resultado.
         */
        print_number(result);


        /*
         * Liberamos el acumulador antes de procesar la siguiente expresión.
         */
        free_number(result);
    }


    return 0;
}