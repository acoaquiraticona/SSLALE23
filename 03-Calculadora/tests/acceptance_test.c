/* Aceptacion del ejecutable real; comparaciones y verificaciones con assert. */
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

typedef struct { const char *codigo; int linea, columna; } ExpectedError;

static void Logs(const char *log, const ExpectedError *errores, size_t cantidad,
                 int infosEsperados, bool trace) {
    size_t e = 0;
    int infos = 0;
    for (const unsigned char *p = (const unsigned char *)log; *p; ++p)
        assert(*p == '\n' || (*p >= 32 && *p <= 126));
    while (*log) {
        const char *fin = strchr(log, '\n');
        assert(fin != NULL);
        size_t n = (size_t)(fin - log);
        char linea[4096];
        assert(n > 0 && n < sizeof linea);
        memcpy(linea, log, n); linea[n] = 0;
        if (strncmp(linea, "ERROR ", 6) == 0) {
            assert(e < cantidad);
            int l, c;
            char codigo[32];
            assert(sscanf(linea, "ERROR linea=%d columna=%d codigo=%31s", &l, &c, codigo) == 3);
            TestEqual(codigo, errores[e].codigo);
            if (l != errores[e].linea || c != errores[e].columna)
                fprintf(stderr, "Ubicacion esperada %d:%d; recibida %d:%d\n", errores[e].linea, errores[e].columna, l, c);
            assert(l == errores[e].linea && c == errores[e].columna);
            const char *causa = strstr(linea, " mensaje=\"");
            assert(causa != NULL && causa[10] != '"' && causa[10] != 0);
            assert(linea[n - 1] == '"');
            ++e;
        } else if (strncmp(linea, "INFO ", 5) == 0) {
            int l, c;
            char accion[32];
            assert(sscanf(linea, "INFO linea=%d columna=%d accion=%31s", &l, &c, accion) == 3);
            assert(l > 0 && c > 0);
            assert(strcmp(accion, "asignacion") == 0 || strcmp(accion, "liberacion") == 0 || strcmp(accion, "evaluacion") == 0);
            if (strcmp(accion, "evaluacion") != 0) assert(strstr(linea, " variable=") != NULL);
            if (strcmp(accion, "liberacion") != 0) assert(strstr(linea, " valor=") != NULL);
            ++infos;
        } else {
            assert(trace && strncmp(linea, "TRACE ", 6) == 0);
        }
        log = fin + 1;
    }
    assert(e == cantidad && infos == infosEsperados);
}

static void Check(const char *input, const char *output, int status,
                  const ExpectedError *errors, size_t n, int infos) {
    TestRunResult r = TestRun(input, "");
    TestEqual(r.out, output);
    assert(r.estado == status);
    Logs(r.err, errors, n, infos, false);
    TestFree(r);
}

static void Tabla(void) {
    size_t cantidad = sizeof casos / sizeof casos[0];
    assert(cantidad == 76);
    for (size_t i = 0; i < cantidad; ++i) {
        const AcceptanceCase *c = &casos[i];
        TestRunResult r = TestRun(c->entrada, "");
        if (r.estado != c->estado || strcmp(r.out, c->salida) != 0)
            fprintf(stderr, "Fallo en %s\n%s", c->id, r.err);
        assert(r.estado == c->estado);
        TestEqual(r.out, c->salida);
        ExpectedError error = {c->codigo, c->linea, c->columna};
        Logs(r.err, &error, *c->codigo ? 1 : 0, c->infos, false);
        TestFree(r);
    }
    puts("Pruebas: 76/76");
}

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

static void SesionCompleta(void) {
    const char *input = "# Demostracion\n"
        "captura energia = [ivysaur] plusle [venusaur] tandemaus [ivysaur];\n"
        "energia;\nditto(energia);\ncaptura energia = energia minun [blastoise];\n"
        "energia;\nliberar energia;\nenergia;\n[pikachu] plusle [ivysaur];\n"
        "captura energia = [charmander];\nenergia minun [ivysaur];\n";
    ExpectedError e[] = {{"E_UNDEFINED", 8, 1}, {"E_MISSINGNO", 9, 1}};
    Check(input, "[wartortle] (8)\n[bulbasaur charizard] (16)\nminun [bulbasaur] (-1)\n[ivysaur] (2)\n", 1, e, 2, 8);
}

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

