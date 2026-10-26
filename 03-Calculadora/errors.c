/**
 * @file errors.c
 * @brief Construcción de diagnósticos y escritura centralizada de stderr.
 *
 * Las funciones de reporte fijan el orden de las claves y delegan el texto
 * variable a un único codificador ASCII. Cada operación comprueba su resultado
 * y cada registro se vacía antes de devolver éxito; así el llamador puede
 * detectar también un error que aparezca al vaciar los búferes del flujo.
 *
 * Un fallo puede dejar una línea parcial: se devuelve false y se delega en
 * parser o main la terminación. No se intenta escribir un segundo mensaje en
 * el flujo que falló ni se llama a exit desde este módulo.
 */
#include "errors.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Nombres indexados en el mismo orden que ErrorCode, incluido E_NONE.
 * @details Ambas conversiones usan esta tabla para conservar una sola
 * correspondencia entre el código interno y su representación externa.
 */
static const char *const nombresDeErrores[] = {
    "E_NONE", "E_MISSINGNO", "E_LEXICAL", "E_LIMIT", "E_SYNTAX",
    "E_UNDEFINED", "E_RANGE", "E_IO", "E_RESOURCE", "E_USAGE"
};

/**
 * @brief Traduce un código al texto del catálogo y resuelve valores desconocidos.
 * @param codigo Código interno que se desea mostrar.
 * @return Nombre estático; E_RESOURCE si el índice no pertenece a la tabla.
 * @details Los límites nombrados evitan acceder fuera del arreglo. Si el
 * compilador representa ErrorCode con un entero sin signo, un valor negativo
 * convertido a este tipo resulta grande y lo rechaza la comparación superior.
 */
const char *ErrorCodeName(ErrorCode codigo) {
    if (codigo < E_NONE || codigo > E_USAGE) {
        return "E_RESOURCE";
    }
    return nombresDeErrores[codigo];
}

/**
 * @brief Busca un nombre textual completo en el catálogo compartido.
 * @param nombre Texto terminado en NUL o NULL.
 * @return Código exacto o E_RESOURCE como alternativa para nombres desconocidos.
 */
ErrorCode ErrorCodeFromName(const char *nombre) {
    if (nombre == NULL) {
        return E_RESOURCE;
    }

    /* sizeof mide bytes. Dividir el tamaño del arreglo por el de un elemento
     * da su cantidad de elementos; aquí ambos tamaños provienen del arreglo
     * completo, no de un puntero recibido como parámetro. */
    const size_t cantidadDeCodigos = sizeof nombresDeErrores / sizeof nombresDeErrores[0];
    for (size_t indice = 0; indice < cantidadDeCodigos; ++indice) {
        if (strcmp(nombre, nombresDeErrores[indice]) == 0) {
            /* La posición de cada nombre coincide con el valor de su enum. */
            return (ErrorCode)indice;
        }
    }
    return E_RESOURCE;
}

/**
 * @brief Separa la creación del diagnóstico de su eventual escritura.
 * @param codigo Causa detectada por el módulo llamador.
 * @param posicion Ubicación que se conservará sin reinterpretar.
 * @param detalle Texto a copiar; NULL o vacío activa una explicación genérica.
 * @return Diagnóstico independiente de la memoria del mensaje original.
 * @details La gravedad depende del código y el texto conserva terminación NUL.
 * Si snprintf informa una longitud mayor que la capacidad útil, se sobrescribe
 * el tramo final con una marca de recorte en lugar de ocultar la pérdida.
 */
