/**
 * @file acceptance_test.c
 * @brief Pruebas de aceptación del ejecutable completo y sus flujos externos.
 *
 * A diferencia de las suites unitarias, aquí se lanza pokecalc como proceso y
 * se observan stdout, stderr y su estado de terminación. Se verifica el corpus
 * de Calc.md y escenarios de capacidad, recuperación, transacciones, argumentos,
 * trazas y límites de recursos. Las pruebas POSIX adicionales controlan fallos
 * de entrada/salida y la entrega de resultados antes de recibir EOF.
 */
#define _POSIX_C_SOURCE 200809L
#include "test_support.h"
#include "casos.h"
#ifndef _WIN32
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#endif

/**
 * @brief Diagnóstico recuperable o fatal esperado en una posición de la fuente.
 *
 * @details codigo conserva el nombre público E_xxx; linea y columna identifican
 * la ubicación esperada, comenzando en uno. Logs consume estos elementos en
 * el mismo orden en que deben aparecer los registros ERROR.
 */
typedef struct {
    const char *codigo;
    int linea;
    int columna;
} ExpectedError;

/**
 * @brief Valida la estructura de los registros y los errores esperados en stderr.
 * @param log Cadena completa de registros terminados por salto de línea.
 * @param errores Secuencia de códigos y posiciones esperados; puede ser NULL
 * si cantidad vale cero.
 * @param cantidad Cantidad exacta de registros ERROR que deben aparecer.
 * @param infosEsperados Cantidad exacta de sentencias confirmadas mediante INFO.
 * @param trace Permite registros TRACE además de ERROR e INFO.
 *
 * Primero exige bytes ASCII imprimibles o saltos de línea. Luego separa líneas
 * y comprueba campos según su clase: código, posición y mensaje no vacío para
 * ERROR; acción y sus atributos correspondientes para INFO. Para TRACE se
 * verifica que esté habilitada; los escenarios específicos comparan su contenido exacto.
 */
static void Logs(const char *log, const ExpectedError *errores, size_t cantidad,
                 int infosEsperados, bool trace) {
    size_t indiceError = 0;
    int cantidadInfos = 0;
    for (const unsigned char *byte = (const unsigned char *)log; *byte != '\0'; byte++) {
        assert(*byte == '\n' || (*byte >= 32 && *byte <= 126));
    }
    while (log[0] != '\0') {
        const char *finLinea = strchr(log, '\n');
        assert(finLinea != NULL);
        /* Restar dos direcciones del mismo texto da la distancia en bytes. */
        size_t longitudLinea = (size_t)(finLinea - log);
        char linea[4096];
        assert(longitudLinea > 0 && longitudLinea < sizeof linea);
        memcpy(linea, log, longitudLinea);
        linea[longitudLinea] = '\0';
        if (strncmp(linea, "ERROR ", 6) == 0) {
            assert(indiceError < cantidad);
            int numeroLinea;
            int columna;
            char codigo[32];
            assert(sscanf(linea, "ERROR linea=%d columna=%d codigo=%31s", &numeroLinea, &columna, codigo) == 3);
            TestEqual(codigo, errores[indiceError].codigo);
            if (numeroLinea != errores[indiceError].linea || columna != errores[indiceError].columna) {
                fprintf(stderr, "Ubicacion esperada %d:%d; recibida %d:%d\n",
                        errores[indiceError].linea, errores[indiceError].columna,
                        numeroLinea, columna);
            }
            assert(numeroLinea == errores[indiceError].linea && columna == errores[indiceError].columna);
            const char *causa = strstr(linea, " mensaje=\"");
            assert(causa != NULL && causa[10] != '"' && causa[10] != 0);
            assert(linea[longitudLinea - 1] == '"');
            indiceError++;
        } else if (strncmp(linea, "INFO ", 5) == 0) {
            int numeroLinea;
            int columna;
            char accion[32];
            assert(sscanf(linea, "INFO linea=%d columna=%d accion=%31s", &numeroLinea, &columna, accion) == 3);
            assert(numeroLinea > 0 && columna > 0);
            assert(strcmp(accion, "asignacion") == 0 || strcmp(accion, "liberacion") == 0 || strcmp(accion, "evaluacion") == 0);
            if (strcmp(accion, "evaluacion") != 0) {
                assert(strstr(linea, " variable=") != NULL);
            }
            if (strcmp(accion, "liberacion") != 0) {
                assert(strstr(linea, " valor=") != NULL);
            }
            cantidadInfos++;
        } else {
            assert(trace && strncmp(linea, "TRACE ", 6) == 0);
        }
        log = finLinea + 1;
    }
    assert(indiceError == cantidad && cantidadInfos == infosEsperados);
}

