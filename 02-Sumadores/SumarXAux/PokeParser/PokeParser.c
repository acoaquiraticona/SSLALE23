#include "PokeParser.h"
#include "../Pokedex/Pokedex.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* ========================================================================== */
/* Funciones auxiliares privadas                                             */
/* ========================================================================== */

static ParseResult make_result(ParseStatus status, const char *message)
{
    ParseResult result;

    result.status = status;

    if (message) {
        snprintf(
            result.error_message,
            sizeof(result.error_message),
            "%s",
            message
        );
    } else {
        result.error_message[0] = '\0';
    }

    return result;
}


/**
 * @brief Descarta el resto de la expresión actual.
 *
 * Se utiliza después de detectar un error para que el próximo llamado al
 * parser comience correctamente en la siguiente línea.
 */
static void discard_until_end_of_expression(FILE *input)
{
    int c;

    while ((c = fgetc(input)) != EOF) {
        if (c == '\n') {
            break;
        }
    }
}


/**
 * @brief Construye un mensaje de error para un Pokémon inválido.
 */
static ParseResult make_pokemon_error(const char *token)
{
    char message[256];

    snprintf(
        message,
        sizeof(message),
        "Pokémon inválido '%s'",
        token
    );

    return make_result(PARSE_INVALID_POKEMON, message);
}


/**
 * @brief Agrega un carácter al token.
 */
static bool append_token_char(char *token, size_t *length, int c){
    if (*length >= POKEMON_TOKEN_MAX) return false;

    token[*length] = (char)c;
    (*length)++;

    token[*length] = '\0';

    return true;
}


/**
 * @brief Elimina espacios al final del token.
 */
static void trim_token(char *token, size_t *length){
    while (*length > 0 && isspace((unsigned char)token[*length - 1])) (*length)--;
    
    token[*length] = '\0';
}


/* ========================================================================== */
/* Parser                                                                     */
/* ========================================================================== */