Diagnostic MakeDiagnostic(ErrorCode codigo, SourcePosition posicion, const char *detalle) {
    Diagnostic resultado = {0};
    resultado.codigo = codigo;
    resultado.gravedad = ERROR_RECOVERABLE;
    if (codigo == E_IO || codigo == E_RESOURCE || codigo == E_USAGE) {
        resultado.gravedad = ERROR_FATAL;
    }
    resultado.posicion = posicion;

    /* Un puntero NULL no señala una cadena: hay que comprobarlo antes de
     * consultar detalle[0]. El mensaje genérico cubre ambos casos sin texto. */
    const char *mensaje = "Fallo sin detalle adicional.";
    if (detalle != NULL) {
        if (detalle[0] != '\0') {
            mensaje = detalle;
        }
    }
    int caracteresNecesarios = snprintf(resultado.detalle, sizeof resultado.detalle,
                                       "%s", mensaje);
    if (caracteresNecesarios >= (int)sizeof resultado.detalle) {
        static const char marca[] = "... (recortado)";
        /* sizeof marca incluye el NUL final: al copiarlo también se conserva
         * la terminación de la cadena dentro de la capacidad del destino. */
        size_t inicioDeLaMarca = sizeof resultado.detalle - sizeof marca;
        memcpy(&resultado.detalle[inicioDeLaMarca], marca, sizeof marca);
    }
    return resultado;
}

/**
 * @brief Escribe texto como ASCII imprimible, escapando cada byte por separado.
 * @param texto Cadena terminada en NUL; NULL equivale a una cadena vacía.
 * @param entreComillas Indica si también se protegen comillas y barras inversas.
 * @return true si se escribieron todos los bytes; false al primer fallo de stderr.
 * @details LF, CR y tabulación se representan por escapes cortos. Los demás
 * bytes fuera de 0x20..0x7E se representan como barra inversa, x y dos cifras
 * hexadecimales en mayúsculas. El recorrido usa unsigned char para que los
 * bytes mayores que 127 no se conviertan en valores negativos al formatearlos.
 * No decodifica UTF-8: una é codificada como C3 A9 produce dos escapes de byte.
 * No agrega comillas ni salto final; eso corresponde a la función del registro.
 */
static bool TextoASCII(const char *texto, bool entreComillas) {
    if (texto == NULL) {
        texto = "";
    }

    /* El puntero recorre la cadena sin copiarla ni tomar su propiedad.
     * unsigned char permite leer cada byte como un valor no negativo, incluso
     * los bytes de UTF-8 que quedan fuera del rango ASCII. */
    const unsigned char *caracterActual = (const unsigned char *)texto;
    while (*caracterActual != '\0') {
        unsigned char caracter = *caracterActual;
        const char *escape = NULL;
        switch (caracter) {
            case '\n':
                escape = "\\n";
                break;
            case '\r':
                escape = "\\r";
                break;
            case '\t':
                escape = "\\t";
                break;
            case '\\':
                if (entreComillas) {
                    escape = "\\\\";
                }
                break;
            case '"':
                if (entreComillas) {
                    escape = "\\\"";
                }
                break;
            default:
                break;
        }
        if (escape != NULL) {
            if (fputs(escape, stderr) == EOF) {
                return false;
            }
        } else if (caracter < 0x20 || caracter > 0x7e) {
            /* %X espera unsigned int; el cast adapta el byte a ese formato. */
            if (fprintf(stderr, "\\x%02X", (unsigned int)caracter) < 0) {
                return false;
            }
        } else {
            if (fputc(caracter, stderr) == EOF) {
                return false;
            }
        }
        ++caracterActual;
    }
    return true;
}

/**
 * @brief Cierra una línea de registro con LF y fuerza la escritura pendiente.
 * @return true si ambas operaciones tienen éxito; false si falla cualquiera.
 * @details Si falla el terminador se retorna antes de intentar fflush. Si se
 * pudo escribir, fflush permite detectar un error que el búfer había postergado.
 */
static bool FinRegistro(void) {
    if (fputc('\n', stderr) == EOF) {
        return false;
    }
    if (fflush(stderr) != 0) {
        return false;
    }
    return true;
}

/**
 * @brief Serializa código, ubicación opcional y causa de un diagnóstico ERROR.
 * @param error Diagnóstico válido que se desea comunicar, distinto de NULL.
 * @return Resultado de escribir el registro y vaciar stderr.
 * @details La ubicación sólo aparece con ambas coordenadas positivas. El campo
 * mensaje se delimita con comillas y se escapa en ese contexto; la gravedad
 * queda disponible para el control del llamador y no forma parte de la línea.
 */