/**
 * @brief Ejecuta una sesión sin traza y verifica todas sus salidas observables.
 * @param input Programa suministrado al ejecutable por stdin.
 * @param output Cadena exacta esperada en stdout.
 * @param status Estado de terminación esperado.
 * @param errors Errores esperados en orden de aparición; NULL si n es cero.
 * @param n Cantidad de elementos de errors.
 * @param infos Cantidad de registros INFO esperados.
 *
 * Centraliza captura, comparación y liberación para que los escenarios describan
 * el comportamiento del lenguaje sin repetir la infraestructura de procesos.
 */
static void Check(const char *input, const char *output, int status,
                  const ExpectedError *errors, size_t n, int infos) {
    TestRunResult r = TestRun(input, "");
    TestEqual(r.out, output);
    assert(r.estado == status);
    Logs(r.err, errors, n, infos, false);
    TestFree(r);
}

/**
 * @brief Recorre los 76 casos de Calc.md y exige sus expectativas independientes.
 *
 * Cada fila ejecuta un proceso nuevo. Si falla estado o salida, se imprime el
 * identificador del caso antes del assert para localizarlo en la especificación.
 * La cadena de código vacía expresa que no debe haber ningún diagnóstico ERROR.
 */
static void Tabla(void) {
    size_t cantidad = sizeof casos / sizeof casos[0];
    assert(cantidad == 76);
    for (size_t i = 0; i < cantidad; ++i) {
        const AcceptanceCase *caso = &casos[i];
        TestRunResult resultado = TestRun(caso->entrada, "");
        if (resultado.estado != caso->estado || strcmp(resultado.out, caso->salida) != 0) {
            fprintf(stderr, "Fallo en %s\n%s", caso->id, resultado.err);
        }
        assert(resultado.estado == caso->estado);
        TestEqual(resultado.out, caso->salida);
        ExpectedError error = {caso->codigo, caso->linea, caso->columna};
        size_t cantidadErrores = 0;
        if (caso->codigo[0] != '\0') {
            cantidadErrores = 1;
        }
        Logs(resultado.err, &error, cantidadErrores, caso->infos, false);
        TestFree(resultado);
    }
    puts("Pruebas: 76/76");
}

/**
 * @brief Comprueba el límite de 32 variables y la reutilización de sus lugares.
 *
 * El primer escenario llena la tabla: reasignar sigue siendo válido, crear un
 * nombre adicional falla y liberar uno permite una nueva captura. El segundo
 * verifica que una captura cuyo valor falla por rango no reserve un lugar ni
 * publique una variable que pueda consultarse después.
 */
static void Capacidad(void) {
    char input[6000] = "";
    for (int i = 0; i < 32; ++i) {
        char sentencia[64];
        snprintf(sentencia, sizeof sentencia, "captura v%d = [mew];\n", i);
        strcat(input, sentencia);
    }
    strcat(input, "captura v0 = [ivysaur];\ncaptura v32 = [venusaur];\nv0;\nliberar v1;\ncaptura v32 = [venusaur];\nv0;\nv32;\n");
    ExpectedError e1[] = {{"E_LIMIT", 34, 9}};
    Check(input, "[ivysaur] (2)\n[ivysaur] (2)\n[venusaur] (3)\n", 1, e1, 1, 38);
    input[0] = 0;
    for (int i = 0; i < 31; ++i) {
        char sentencia[64];
        snprintf(sentencia, sizeof sentencia, "captura v%d = [mew];\n", i);
        strcat(input, sentencia);
    }
    strcat(input, "captura v31 = [bulbasaur mew mew mew mew mew mew];\ncaptura v32 = [ivysaur];\nv32;\nv31;\n");
    ExpectedError e2[] = {{"E_RANGE", 32, 15}, {"E_UNDEFINED", 35, 1}};
    Check(input, "[ivysaur] (2)\n", 1, e2, 2, 33);
    puts("Capacidad: 2/2");
}

