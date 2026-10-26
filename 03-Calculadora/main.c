/**
 * @file main.c
 * @brief Inicio del intérprete y preparación de los flujos de entrada y salida.
 *
 * Este archivo traduce los argumentos del proceso a una opción del parser,
 * prepara las particularidades de cada plataforma y conserva un estado de
 * salida coherente ante errores de escritura. La interpretación del lenguaje
 * pertenece a ParserRun(); los registros se formatean en el módulo errors.
 */
#include "Parser.h"
#include "errors.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

/**
 * @brief Configura el entorno de ejecución e interpreta stdin hasta EOF.
 * @param argc Cantidad de argumentos, incluido el nombre del programa.
 * @param argv Argumentos; la única opción admitida es @c --trace.
 * @retval 0 La sesión terminó sin rechazar sentencias ni fallar los flujos.
 * @retval 1 Hubo errores recuperables de entrada y se alcanzó el fin de la sesión.
 * @retval 2 Hubo un error fatal, argumentos inválidos o un fallo de entrada/salida.
 *
 * Los resultados se escriben en stdout y los diagnósticos, INFO y trazas en
 * stderr. La configuración y el vaciado final también pueden elevar el estado
 * a 2, aunque las expresiones se hayan interpretado correctamente.
 */
int main(int argc, char **argv) {
    /** La opción se acepta únicamente como argumento único. Cualquier otra
     * combinación se rechaza antes de solicitar tokens o consumir stdin. */
    bool trace = false;
    SourcePosition sinPosicionDeFuente = {0, 0};
    if (argc == 2) {
        /* argv[1] solo se consulta si existe; strcmp devuelve cero si coincide. */
        trace = strcmp(argv[1], "--trace") == 0;
    }
#ifdef _WIN32
    /** Windows suele traducir LF a CRLF en flujos de texto. Preparar stderr
     * primero conserva el formato de los registros incluso si los argumentos
     * resultan inválidos y se informa E_USAGE inmediatamente. */
    if (_setmode(_fileno(stderr), _O_BINARY) == -1) {
        Diagnostic error = MakeDiagnostic(E_IO, sinPosicionDeFuente, "No se pudo configurar los flujos.");
        ReportError(&error); /* & entrega la dirección del diagnóstico al módulo errors. */
        return 2;
    }
#endif
    if (argc != 1 && !trace) {
        Diagnostic error = MakeDiagnostic(E_USAGE, sinPosicionDeFuente, "uso: pokecalc [--trace]");
        ReportError(&error);
        return 2;
    }
#ifdef _WIN32
    /** Tras validar los argumentos, el modo binario permite que el scanner
     * vea CR, LF y el resto de los bytes originales, sin traducciones de texto.
     * stdout también usa LF para los resultados, igual que en otras plataformas. */
    if (_setmode(_fileno(stdin), _O_BINARY) == -1) {
        Diagnostic error = MakeDiagnostic(E_IO, sinPosicionDeFuente, "No se pudo configurar los flujos.");
        ReportError(&error);
        return 2;
    }
    if (_setmode(_fileno(stdout), _O_BINARY) == -1) {
        Diagnostic error = MakeDiagnostic(E_IO, sinPosicionDeFuente, "No se pudo configurar los flujos.");
        ReportError(&error);
        return 2;
    }
#endif
#ifdef SIGPIPE
    /** Si la plataforma dispone de SIGPIPE, ignorarla hace que una tubería
     * cerrada se detecte al escribir. El intérprete puede entonces informar
     * E_IO y devolver el mismo estado fatal que ante otros fallos de salida. */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        Diagnostic error = MakeDiagnostic(E_RESOURCE, sinPosicionDeFuente, "No se pudo configurar SIGPIPE.");
        ReportError(&error);
        return 2;
    }
#endif
    /** El parser administra su sesión y devuelve la gravedad acumulada.
     * El vaciado final detecta fallos que un búfer pudiera haber diferido;
     * cualquier fallo de stderr termina con 2, sin escribir otro error allí. */
    int estado = ParserRun(trace);
    if (estado != 2) {
        if (fflush(stdout) != 0) {
            Diagnostic error = MakeDiagnostic(E_IO, sinPosicionDeFuente, "Fallo al cerrar la salida estandar.");
            ReportError(&error);
            estado = 2;
        }
    }
    if (fflush(stderr) != 0) {
        estado = 2;
    }
    return estado;
}
