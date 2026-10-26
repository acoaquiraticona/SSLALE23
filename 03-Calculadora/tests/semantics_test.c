/**
 * @file semantics_test.c
 * @brief Pruebas unitarias de valores, operaciones y almacenamiento de variables.
 *
 * La capa semántica se invoca sin parser para comprobar sus contratos de forma
 * directa: dominio numérico, conversión a texto Pokémon, códigos de error y
 * capacidad y ciclo de vida de la tabla de símbolos.
 */
#include "Semantics.h"
#include "test_support.h"

/**
 * @brief Comprueba el contrato de una operación rechazada por rango.
 * @param r Resultado fallido que debe incluir E_RANGE, gravedad recuperable
 * y una explicación no vacía.
 */
static void Range(ResultadoSem r) {
    assert(!r.ok);
    assert(r.diagnostico.codigo == E_RANGE);
    assert(r.diagnostico.gravedad == ERROR_RECOVERABLE);
    assert(r.diagnostico.detalle[0]);
}

/**
 * @brief Verifica la semántica aritmética y la tabla de símbolos con assert.
 * @return Cero si todas las comprobaciones se cumplen.
 */
int main(void) {
    /** Los naturales aceptan cero y el máximo del dominio; la marca de rango
     * del scanner también debe rechazarse aunque el valor parcial sea pequeño. */
    SemReset();
    assert(SemNatural(0, false).ok);
    assert(SemNatural(999999, false).valor == 999999);
    Range(SemNatural(1, true));
    Range(SemNatural(1000000, false));
    Range(SemNatural(-1, false));
    /** Cada operación conserva los resultados válidos y rechaza los que salen
     * del intervalo. Negación y duplicación incluyen negativos, cero y extremos. */
    assert(SemSumar(2, 3).valor == 5);
    assert(SemSumar(-999999, 999999).valor == 0);
    Range(SemSumar(999999, 1));
    Range(SemSumar(-999999, -1));
    assert(SemRestar(9, 3).valor == 6);
    Range(SemRestar(-999999, 1));
    Range(SemRestar(999999, -1));
    assert(SemMultiplicar(-2, 3).valor == -6);
    assert(SemMultiplicar(0, 999999).valor == 0);
    Range(SemMultiplicar(999999, 999999));
    assert(SemNegar(999999).valor == -999999);
    assert(SemNegar(-999999).valor == 999999);
    assert(SemNegar(0).valor == 0);
    assert(SemDuplicar(-2).valor == -4);
    assert(SemDuplicar(499999).valor == 999998);
    Range(SemDuplicar(500000));
    Range(SemDuplicar(-500000));
    /** La conversión inversa produce nombres canónicos, orden posicional y
     * prefijo minun para negativos. Los destinos de capacidad mínima o nula
     * permiten comprobar que la escritura respeta el espacio proporcionado. */
    static const char *const nombres[] = {
        "mew", "bulbasaur", "ivysaur", "venusaur", "charmander",
        "charmeleon", "charizard", "squirtle", "wartortle", "blastoise"
    };
    for (int i = 0; i < 10; ++i) {
        char resultado[128];
        char esperado[128];
        SemAPokemon(i, resultado, sizeof resultado);
        snprintf(esperado, sizeof esperado, "[%s]", nombres[i]);
        TestEqual(resultado, esperado);
    }
    char out[128];
    SemAPokemon(-109, out, sizeof out);
    TestEqual(out, "minun [bulbasaur mew blastoise]");
    SemAPokemon(999999, out, sizeof out);
    TestEqual(out, "[blastoise blastoise blastoise blastoise blastoise blastoise]");
    char tiny[1] = {'x'};
    SemAPokemon(2, tiny, sizeof tiny);
    assert(tiny[0] == '\0');
    SemAPokemon(2, NULL, 0);

    /** La consulta de un nombre inexistente identifica la variable en el error.
     * Llenar los 32 lugares separa reasignación, que reutiliza un lugar existente,
     * de alta nueva, que debe fallar sin crear una entrada parcial. */
    ResultadoSem missing = SemVariable("energia");
    assert(!missing.ok && missing.diagnostico.codigo == E_UNDEFINED);
    assert(strstr(missing.diagnostico.detalle, "energia") != NULL);
    for (int i = 0; i < 32; ++i) {
        char name[32];
        snprintf(name, sizeof name, "v%d", i);
        assert(SemAsignar(name, i).ok);
    }
    assert(SemAsignar("v0", 99).ok);
    assert(SemVariable("v0").valor == 99);
    assert(SemAsignar("v32", 32).diagnostico.codigo == E_LIMIT);
    assert(!SemVariable("v32").ok);
    /** Liberar invalida el nombre y habilita un lugar para una nueva variable;
     * una segunda liberación del mismo nombre informa que ya no existe. */
    assert(SemLiberar("v1").ok);
    assert(!SemVariable("v1").ok);
    assert(SemLiberar("v1").diagnostico.codigo == E_UNDEFINED);
    assert(SemAsignar("v32", 32).ok);
    assert(SemVariable("v32").valor == 32);
    /** El reinicio vacía toda la tabla, y la comparación de identificadores
     * mantiene entradas distintas para nombres que solo difieren en mayúsculas. */
    SemReset();
    assert(!SemVariable("v0").ok);
    assert(SemAsignar("energia", 2).ok);
    assert(SemAsignar("Energia", 3).ok);
    assert(SemVariable("energia").valor == 2);
    assert(SemVariable("Energia").valor == 3);
    puts("Semantics: assert OK");
    return 0;
}