/**
 * @brief Ejecuta la sesión de ejemplo con operaciones y errores recuperables.
 *
 * Combina precedencia, consulta, ditto, reasignación y liberación. Después de
 * consultar una variable liberada y encontrar un literal inválido, una captura
 * y una expresión válidas deben seguir ejecutándose en la misma sesión.
 */
static void SesionCompleta(void) {
    const char *input = "# Demostracion\n"
        "captura energia = [ivysaur] plusle [venusaur] tandemaus [ivysaur];\n"
        "energia;\nditto(energia);\ncaptura energia = energia minun [blastoise];\n"
        "energia;\nliberar energia;\nenergia;\n[pikachu] plusle [ivysaur];\n"
        "captura energia = [charmander];\nenergia minun [ivysaur];\n";
    ExpectedError e[] = {{"E_UNDEFINED", 8, 1}, {"E_MISSINGNO", 9, 1}};
    Check(input, "[wartortle] (8)\n[bulbasaur charizard] (16)\nminun [bulbasaur] (-1)\n[ivysaur] (2)\n", 1, e, 2, 8);
}

/**
 * @brief Verifica argumentos, formato exacto de trazas y su orden temporal.
 *
 * Se comparan una ejecución normal y otra con --trace; argumentos desconocidos
 * o repetidos deben producir E_USAGE y estado fatal. Durante la recuperación,
 * la traza conserva los tokens descartados pero solo se emite el primer error
 * recuperable de la sentencia, antes de continuar con la siguiente.
 */
static void CLI(void) {
    TestRunResult r = TestRun("[ivysaur];\n", "");
    assert(r.estado == 0);
    TestEqual(r.out, "[ivysaur] (2)\n");
    TestEqual(r.err, "INFO linea=1 columna=1 accion=evaluacion valor=2\n");
    TestFree(r);
    r = TestRun("[ivysaur];\n", "--trace");
    assert(r.estado == 0);
    TestEqual(r.out, "[ivysaur] (2)\n");
    TestEqual(r.err, "TRACE linea=1 columna=1 token=NATURAL valor=2\n"
        "TRACE linea=1 columna=10 token=';'\n"
        "INFO linea=1 columna=1 accion=evaluacion valor=2\n"
        "TRACE linea=2 columna=1 token=EOF\n");
    TestFree(r);
    const char *args[] = {"--foo", "--trace --trace"};
    for (int i = 0; i < 2; ++i) {
        r = TestRun("[ivysaur];\n", args[i]);
        assert(r.estado == 2);
        TestEqual(r.out, "");
        TestEqual(r.err, "ERROR codigo=E_USAGE mensaje=\"uso: pokecalc [--trace]\"\n");
        TestFree(r);
    }
    r = TestRun("[pikachu] @; [venusaur];\n", "--trace");
    assert(r.estado == 1);
    TestEqual(r.out, "[venusaur] (3)\n");
    const char *primero = "TRACE linea=1 columna=1 token=ERROR codigo=E_MISSINGNO\n";
    assert(strncmp(r.err, primero, strlen(primero)) == 0);
    const char *diagnostico = r.err + strlen(primero);
    assert(strncmp(diagnostico, "ERROR linea=1 columna=1 codigo=E_MISSINGNO mensaje=\"", 49) == 0);
    const char *resto = strchr(diagnostico, '\n');
    assert(resto != NULL);
    TestEqual(resto + 1, "TRACE linea=1 columna=11 token=ERROR codigo=E_LEXICAL\n"
        "TRACE linea=1 columna=12 token=';'\n"
        "TRACE linea=1 columna=14 token=NATURAL valor=3\n"
        "TRACE linea=1 columna=24 token=';'\n"
        "INFO linea=1 columna=14 accion=evaluacion valor=3\n"
        "TRACE linea=2 columna=1 token=EOF\n");
    ExpectedError e[] = {{"E_MISSINGNO", 1, 1}};
    Logs(r.err, e, 1, 1, true);
    TestFree(r);
    puts("Argumentos y traza: 5/5");
}