static void Bordes(void) {
    Check("captura falinks = [ivysaur]; falinks;", "[ivysaur] (2)\n", 0, NULL, 0, 2);
    ExpectedError division[] = {{"E_SYNTAX", 1, 11}};
    Check("[ivysaur] falinks [bulbasaur]; [venusaur];", "[venusaur] (3)\n", 1, division, 1, 1);
    const char *frontera = "captura energia = [ivysaur]; @; energia;";
    ExpectedError lex[] = {{"E_LEXICAL", 1, (int)(strchr(frontera, '@') - frontera) + 1}};
    Check(frontera, "[ivysaur] (2)\n", 1, lex, 1, 2);
    ExpectedError falta[] = {{"E_SYNTAX", 3, 1}};
    Check("captura a=[ivysaur];\ncaptura a=[venusaur]\na;\na;\n", "[ivysaur] (2)\n", 1, falta, 1, 2);
    Check("captura energia=[ivysaur]; ditto(energia); energia;", "[charmander] (4)\n[ivysaur] (2)\n", 0, NULL, 0, 3);

    char *source = calloc(180000, 1);
    assert(source != NULL);
    strcpy(source, "[");
    for (int i = 0; i < 4000; ++i) strcat(source, "mew ");
    strcat(source, "bulbasaur];");
    Check(source, "[bulbasaur] (1)\n", 0, NULL, 0, 1);
    strcpy(source, "[");
    for (int i = 0; i < 1000; ++i) strcat(source, "blastoise ");
    strcat(source, "];[ivysaur];");
    TestRunResult r = TestRun(source, "--trace");
    assert(r.estado == 1);
    TestEqual(r.out, "[ivysaur] (2)\n");
    ExpectedError rango[] = {{"E_RANGE", 1, 1}};
    Logs(r.err, rango, 1, 1, true);
    assert(strstr(r.err, "token=NATURAL fuera_de_rango=1\n") != NULL);
    TestFree(r);

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

    strcpy(source, "[mew]");
    for (int i = 0; i < 4000; ++i) strcat(source, " plusle [mew]");
    strcat(source, ";");
    Check(source, "[mew] (0)\n", 0, NULL, 0, 1);
    /* Recursos: una sentencia demasiado anidada termina con E_RESOURCE,
     * no SIGSEGV ni recuperacion que ejecute la sentencia posterior. */
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
        TestEqual(out, ""); free(out);
    }
    int status = system("./pokecalc < .build > .build/salida.txt 2> .build/registros.txt");
    assert(TestExitCode(status) == 2);
    char *log = TestReadFile(".build/registros.txt");
    ExpectedError e[] = {{"E_IO", 1, 1}};
    Logs(log, e, 1, 0, false); free(log);
    puts("Lectura y escritura fallidas: assert OK");
}

static void UsoSinLeer(void) {
    for (int doble = 0; doble < 2; ++doble) {
        int entrada[2];
        assert(pipe(entrada) == 0);
        pid_t child = fork(); assert(child >= 0);
        if (child == 0) {
            assert(dup2(entrada[0], STDIN_FILENO) >= 0);
            close(entrada[0]); close(entrada[1]);
            int fd = open(".build/uso-sin-leer.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);
            assert(fd >= 0 && dup2(fd, STDERR_FILENO) >= 0); close(fd);
            if (doble) execl("./pokecalc", "pokecalc", "--trace", "--trace", (char *)NULL);
            else execl("./pokecalc", "pokecalc", "--foo", (char *)NULL);
            _Exit(127);
        }
        close(entrada[0]);
        int status = 0; bool termino = false;
        for (int i = 0; i < 1000; ++i) {
            pid_t result = waitpid(child, &status, WNOHANG);
            assert(result >= 0);
            if (result == child) { termino = true; break; }
            struct timespec espera = {0, 2000000};
            nanosleep(&espera, NULL);
        }
        if (!termino) { kill(child, SIGKILL); waitpid(child, &status, 0); }
        close(entrada[1]);
        assert(termino && WIFEXITED(status) && WEXITSTATUS(status) == 2);
    }
}

static void FlushInteractivo(void) {
    int entrada[2], salida[2];
    assert(pipe(entrada) == 0 && pipe(salida) == 0);
    pid_t child = fork(); assert(child >= 0);
    if (child == 0) {
        assert(dup2(entrada[0], STDIN_FILENO) >= 0 && dup2(salida[1], STDOUT_FILENO) >= 0);
        close(entrada[0]); close(entrada[1]); close(salida[0]); close(salida[1]);
        int fd = open(".build/interactivo.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);
        assert(fd >= 0 && dup2(fd, STDERR_FILENO) >= 0); close(fd);
        execl("./pokecalc", "pokecalc", (char *)NULL);
        _Exit(127);
    }
    close(entrada[0]); close(salida[1]);
    const char *input = "[ivysaur];";
    assert(write(entrada[1], input, strlen(input)) == (ssize_t)strlen(input));
    struct pollfd p = {salida[0], POLLIN, 0};
    int ready = poll(&p, 1, 2000);
    if (ready <= 0) { kill(child, SIGKILL); waitpid(child, NULL, 0); }
    assert(ready > 0 && (p.revents & POLLIN));
    char out[128];
    ssize_t n = read(salida[0], out, sizeof out - 1); assert(n > 0);
    out[n] = 0;
    TestEqual(out, "[ivysaur] (2)\n");
    close(entrada[1]); close(salida[0]);
    int status;
    assert(waitpid(child, &status, 0) == child);
    assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
    puts("Validacion previa y flush interactivo: assert OK");
}
#endif

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
