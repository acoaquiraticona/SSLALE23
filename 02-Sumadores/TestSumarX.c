#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#define main unused_main
#include "SumarX.c"
#undef main

static bool test_expression(const char *expression, PokemonNumber *total) {
    FILE *stream = tmpfile();
    assert(stream != NULL);

    fputs(expression, stream);
    rewind(stream);

    number_init(total);
    int value;
    bool success = false;

    while (GetNextX(stream, &value)) {
        number_add(total, value);
        success = true;
    }

    fclose(stream);
    return success;
}

int main(void) {
    // Test para un solo operando
    {
        PokemonNumber total;
        bool ok = test_expression("Pikachu", &total);
        assert(ok);
        assert(total.size >= 1);
    }

    // Test para múltiples operandos
    {
        PokemonNumber total;
        bool ok = test_expression("Pikachu Bulbasaur Charmander", &total);
        assert(ok);
    }

    // Test para entrada inválida
    {
        PokemonNumber total;
        bool ok = test_expression("InvalidPokemonName", &total);
        assert(!ok);
    }

    printf("Todos los tests de SumarX pasaron exitosamente!\n");
    return 0;
}