/**
 * @brief Prueba límites y situaciones que combinan varias capas del intérprete.
 *
 * Abarca operadores no reconocidos, atomicidad de asignación, pureza de ditto, lexemas
 * extensos, rango, expresiones largas y anidamiento excesivo. Cada caso comprueba
 * tanto el diagnóstico como la continuidad o terminación que le corresponde.
 */
static void Bordes(void) {
    /** Un identificador entre operandos no funciona como operador; el parser
     * informa un error sintáctico y continúa con la sentencia posterior. */
    ExpectedError operadorInvalido[] = {{"E_SYNTAX", 1, 11}};
    Check("[ivysaur] cociente [bulbasaur]; [venusaur];", "[venusaur] (3)\n", 1, operadorInvalido, 1, 1);
    /** Un error posterior a una captura no revierte su asignación ya confirmada. */
    const char *frontera = "captura energia = [ivysaur]; @; energia;";
    ExpectedError lex[] = {{"E_LEXICAL", 1, (int)(strchr(frontera, '@') - frontera) + 1}};
    Check(frontera, "[ivysaur] (2)\n", 1, lex, 1, 2);
    /** Una captura sin ; no modifica el valor anterior. ditto devuelve un valor
     * duplicado para la expresión sin alterar la variable consultada. */
    ExpectedError falta[] = {{"E_SYNTAX", 3, 1}};
    Check("captura a=[ivysaur];\ncaptura a=[venusaur]\na;\na;\n", "[ivysaur] (2)\n", 1, falta, 1, 2);
    Check("captura energia=[ivysaur]; ditto(energia); energia;", "[charmander] (4)\n[ivysaur] (2)\n", 0, NULL, 0, 3);

    /** El tamaño del texto guardado no limita la longitud reconocida: miles de
     * ceros iniciales conservan el valor uno. Un natural enorme sigue siendo un
     * token NATURAL fuera de rango y permite continuar tras el punto y coma. */
    char *source = calloc(180000, 1);
    assert(source != NULL);
    strcpy(source, "[");
    for (int i = 0; i < 4000; ++i) {
        strcat(source, "mew ");
    }
    strcat(source, "bulbasaur];");
    Check(source, "[bulbasaur] (1)\n", 0, NULL, 0, 1);
    strcpy(source, "[");
    for (int i = 0; i < 1000; ++i) {
        strcat(source, "blastoise ");
    }
    strcat(source, "];[ivysaur];");
    TestRunResult r = TestRun(source, "--trace");
    assert(r.estado == 1);
    TestEqual(r.out, "[ivysaur] (2)\n");
    ExpectedError rango[] = {{"E_RANGE", 1, 1}};
    Logs(r.err, rango, 1, 1, true);
    assert(strstr(r.err, "token=NATURAL fuera_de_rango=1\n") != NULL);
    TestFree(r);

    /** Identificadores y cifras arábigas excesivos se consumen completos; el
     * diagnóstico marca el recorte cuando corresponde y la sentencia sigue aislada. */
    memset(source, 'a', 4000);
    strcpy(source + 4000, ";[ivysaur];");
    ExpectedError largo[] = {{"E_LIMIT", 1, 1}};
    r = TestRun(source, "");
    assert(r.estado == 1);
    TestEqual(r.out, "[ivysaur] (2)\n");
    Logs(r.err, largo, 1, 1, false);
    assert(strstr(r.err, "(recortado)") != NULL);
    TestFree(r);
    memset(source, '9', 4000);
    strcpy(source + 4000, ";[ivysaur];");
    ExpectedError arabigo[] = {{"E_MISSINGNO", 1, 1}};
    Check(source, "[ivysaur] (2)\n", 1, arabigo, 1, 1);

    /** Una cadena larga de sumas prueba procesamiento secuencial sin exigir
     * una profundidad proporcional al número de operadores binarios. */
    strcpy(source, "[mew]");
    for (int i = 0; i < 4000; ++i) {
        strcat(source, " plusle [mew]");
    }
    strcat(source, ";");
    Check(source, "[mew] (0)\n", 0, NULL, 0, 1);
    /** El anidamiento supera la pila acotada del parser: E_RESOURCE debe ser
     * fatal y evitar que se ejecute incluso la sentencia válida que viene después. */
    memset(source, '(', 1000);
    strcpy(source + 1000, "[ivysaur]");
    memset(source + 1009, ')', 1000);
    strcpy(source + 2009, ";[venusaur];");
    ExpectedError recurso[] = {{"E_RESOURCE", 1, 257}};
    Check(source, "", 2, recurso, 1, 0);
    free(source);
    puts("Bordes, transacciones y recursos: assert OK");
}

