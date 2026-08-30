#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../../SumarXAux/PokeParser/PokeParser.h"


/* ========================================================================== */
/* Contexto de prueba                                                         */
/* ========================================================================== */

#define MAX_TEST_OPERANDS 32

typedef struct {
    int16_t values[MAX_TEST_OPERANDS];
    size_t count;
} TestContext;


/* ========================================================================== */
/* Callback                                                                   */
/* ========================================================================== */

static bool collect_operand(int16_t value, void *context){
    TestContext *test = context;

    assert(test != NULL);

    if (test->count >= MAX_TEST_OPERANDS) {
        return false;
    }

    test->values[test->count] = value;
    test->count++;

    return true;
}


/* ========================================================================== */
/* Helper                                                                      */
/* ========================================================================== */

static TestContext parse_string(const char *input){
    FILE *stream = tmpfile();

    assert(stream != NULL);

    fputs(input, stream);
    rewind(stream);

    TestContext context = {0};

    ParseResult result = parse_expression(stream, collect_operand, &context);

    assert(result.status == PARSE_OK);

    fclose(stream);

    return context;
}


/* ========================================================================== */
/* Un operando                                                                */
/* ========================================================================== */

static void test_single_operand(void){
    
    TestContext context = parse_string("Pikachu\n");

    assert(context.count == 1);
    assert(context.values[0] == 25);
}


/* ========================================================================== */
/* Múltiples operandos                                                        */
/* ========================================================================== */

static void test_multiple_operands(void){
    
    TestContext context = parse_string("Pikachu + Charmander + Squirtle\n");

    assert(context.count == 3);

    assert(context.values[0] == 25);
    assert(context.values[1] == 4);
    assert(context.values[2] == 7);
}


/* ========================================================================== */
/* Sin espacios                                                               */
/* ========================================================================== */

static void test_without_spaces(void){
    
    TestContext context = parse_string("Pikachu+Charmander+Squirtle\n");

    assert(context.count == 3);

    assert(context.values[0] == 25);
    assert(context.values[1] == 4);
    assert(context.values[2] == 7);
}


/* ========================================================================== */
/* Espacios arbitrarios                                                       */
/* ========================================================================== */

static void test_with_spaces(void){
    
    TestContext context = parse_string("  Pikachu   +   Charmander   +   Squirtle  \n");

    assert(context.count == 3);

    assert(context.values[0] == 25);
    assert(context.values[1] == 4);
    assert(context.values[2] == 7);
}


/* ========================================================================== */
/* Mew                                                                         */
/* ========================================================================== */

static void test_mew(void){
    
    TestContext context = parse_string("Mew\n");

    assert(context.count == 1);
    assert(context.values[0] == 0);
}


/* ========================================================================== */
/* Mewtwo                                                                      */
/* ========================================================================== */

static void test_mewtwo(void){
    
    TestContext context = parse_string("Mewtwo\n");

    assert(context.count == 1);
    assert(context.values[0] == 150);
}


/* ========================================================================== */
/* Pokémon inválido                                                           */
/* ========================================================================== */

static void test_invalid_pokemon(void){
    FILE *stream = tmpfile();

    assert(stream != NULL);

    fputs("Pikachu + EstoNoExiste + Mewtwo\n", stream);
    rewind(stream);

    TestContext context = {0};

    ParseResult result = parse_expression(stream, collect_operand, &context);

    assert(result.status == PARSE_INVALID_POKEMON);

    /*
     * Pikachu sí fue procesado antes de encontrar el error.
     */
    assert(context.count == 1);
    assert(context.values[0] == 25);

    fclose(stream);
}


/* ========================================================================== */
/* Operando vacío                                                             */
/* ========================================================================== */

static void test_empty_operand(void){
    FILE *stream = tmpfile();

    assert(stream != NULL);

    fputs("Pikachu ++ Mewtwo\n", stream);
    rewind(stream);

    TestContext context = {0};

    ParseResult result = parse_expression(stream, collect_operand, &context);

    assert(result.status == PARSE_EMPTY_OPERAND);

    fclose(stream);
}


/* ========================================================================== */
/* Plus al comienzo                                                           */
/* ========================================================================== */

static void test_leading_plus(void){
    FILE *stream = tmpfile();

    assert(stream != NULL);

    fputs("+ Pikachu\n", stream);
    rewind(stream);

    TestContext context = {0};

    ParseResult result = parse_expression(stream, collect_operand, &context);

    assert(result.status == PARSE_EMPTY_OPERAND);

    fclose(stream);
}


/* ========================================================================== */
/* Plus al final                                                              */
/* ========================================================================== */

static void test_trailing_plus(void){
    FILE *stream = tmpfile();

    assert(stream != NULL);

    fputs("Pikachu +\n", stream);
    rewind(stream);

    TestContext context = {0};

    ParseResult result = parse_expression(stream, collect_operand, &context);

    assert(result.status == PARSE_TRAILING_PLUS);

    fclose(stream);
}


/* ========================================================================== */
/* Dos Pokémon sin '+'                                                        */
/* ========================================================================== */

static void test_missing_operator(void){
    FILE *stream = tmpfile();

    assert(stream != NULL);

    fputs("Pikachu Charmander\n", stream);
    rewind(stream);

    TestContext context = {0};

    ParseResult result = parse_expression(stream, collect_operand, &context);

    assert(result.status == PARSE_INVALID_POKEMON);

    fclose(stream);
}


/* ========================================================================== */
/* Expresión vacía                                                            */
/* ========================================================================== */

static void test_empty_line(void){
    FILE *stream = tmpfile();

    assert(stream != NULL);

    fputs("\n", stream);
    rewind(stream);

    TestContext context = {0};

    ParseResult result = parse_expression(stream, collect_operand, &context);

    assert(result.status == PARSE_EMPTY_LINE);

    fclose(stream);
}


/* ========================================================================== */
/* EOF                                                                         */
/* ========================================================================== */

static void test_eof(void){
    FILE *stream = tmpfile();

    assert(stream != NULL);

    TestContext context = {0};

    ParseResult result = parse_expression(stream, collect_operand, &context);

    assert(result.status == PARSE_EOF);
    assert(context.count == 0);

    fclose(stream);
}


/* ========================================================================== */
/* Main                                                                       */
/* ========================================================================== */

int main(void){
    test_single_operand();

    test_multiple_operands();

    test_without_spaces();

    test_with_spaces();

    test_mew();

    test_mewtwo();

    test_invalid_pokemon();

    test_empty_operand();

    test_leading_plus();

    test_trailing_plus();

    test_missing_operator();

    test_empty_line();

    test_eof();

    printf("Todos los testeos del parser fueron superados éxitosamente !\n");

    return 0;
}