bool ReportError(const Diagnostic *error) {
    /* Cada escritura se comprueba antes de intentar la siguiente. Así un fallo
     * puede dejar una línea parcial, pero nunca origina más escrituras aquí. */
    if (fputs("ERROR", stderr) == EOF) {
        return false;
    }
    if (error->posicion.linea > 0 && error->posicion.columna > 0) {
        if (fprintf(stderr, " linea=%d columna=%d", error->posicion.linea,
                    error->posicion.columna) < 0) {
            return false;
        }
    }
    if (fprintf(stderr, " codigo=%s mensaje=\"", ErrorCodeName(error->codigo)) < 0) {
        return false;
    }

    const char *mensaje = error->detalle;
    if (mensaje[0] == '\0') {
        mensaje = "Fallo sin detalle adicional.";
    }
    if (!TextoASCII(mensaje, true)) {
        return false;
    }
    if (fputc('"', stderr) == EOF) {
        return false;
    }
    return FinRegistro();
}

/**
 * @brief Forma un INFO con los campos pertinentes a una acción completada.
 * @param posicion Ubicación de la sentencia confirmada.
 * @param accion Asignación, evaluación o liberación.
 * @param nombre Nombre para asignación/liberación; se ignora en evaluación.
 * @param valor Valor para asignación/evaluación; se ignora en liberación.
 * @return true si el registro se escribió y vació; false por acción desconocida
 * o por fallo de salida.
 * @details Primero se valida la acción para indexar su tabla de nombres. La
 * base común siempre incluye posición y acción. Dos condiciones agregan los
 * campos opcionales en orden variable, valor; PRId64 mantiene el formato
 * portable de los enteros int64_t que usa la semántica.
 */
bool ReportInfo(SourcePosition posicion, InfoAction accion, const char *nombre, int64_t valor) {
    static const char *const acciones[] = {
        "asignacion", "evaluacion", "liberacion"
    };
    if (accion < INFO_ASIGNACION || accion > INFO_LIBERACION) {
        return false;
    }
    if (fprintf(stderr, "INFO linea=%d columna=%d accion=%s", posicion.linea,
                posicion.columna, acciones[accion]) < 0) {
        return false;
    }
    if (accion != INFO_EVALUACION) {
        if (fputs(" variable=", stderr) == EOF) {
            return false;
        }
        if (!TextoASCII(nombre, false)) {
            return false;
        }
    }
    if (accion != INFO_LIBERACION) {
        if (fprintf(stderr, " valor=%" PRId64, valor) < 0) {
            return false;
        }
    }
    return FinRegistro();
}

/**
 * @brief Forma un TRACE cuya variante de atributo selecciona sus campos finales.
 * @param traza Datos válidos del token; el puntero no puede ser NULL.
 * @return true al completar y vaciar la línea; false ante un fallo de stderr.
 * @details Todos los tokens comparten posición y clase. El switch agrega sólo
 * lo aplicable: valor o marca de rango, nombre, código o ningún campo. Un natural
 * fuera de rango omite su acumulador porque ya no representa el número completo.
 * La función no solicita otro token ni activa la opción --trace por su cuenta.
 */
bool ReportTrace(const TraceRecord *traza) {
    if (fprintf(stderr, "TRACE linea=%d columna=%d token=", traza->posicion.linea,
                traza->posicion.columna) < 0) {
        return false;
    }
    if (!TextoASCII(traza->clase, false)) {
        return false;
    }
    switch (traza->atributo) {
        case TRACE_NATURAL:
            if (traza->fuera_de_rango) {
                if (fputs(" fuera_de_rango=1", stderr) == EOF) {
                    return false;
                }
            } else {
                if (fprintf(stderr, " valor=%" PRId64, traza->valor) < 0) {
                    return false;
                }
            }
            break;
        case TRACE_IDENTIFIER:
            if (fputs(" nombre=", stderr) == EOF) {
                return false;
            }
            if (!TextoASCII(traza->nombre, false)) {
                return false;
            }
            break;
        case TRACE_ERROR:
            if (fprintf(stderr, " codigo=%s", ErrorCodeName(traza->codigo)) < 0) {
                return false;
            }
            break;
        case TRACE_NONE:
            break;
    }
    return FinRegistro();
}