#ifndef _WIN32
/**
 * @brief Fuerza errores de lectura y escritura en una plataforma POSIX.
 *
 * Si /dev/full está disponible, comprueba fallos al publicar resultados y
 * trazas. Leer un directorio como stdin provoca el fallo de entrada. En todos
 * los casos el proceso debe terminar con estado fatal, sin aparentar éxito.
 */
static void FallosIO(void) {
    TestWriteFile(".build/entrada.txt", "[ivysaur];\n");
    FILE *full = fopen("/dev/full", "wb");
    if (full != NULL) {
        assert(fclose(full) == 0);
        int status = system("./pokecalc < .build/entrada.txt > /dev/full 2> .build/registros.txt");
        assert(TestExitCode(status) == 2);
        char *log = TestReadFile(".build/registros.txt");
        ExpectedError e[] = {{"E_IO", 1, 1}};
        Logs(log, e, 1, 0, false);
        free(log);
        status = system("./pokecalc --trace < .build/entrada.txt > .build/salida.txt 2> /dev/full");
        assert(TestExitCode(status) == 2);
        char *out = TestReadFile(".build/salida.txt");
        TestEqual(out, "");
        free(out);
    }
    int status = system("./pokecalc < .build > .build/salida.txt 2> .build/registros.txt");
    assert(TestExitCode(status) == 2);
    char *log = TestReadFile(".build/registros.txt");
    ExpectedError e[] = {{"E_IO", 1, 1}};
    Logs(log, e, 1, 0, false);
    free(log);
    puts("Lectura y escritura fallidas: assert OK");
}

/**
 * @brief Comprueba que los argumentos inválidos se rechacen antes de leer stdin.
 *
 * Se conecta una tubería cuyo extremo de escritura permanece abierto, pero no
 * se envía información. Si pokecalc intentara leer, quedaría bloqueado esperando
 * datos. El padre observa la terminación sin bloquear y aplica un límite de
 * espera para que el fallo de la prueba no deje un proceso hijo ejecutándose.
 */
