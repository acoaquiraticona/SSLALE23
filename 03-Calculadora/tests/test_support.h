/**
 * @file test_support.h
 * @brief Utilidades compartidas para preparar entradas y comprobar resultados.
 *
 * Las suites usan assert de C tanto para sus expectativas como para comprobar
 * que la infraestructura de prueba pudo abrir, leer y escribir los archivos.
 * Por eso deben compilarse con las aserciones habilitadas. Los archivos de
 * trabajo viven en .build y sus nombres son fijos: las suites que los comparten
 * deben ejecutarse de manera secuencial desde la carpeta de la calculadora.
 */
#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/wait.h>
#endif

/**
 * @brief Lee un archivo completo como una cadena terminada en NUL.
 * @param ruta Ruta del archivo que ya debe existir.
 * @return Memoria reservada con malloc que el llamador debe liberar con free.
 *
 * La lectura binaria conserva los bytes y los finales de línea para permitir
 * comparaciones exactas. Las aserciones detectan fallos de posicionamiento,
 * reserva de memoria, lectura y cierre antes de evaluar el contenido.
 */
static inline char *TestReadFile(const char *ruta) {
    FILE *archivo = fopen(ruta, "rb");
    assert(archivo != NULL);
    assert(fseek(archivo, 0, SEEK_END) == 0);
    long longitudArchivo = ftell(archivo);
    assert(longitudArchivo >= 0);
    rewind(archivo);

    /* ftell devuelve long; malloc y fread reciben tamaños de tipo size_t.
     * El cast explicita esa conversión después de comprobar que no hubo error.
     * Se reserva un byte adicional para el NUL que termina la cadena de C. */
    char *contenidoArchivo = malloc((size_t)longitudArchivo + 1);
    assert(contenidoArchivo != NULL);
    assert(fread(contenidoArchivo, 1, (size_t)longitudArchivo, archivo) == (size_t)longitudArchivo);
    contenidoArchivo[longitudArchivo] = '\0';
    assert(fclose(archivo) == 0);

    /* Se entrega la dirección de la memoria reservada, no una copia: quien
     * recibe el resultado debe llamar a free cuando termine de consultarlo. */
    return contenidoArchivo;
}

/**
 * @brief Exige igualdad exacta entre dos cadenas y muestra ambas si difieren.
 * @param actual Resultado obtenido, terminado en NUL.
 * @param esperado Resultado esperado, terminado en NUL.
 *
 * El diagnóstico previo a assert permite ver diferencias de formato y de
 * contenido que una comparación booleana aislada no mostraría.
 */
static inline void TestEqual(const char *actual, const char *esperado) {
    if (strcmp(actual, esperado) != 0) {
        fprintf(stderr, "Esperado:\n%s\nRecibido:\n%s\n", esperado, actual);
        assert(strcmp(actual, esperado) == 0);
    }
}

/**
 * @brief Crea o reemplaza un archivo con los bytes de una cadena de prueba.
 * @param ruta Ruta de destino dentro del área de trabajo de las pruebas.
 * @param contenido Texto terminado en NUL; su terminador no se escribe.
 *
 * Se usa modo binario para conservar CR y LF exactamente como los especifica
 * el caso. Las entradas con NUL incrustado se preparan directamente con fputc.
 */
static inline void TestWriteFile(const char *ruta, const char *contenido) {
    FILE *archivo = fopen(ruta, "wb");
    assert(archivo != NULL);
    size_t longitudContenido = strlen(contenido);
    assert(fwrite(contenido, 1, longitudContenido, archivo) == longitudContenido);
    assert(fclose(archivo) == 0);
}

/**
 * @brief Resultado capturado de una ejecución del programa real.
 *
 * @details estado es el código de salida normalizado del proceso; out contiene
 * stdout, con los resultados de las expresiones; err contiene stderr, con los
 * diagnósticos, registros INFO y trazas. Las dos cadenas son reservas dinámicas
 * independientes cuya propiedad se entrega al llamador y que libera TestFree.
 */
typedef struct {
    int estado; /**< Código de terminación del proceso. */
    char *out;  /**< Cadena reservada con el contenido capturado de stdout. */
    char *err;  /**< Cadena reservada con el contenido capturado de stderr. */
} TestRunResult;

/**
 * @brief Obtiene el código de salida a partir del resultado de system.
 * @param estadoDelSistema Estado devuelto por system, que debe indicar terminación normal.
 * @return Código del programa hijo, con el mismo significado en Windows y POSIX.
 *
 * POSIX codifica información adicional en el estado y requiere WEXITSTATUS;
 * Windows entrega directamente el código. Una señal o el fallo al crear el
 * proceso hacen fallar la prueba, en lugar de confundirse con un error de Calc.
 */
static inline int TestExitCode(int estadoDelSistema) {
    assert(estadoDelSistema != -1);
#ifdef _WIN32
    return estadoDelSistema;
#else
    assert(WIFEXITED(estadoDelSistema));
    return WEXITSTATUS(estadoDelSistema);
#endif
}

/**
 * @brief Ejecuta pokecalc con entrada preparada y captura sus dos flujos.
 * @param entrada Programa del lenguaje Pokémon que se suministra por stdin.
 * @param argumentos Argumentos controlados por la suite, insertados en el comando.
 * @return Estado del proceso y cadenas de salida que deben pasarse a TestFree.
 *
 * Primero materializa la entrada en .build/entrada.txt. Luego la redirección
 * del intérprete de comandos separa resultados y registros en otros dos
 * archivos, para comprobar por separado valor, formato, errores y trazas. La
 * ruta del ejecutable se adapta a cmd.exe en Windows y al shell en POSIX.
 * @pre pokecalc y el directorio .build deben existir en el directorio actual.
 */
static inline TestRunResult TestRun(const char *entrada, const char *argumentos) {
#ifdef _WIN32
    const char *rutaEjecutable = ".\\pokecalc.exe";
#else
    const char *rutaEjecutable = "./pokecalc";
#endif
    TestWriteFile(".build/entrada.txt", entrada);
    char comando[256];
    int caracteresNecesarios = snprintf(comando, sizeof comando,
                     "%s %s < .build/entrada.txt > .build/salida.txt 2> .build/registros.txt",
                     rutaEjecutable, argumentos);
    /* snprintf devuelve int, mientras sizeof devuelve size_t. El cast permite
     * comparar con la capacidad fija del arreglo usando el mismo tipo. */
    assert(caracteresNecesarios > 0 && caracteresNecesarios < (int)sizeof comando);
    TestRunResult resultado;
    resultado.estado = TestExitCode(system(comando));
    resultado.out = TestReadFile(".build/salida.txt");
    resultado.err = TestReadFile(".build/registros.txt");
    return resultado;
}

/**
 * @brief Libera las dos cadenas capturadas por TestRun.
 * @param resultado Resultado cuyos campos out y err dejan de ser utilizables.
 */
static inline void TestFree(TestRunResult resultado) {
    /* La estructura llega por valor, pero sus punteros siguen señalando las
     * mismas reservas. Liberarlas invalida también los punteros del llamador. */
    free(resultado.out);
    free(resultado.err);
}
#endif
