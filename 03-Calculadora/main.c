#include "Parser.h"
#include "errors.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

int main(int argc, char **argv) {
    bool trace = argc == 2 && strcmp(argv[1], "--trace") == 0;
#ifdef _WIN32
    /* Los registros terminan con LF, incluso para E_USAGE. */
    if (_setmode(_fileno(stderr), _O_BINARY) == -1) {
        Diagnostic error = MakeDiagnostic(E_IO, (SourcePosition){0, 0}, "No se pudo configurar los flujos.");
        ReportError(&error);
        return 2;
    }
#endif
    if (argc != 1 && !trace) {
        Diagnostic error = MakeDiagnostic(E_USAGE, (SourcePosition){0, 0}, "uso: pokecalc [--trace]");
        ReportError(&error);
        return 2;
    }
#ifdef _WIN32
    /* Solo se configura la entrada despues de validar los argumentos.
     * getchar debe ver CR, LF y todos los demas bytes sin traduccion. */
    if (_setmode(_fileno(stdin), _O_BINARY) == -1 ||
        _setmode(_fileno(stdout), _O_BINARY) == -1) {
        Diagnostic error = MakeDiagnostic(E_IO, (SourcePosition){0, 0}, "No se pudo configurar los flujos.");
        ReportError(&error);
        return 2;
    }
#endif
#ifdef SIGPIPE
    /* Un pipe cerrado debe producir E_IO, sin terminar por una senal. */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        Diagnostic error = MakeDiagnostic(E_RESOURCE, (SourcePosition){0, 0}, "No se pudo configurar SIGPIPE.");
        ReportError(&error);
        return 2;
    }
#endif
    int estado = ParserRun(trace);
    if (estado != 2 && fflush(stdout) != 0) {
        Diagnostic error = MakeDiagnostic(E_IO, (SourcePosition){0, 0}, "Fallo al cerrar la salida estandar.");
        ReportError(&error);
        estado = 2;
    }
    if (fflush(stderr) != 0) estado = 2;
    return estado;
}
