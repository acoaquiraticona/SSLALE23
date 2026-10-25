/* Ayudas de pruebas: las verificaciones siempre usan assert de C. */
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

static inline char *TestReadFile(const char *path) {
    FILE *f = fopen(path, "rb");
    assert(f != NULL);
    assert(fseek(f, 0, SEEK_END) == 0);
    long n = ftell(f);
    assert(n >= 0);
    rewind(f);
    char *text = malloc((size_t)n + 1);
    assert(text != NULL);
    assert(fread(text, 1, (size_t)n, f) == (size_t)n);
    text[n] = '\0';
    assert(fclose(f) == 0);
    return text;
}

static inline void TestEqual(const char *actual, const char *expected) {
    if (strcmp(actual, expected) != 0) {
        fprintf(stderr, "Esperado:\n%s\nRecibido:\n%s\n", expected, actual);
        assert(strcmp(actual, expected) == 0);
    }
}

static inline void TestWriteFile(const char *path, const char *source) {
    FILE *f = fopen(path, "wb");
    assert(f != NULL);
    size_t n = strlen(source);
    assert(fwrite(source, 1, n, f) == n);
    assert(fclose(f) == 0);
}

typedef struct { int estado; char *out, *err; } TestRunResult;

static inline int TestExitCode(int status) {
    assert(status != -1);
#ifdef _WIN32
    return status;
#else
    assert(WIFEXITED(status));
    return WEXITSTATUS(status);
#endif
}

static inline TestRunResult TestRun(const char *source, const char *args) {
    TestWriteFile(".build/entrada.txt", source);
    char command[256];
    int n = snprintf(command, sizeof command,
                     "./pokecalc %s < .build/entrada.txt > .build/salida.txt 2> .build/registros.txt", args);
    assert(n > 0 && n < (int)sizeof command);
    TestRunResult result;
    result.estado = TestExitCode(system(command));
    result.out = TestReadFile(".build/salida.txt");
    result.err = TestReadFile(".build/registros.txt");
    return result;
}

static inline void TestFree(TestRunResult result) {
    free(result.out);
    free(result.err);
}
#endif