static void UsoSinLeer(void) {
    for (int doble = 0; doble < 2; ++doble) {
        int entrada[2];
        assert(pipe(entrada) == 0);
        pid_t child = fork();
        assert(child >= 0);
        /** El hijo recibe la tubería como stdin y registra stderr por separado;
         * el padre conserva el escritor abierto para impedir un EOF accidental. */
        if (child == 0) {
            assert(dup2(entrada[0], STDIN_FILENO) >= 0);
            close(entrada[0]);
            close(entrada[1]);
            int fd = open(".build/uso-sin-leer.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);
            assert(fd >= 0 && dup2(fd, STDERR_FILENO) >= 0);
            close(fd);
            if (doble) {
                execl("./pokecalc", "pokecalc", "--trace", "--trace", (char *)NULL);
            } else {
                execl("./pokecalc", "pokecalc", "--foo", (char *)NULL);
            }
            _Exit(127);
        }
        close(entrada[0]);
        /** El sondeo da hasta aproximadamente dos segundos para rechazar el uso,
         * antes de terminar y recoger un hijo que haya quedado bloqueado. */
        int status = 0;
        bool termino = false;
        for (int i = 0; i < 1000; ++i) {
            pid_t result = waitpid(child, &status, WNOHANG);
            assert(result >= 0);
            if (result == child) {
                termino = true;
                break;
            }
            struct timespec espera = {0, 2000000};
            nanosleep(&espera, NULL);
        }
        if (!termino) {
            kill(child, SIGKILL);
            waitpid(child, &status, 0);
        }
        close(entrada[1]);
        assert(termino && WIFEXITED(status) && WEXITSTATUS(status) == 2);
    }
}

/**
 * @brief Exige que el resultado de una sentencia esté disponible sin esperar EOF.
 *
 * Dos tuberías conectan la entrada y salida de un proceso hijo real. El padre
 * envía una sentencia terminada en ; y mantiene stdin abierto mientras espera
 * stdout con poll. Esto detecta tanto una lectura anticipada bloqueante como
 * la falta de vaciado del búfer de resultados al confirmar la sentencia.
 */
static void FlushInteractivo(void) {
    int entrada[2];
    int salida[2];
    assert(pipe(entrada) == 0 && pipe(salida) == 0);
    pid_t child = fork();
    assert(child >= 0);
    if (child == 0) {
        assert(dup2(entrada[0], STDIN_FILENO) >= 0 && dup2(salida[1], STDOUT_FILENO) >= 0);
        close(entrada[0]);
        close(entrada[1]);
        close(salida[0]);
        close(salida[1]);
        int fd = open(".build/interactivo.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);
        assert(fd >= 0 && dup2(fd, STDERR_FILENO) >= 0);
        close(fd);
        execl("./pokecalc", "pokecalc", (char *)NULL);
        _Exit(127);
    }
    /** El padre usa solo el escritor de entrada y el lector de salida. La
     * disponibilidad se comprueba antes de cerrar la entrada y producir EOF. */
    close(entrada[0]);
    close(salida[1]);
    const char *input = "[ivysaur];";
    assert(write(entrada[1], input, strlen(input)) == (ssize_t)strlen(input));
    struct pollfd descriptorSalida = {salida[0], POLLIN, 0};
    int ready = poll(&descriptorSalida, 1, 2000);
    if (ready <= 0) {
        kill(child, SIGKILL);
        waitpid(child, NULL, 0);
    }
    assert(ready > 0 && (descriptorSalida.revents & POLLIN));
    char out[128];
    ssize_t bytesLeidos = read(salida[0], out, sizeof out - 1);
    assert(bytesLeidos > 0);
    out[bytesLeidos] = '\0';
    TestEqual(out, "[ivysaur] (2)\n");
    /** Una vez recibido el resultado, cerrar stdin permite la terminación
     * normal del intérprete y waitpid comprueba que no hubo un fallo tardío. */
    close(entrada[1]);
    close(salida[0]);
    int status;
    assert(waitpid(child, &status, 0) == child);
    assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
    puts("Validacion previa y flush interactivo: assert OK");
}
#endif

/**
 * @brief Ejecuta todas las pruebas de aceptación en orden y de forma secuencial.
 * @return Cero si el corpus y los escenarios adicionales pasan sus aserciones.
 *
 * Las pruebas de tuberías, procesos y dispositivos especiales se incluyen solo
 * fuera de Windows; las verificaciones del lenguaje y de la CLI son comunes.
 */
int main(void) {
    Tabla();
    Capacidad();
    SesionCompleta();
    CLI();
    Bordes();
#ifndef _WIN32
    FallosIO();
    UsoSinLeer();
    FlushInteractivo();
#endif
    puts("Todos los PokeCasos fueron aprobados!");
    return 0;
}