ParseResult parse_expression(FILE *input, OperandConsumer consumer, void *context){
    if (!input || !consumer) return make_result(PARSE_SYNTAX_ERROR,"parámetros inválidos del parser");
    
    char token[POKEMON_TOKEN_MAX + 1];

    size_t token_length = 0;

    /*
     * true  -> esperamos un Pokémon
     * false -> esperamos '+', espacios o fin de expresión
     */
    bool expect_operand = true;

    /*
     * Indica si encontramos algún contenido antes de EOF/newline.
     */
    bool saw_anything = false;

    /*
     * Si Arithmetic falla al procesar un operando, continuamos leyendo la
     * expresión para dejar stdin correctamente posicionado.
     */
    bool callback_failed = false;

    while (true) {

        int c = fgetc(input);

        /* ------------------------------------------------------------------ */
        /* EOF                                                                 */
        /* ------------------------------------------------------------------ */

        if (c == EOF) {

            if (ferror(input)) return make_result(PARSE_IO_ERROR,"error de lectura en stdin");

            /*
             * EOF sin haber encontrado contenido:
             * no existe otra expresión.
             */
            if (!saw_anything) return make_result(PARSE_EOF, NULL);

            /*
             * Si terminamos después de '+', falta un operando.
             */
            if (expect_operand) return make_result(PARSE_TRAILING_PLUS, "expresión termina con '+' sin operando");
  
            if (callback_failed) return make_result(PARSE_CALLBACK_ERROR, "no se pudo procesar un operando");

            return make_result(PARSE_OK, NULL);
        }


        /* ------------------------------------------------------------------ */
        /* FIN DE EXPRESIÓN                                                   */
        /* ------------------------------------------------------------------ */

        if (c == '\n') {

            if (!saw_anything) return make_result(PARSE_EMPTY_LINE, "línea vacía");

            if (expect_operand) return make_result(PARSE_TRAILING_PLUS, "expresión termina con '+' sin operando");

            if (callback_failed) return make_result(PARSE_CALLBACK_ERROR, "no se pudo procesar un operando");

            return make_result(PARSE_OK, NULL);
        }


        /* ================================================================== */
        /* ESPERAMOS UN OPERANDO                                              */
        /* ================================================================== */

        if (expect_operand) {

            /*
             * Ignoramos espacios antes del Pokémon.
             */
            if (isspace((unsigned char)c)) {
                saw_anything = true;
                continue;
            }


            /*
             * '+' cuando esperábamos un Pokémon significa:
             *
             *     Pikachu + + Mew
             *              ^
             *
             * o:
             *
             *     + Pikachu
             */
            if (c == '+') {

                discard_until_end_of_expression(input);

                return make_result(PARSE_EMPTY_OPERAND, "operando vacío");
            }


            /*
             * Comienza un nuevo token.
             */
            saw_anything = true;

            token_length = 0;
            token[0] = '\0';


            if (!append_token_char(token, &token_length, c)) {
                discard_until_end_of_expression(input);
                return make_result(PARSE_TOKEN_TOO_LONG, "nombre de Pokémon demasiado largo");
            }


            /*
             * Leemos el resto del Pokémon hasta:
             *
             *     '+'
             *     '\n'
             *     EOF
             *
             * Esto permite nombres como:
             *
             *     Mr. Mime
             */
            while (true) {

                c = fgetc(input);

                if (c == EOF) {

                    if (ferror(input)) return make_result(PARSE_IO_ERROR, "error de lectura en stdin");
                    
                    break;
                }


                if (c == '+' || c == '\n') break;



                if (!append_token_char(token, &token_length, c)) {

                    /*
                     * El token es demasiado largo.
                     *
                     * Consumimos el resto de la expresión para que el
                     * siguiente llamado comience en la línea siguiente.
                     */
                    discard_until_end_of_expression(input);

                    return make_result(PARSE_TOKEN_TOO_LONG, "nombre de Pokémon demasiado largo");
                }
            }


            /*
             * Eliminamos espacios al final.
             */
            trim_token(token, &token_length);


            if (token_length == 0) {

                if (c != '\n' && c != EOF)discard_until_end_of_expression(input);

                return make_result(PARSE_EMPTY_OPERAND, "operando vacío");
            }


            /* -------------------------------------------------------------- */
            /* Pokémon → valor                                                 */
            /* -------------------------------------------------------------- */

            int16_t value = pokemon_to_value(token);

            if (value == INVALID_POKEMON) {

                /*
                 * El delimitador '+' ya fue consumido si corresponde.
                 * Descartamos lo que reste de la expresión.
                 */
                if (c != '\n' && c != EOF) {
                    discard_until_end_of_expression(input);
                }

                return make_pokemon_error(token);
            }


            /* -------------------------------------------------------------- */
            /* Entregamos inmediatamente el operando al consumidor             */
            /* -------------------------------------------------------------- */

            if (!consumer(value, context)) {
                callback_failed = true;
            }

            /*
             * Si encontramos '+', la siguiente cosa debe ser otro Pokémon.
             */
            expect_operand = (c == '+');

            /*
             * Si encontramos newline, la expresión terminó.
             */
            if (c == '\n') {
                if (callback_failed) return make_result(PARSE_CALLBACK_ERROR, "no se pudo procesar un operando");
                return make_result(PARSE_OK, NULL);
            }


            /*
             * Si encontramos EOF, la expresión terminó correctamente.
             */
            if (c == EOF) {
                if (callback_failed) return make_result(PARSE_CALLBACK_ERROR, "no se pudo procesar un operando");
                return make_result(PARSE_OK, NULL);
            }

            continue;
        }


        /* ================================================================== */
        /* ESPERAMOS '+', ESPACIOS O FIN                                      */
        /* ================================================================== */

        if (c == '+') {
            expect_operand = true;
            continue;
        }


        /*
         * Después de un Pokémon permitimos espacios.
         *
         * Ejemplos válidos:
         *
         *     Pikachu + Mew
         *     Pikachu+Mew
         *     Pikachu   +   Mew
         */
        if (isspace((unsigned char)c)) {

            while (true) {

                c = fgetc(input);

                if (c == EOF) {
                    if (ferror(input)) return make_result(PARSE_IO_ERROR, "error de lectura en stdin");
                    if (callback_failed) return make_result(PARSE_CALLBACK_ERROR, "no se pudo procesar un operando");
                    return make_result(PARSE_OK, NULL);
                }

                if (c == '\n') {
                    if (callback_failed) return make_result(PARSE_CALLBACK_ERROR,"no se pudo procesar un operando");
                    return make_result(PARSE_OK, NULL);
                }

                if (c == '+') {
                    expect_operand = true;
                    break;
                }

                if (!isspace((unsigned char)c)) {
                    discard_until_end_of_expression(input);
                    return make_result(PARSE_SYNTAX_ERROR, "se esperaba '+' entre operandos");
                }
            }

            continue;
        }

        /*
         * Encontramos algo que no es '+', espacio ni fin.
         *
         * Ejemplo:
         *
         *     Pikachu Mew
         */
        discard_until_end_of_expression(input);

        return make_result(PARSE_SYNTAX_ERROR, "se esperaba '+' entre operandos");
    }
}