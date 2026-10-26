/**
 * @file Parser.c
 * @brief Descenso recursivo con evaluación inmediata y recuperación por sentencia.
 *
 * La implementación traduce los niveles de precedencia de la gramática
 * en funciones: expresión para suma/resta, término para multiplicación, unario
 * para negación y primario para literales, variables, paréntesis y `ditto`.
 * Las colas de operadores binarios se recorren con bucles para conservar la
 * asociatividad izquierda sin aumentar la recursión por cada operación.
 *
 * Cada expresión produce un entero mediante Semantics. Las modificaciones de
 * variables y la salida de una evaluación se realizan únicamente después de
 * comprobar el punto y coma de la sentencia. Los errores recuperables descartan
 * tokens hasta ese delimitador; los fatales terminan la sesión.
 */

/* Implementacion de Calc.md 5-9. Un analizador por cada no terminal. */
#include "Parser.h"
#include "Semantics.h"
#include "errors.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Máxima cantidad de análisis unarios simultáneos en una expresión.
 *
 * Cada primario pasa por ParseUnary(); los paréntesis, `ditto` y las negaciones
 * anidadas pueden volver a entrar en ese nivel. El límite protege la pila de C
 * y produce E_RESOURCE; no restringe la longitud de una cadena de sumas ni el
 * rango numérico, que se comprueba por separado en Semantics.
 */
enum { MAX_PROFUNDIDAD = 256 };

/**
 * @brief Estado privado de una interpretación, reiniciado por ParserRun().
 *
 * `actual` guarda el token de anticipación y `hayToken` indica si sigue pendiente.
 * `trace` habilita los registros de tokens. `huboError` conserva el resultado
 * global, mientras que `sentenciaFallida` evita repetir diagnósticos recuperables
 * dentro de una misma sentencia. `fatal` impide continuar leyendo o evaluando.
 * `profundidad` cuenta las entradas activas a ParseUnary().
 */
static struct {
    Token actual;
    bool hayToken;
    bool trace;
    bool huboError;
    bool sentenciaFallida;
    bool fatal;
    unsigned profundidad;
} sesion;

/**
 * @brief Obtiene las coordenadas del token de anticipación.
 * @return Línea y columna del inicio de `sesion.actual`.
 * @pre El token actual ya fue solicitado al scanner.
 */
static SourcePosition Posicion(void) {
    SourcePosition posicion;
    posicion.linea = sesion.actual.linea;
    posicion.columna = sesion.actual.columna;
    return posicion;
}

/**
 * @brief Registra un fallo y actualiza el estado de la sentencia y de la sesión.
 *
 * Solo se informa el primer error recuperable de cada sentencia. Un diagnóstico
 * fatal todavía puede informarse después de ese primer error. Si falla la
 * escritura del propio registro también se detiene la interpretación.
 *
 * @param error Diagnóstico completo que se desea comunicar.
 * @return Siempre `false`, para propagar el fallo por las funciones de análisis.
 */
static bool Informar(Diagnostic error) {
    if (sesion.fatal) {
        return false;
    }
    if (!sesion.sentenciaFallida || error.gravedad == ERROR_FATAL) {
        /* &error entrega la dirección del diagnóstico para que ReportError
         * pueda consultar sus campos sin recibir otra copia de la estructura. */
        if (!ReportError(&error)) {
            sesion.fatal = true;
        }
    }
    sesion.huboError = true;
    sesion.sentenciaFallida = true;
    if (error.gravedad == ERROR_FATAL) {
        sesion.fatal = true;
    }
    return false;
}

/**
 * @brief Construye un diagnóstico común y lo transmite a Informar().
 * @param codigo Clase de error léxico, sintáctico, semántico o fatal.
 * @param posicion Lugar de la entrada que explica el fallo.
 * @param detalle Explicación concreta del problema.
 * @return Siempre `false`.
 */
static bool Fallar(ErrorCode codigo, SourcePosition posicion, const char *detalle) {
    Diagnostic error = MakeDiagnostic(codigo, posicion, detalle);
    return Informar(error);
}

/**
 * @brief Mantiene un único token de anticipación y emite su traza al obtenerlo.
 *
 * Si el token ya está almacenado no vuelve a leerlo ni a registrarlo. Al solicitar
 * uno nuevo convierte sus atributos al formato de traza y comprueba enseguida
 * los errores fatales del scanner, incluso durante la recuperación. Los errores
 * léxicos recuperables quedan disponibles para Disponible() o para su descarte.
 *
 * @return `true` si hay un token disponible; `false` ante un error fatal.
 */
static bool AsegurarToken(void) {
    if (sesion.fatal) {
        return false;
    }
    if (!sesion.hayToken) {
        sesion.actual = GetNextToken();
        sesion.hayToken = true;
        /** La traza describe el token antes de que el parser decida aceptarlo. */
        if (sesion.trace) {
            /* {0} inicializa todos los campos: los números quedan en cero y
             * los punteros quedan en NULL. Luego se completan los necesarios. */
            TraceRecord traza = {0};
            traza.posicion = Posicion();
            traza.clase = ScannerTokenName(sesion.actual.tipo);
            if (sesion.actual.tipo == TOKEN_NATURAL) {
                traza.atributo = TRACE_NATURAL;
                traza.valor = sesion.actual.valor;
                traza.fuera_de_rango = sesion.actual.fuera_de_rango;
            } else if (sesion.actual.tipo == TOKEN_IDENTIFIER) {
                traza.atributo = TRACE_IDENTIFIER;
                traza.nombre = sesion.actual.lexema;
            } else if (sesion.actual.tipo == TOKEN_ERROR) {
                traza.atributo = TRACE_ERROR;
                traza.codigo = sesion.actual.diagnostico.codigo;
            }
            if (!ReportTrace(&traza)) {
                sesion.fatal = true;
                return false;
            }
        }
        /* Tambien se verifica durante la recuperacion. */
        if (sesion.actual.tipo == TOKEN_ERROR &&
            sesion.actual.diagnostico.gravedad == ERROR_FATAL) {
            return Informar(sesion.actual.diagnostico);
        }
    }
    return true;
}

/**
 * @brief Exige un token utilizable por las producciones de la gramática.
 * @return `false` si falla la lectura o si el token actual contiene un error;
 *         en ese caso también se comunica el diagnóstico correspondiente.
 */
static bool Disponible(void) {
    if (!AsegurarToken()) {
        return false;
    }
    if (sesion.actual.tipo == TOKEN_ERROR) {
        return Informar(sesion.actual.diagnostico);
    }
    return true;
}

/**
 * @brief Marca el token actual como consumido sin solicitar otro al scanner.
 *
 * La lectura se difiere hasta la próxima necesidad de anticipación. Así los
 * efectos y el registro INFO de una sentencia preceden a la traza del primer
 * token de la siguiente, y no se detectan errores de esa siguiente sentencia
 * antes de terminar la actual.
 */
static void ConsumirToken(void) {
    sesion.hayToken = false;
}

/**
 * @brief Comprueba una clase de token sin consumirla.
 * @param esperado Clase exigida por la producción sintáctica actual.
 * @return `true` si coincide; `false` tras informar un error léxico o sintáctico.
 */
static bool Comprobar(TipoToken esperado) {
    if (!Disponible()) {
        return false;
    }
    if (sesion.actual.tipo != esperado) {
        char detalle[160];
        snprintf(detalle, sizeof detalle, "Se esperaba %s; se encontro %s.",
                 ScannerTokenName(esperado), ScannerTokenName(sesion.actual.tipo));
        return Fallar(E_SYNTAX, Posicion(), detalle);
    }
    return true;
}

/**
 * @brief Comprueba y consume un terminal obligatorio de la gramática.
 * @param esperado Clase del token que debe aparecer a continuación.
 * @return `true` si se pudo aceptar el terminal; `false` si se rechazó.
 */
static bool Esperar(TipoToken esperado) {
    if (!Comprobar(esperado)) {
        return false;
    }
    ConsumirToken();
    return true;
}

/**
 * @brief Integra una operación semántica con la ubicación conocida por el parser.
 *
 * Semantics conoce los valores y las reglas del dominio; el parser agrega la
 * posición del literal, identificador u operador responsable cuando hay un error.
 *
 * @param resultado Valor o diagnóstico devuelto por una función semántica.
 * @param posicion Coordenadas que se asignan al diagnóstico si la operación falla.
 * @param[out] valor Destino del valor obtenido; puede ser NULL si solo interesa
 *                   comprobar el éxito de una asignación o liberación.
 * @return `true` si la operación fue válida; `false` después de informar el fallo.
 */
static bool UsarResultado(ResultadoSem resultado, SourcePosition posicion, int64_t *valor) {
    if (!resultado.ok) {
        resultado.diagnostico.posicion = posicion;
        return Informar(resultado.diagnostico);
    }
    if (valor != NULL) {
        /* valor guarda una dirección; *valor accede al entero del llamador.
         * Así esta función devuelve éxito/fallo y también entrega un número.
         * NULL significa que el llamador no necesita recibir ese número. */
        *valor = resultado.valor;
    }
    return true;
}

static bool ParseTranslationUnit(void);
static bool ParseStatementList(void);
static bool ParseStatement(void);
static bool ParseExpression(int64_t *valor);
static bool ParseExpressionTail(int64_t *acumulado);
static bool ParseTerm(int64_t *valor);
static bool ParseTermTail(int64_t *acumulado);
static bool ParseUnary(int64_t *valor);
static bool ParsePrimary(int64_t *valor);

/**
 * @brief Sincroniza el análisis después de rechazar una sentencia.
 *
 * Descarta tokens hasta consumir un punto y coma, o se detiene al encontrar EOF.
 * Los errores léxicos recuperables del tramo descartado no generan diagnósticos
 * adicionales; su traza, si se pidió, ya se produjo al leerlos. Un error fatal
 * sí interrumpe el proceso porque AsegurarToken() siempre lo comprueba.
 *
 * @return `true` al alcanzar un límite de sentencia o EOF; `false` ante un fatal.
 */
static bool Recuperar(void) {
    /* for (;;) repite sin un límite fijo; los return del cuerpo terminan el bucle. */
    for (;;) {
        if (!AsegurarToken()) {
            return false;
        }
        if (sesion.actual.tipo == TOKEN_EOF) {
            return true;
        }
        if (sesion.actual.tipo == TOKEN_SEMICOLON) {
            ConsumirToken();
            return true;
        }
        /* Los ERROR recuperables se trazaron, pero no se vuelven a informar. */
        ConsumirToken();
    }
}

/**
 * @brief Reconoce la unidad de traducción: una lista de sentencias seguida de EOF.
 * @return `true` al terminar el recorrido, incluso si hubo rechazos recuperables;
 *         `false` si un error fatal impidió completar la entrada.
 */
static bool ParseTranslationUnit(void) {
    if (!ParseStatementList()) {
        return false;
    }
    if (!AsegurarToken()) {
        return false;
    }
    return sesion.actual.tipo == TOKEN_EOF;
}

/**
 * @brief Interpreta sucesivas sentencias y permite continuar después de un rechazo.
 *
 * La lista puede estar vacía. Antes de cada sentencia se habilita nuevamente el
 * primer diagnóstico; el indicador global `huboError` se conserva hasta el final.
 *
 * @return `true` al llegar a EOF; `false` si no es posible recuperarse.
 */
static bool ParseStatementList(void) {
    for (;;) {
        if (!AsegurarToken()) {
            return false;
        }
        if (sesion.actual.tipo == TOKEN_EOF) {
            return true;
        }
        sesion.sentenciaFallida = false;
        if (!ParseStatement()) {
            if (sesion.fatal) {
                return false;
            }
            if (!Recuperar()) {
                return false;
            }
        }
    }
}

/**
 * @brief Reconoce y ejecuta una asignación, una liberación o una evaluación.
 *
 * Durante el reconocimiento conserva en variables locales la acción solicitada,
 * el nombre destino y el valor calculado. Una expresión puede consultar variables
 * existentes, pero no modifica la tabla. Solo después de validar el `;` se invoca
 * SemAsignar(), SemLiberar() o la impresión del resultado. Por eso una captura
 * con expresión inválida conserva el valor anterior de la variable.
 *
 * @return `true` si se aceptó y registró la sentencia; `false` si debe recuperarse
 *         o detenerse la interpretación.
 */
static bool ParseStatement(void) {
    if (!Disponible()) {
        return false;
    }
    SourcePosition inicio = Posicion();
    SourcePosition posicionNombre = inicio;
    char nombre[SEM_MAX_NOMBRE + 1] = "";
    InfoAction accion = INFO_EVALUACION;
    int64_t valor = 0;
    /** El primer token determina cuál de las tres formas de sentencia se analiza. */
    if (sesion.actual.tipo == TOKEN_CAPTURA) {
        accion = INFO_ASIGNACION;
        ConsumirToken();
        if (!Comprobar(TOKEN_IDENTIFIER)) {
            return false;
        }
        posicionNombre = Posicion();
        /* El +1 incluye el '\0' que marca el final de una cadena en C. */
        memcpy(nombre, sesion.actual.lexema, strlen(sesion.actual.lexema) + 1);
        ConsumirToken();
        if (!Esperar(TOKEN_EQUAL)) {
            return false;
        }
        /* &valor permite que ParseExpression escriba el resultado en esta
         * variable. Su retorno bool indica por separado si pudo calcularlo. */
        if (!ParseExpression(&valor)) {
            return false;
        }
    } else if (sesion.actual.tipo == TOKEN_LIBERAR) {
        accion = INFO_LIBERACION;
        ConsumirToken();
        if (!Comprobar(TOKEN_IDENTIFIER)) {
            return false;
        }
        posicionNombre = Posicion();
        memcpy(nombre, sesion.actual.lexema, strlen(sesion.actual.lexema) + 1);
        ConsumirToken();
    } else {
        if (!ParseExpression(&valor)) {
            return false;
        }
    }

    /**
     * El punto y coma permanece pendiente durante la aplicación de los efectos.
     * Si SemAsignar() devuelve E_LIMIT o SemLiberar() devuelve E_UNDEFINED,
     * Recuperar() consume este mismo delimitador y preserva la siguiente sentencia.
     * La escritura de resultados también se comprueba para detectar E_IO.
     */
    if (!Comprobar(TOKEN_SEMICOLON)) {
        return false;
    }
    if (accion == INFO_ASIGNACION) {
        ResultadoSem resultado = SemAsignar(nombre, valor);
        if (!UsarResultado(resultado, posicionNombre, NULL)) {
            return false;
        }
    } else if (accion == INFO_LIBERACION) {
        ResultadoSem resultado = SemLiberar(nombre);
        if (!UsarResultado(resultado, posicionNombre, NULL)) {
            return false;
        }
    } else {
        char pokemon[128];
        SemAPokemon(valor, pokemon, sizeof pokemon);
        /* PRId64 aporta el formato para int64_t; C une las cadenas adyacentes. */
        int caracteresEscritos = fprintf(stdout, "%s (%" PRId64 ")\n", pokemon, valor);
        if (caracteresEscritos < 0) {
            return Fallar(E_IO, inicio, "Fallo al escribir el resultado en stdout.");
        }
        /* Se vacía la salida solo si fprintf tuvo éxito, antes de seguir
         * leyendo: una entrada interactiva puede continuar abierta. */
        if (fflush(stdout) != 0) {
            return Fallar(E_IO, inicio, "Fallo al escribir el resultado en stdout.");
        }
    }
    /** INFO se emite después del efecto exitoso y antes de solicitar otro token. */
    if (!ReportInfo(inicio, accion, nombre, valor)) {
        sesion.fatal = true;
        return false;
    }
    ConsumirToken();
    return true;
}

/**
 * @brief Evalúa `expression -> term expression_tail`.
 * @param[out] valor Resultado de toda la expresión si el análisis tiene éxito.
 * @return `true` si los términos y las operaciones son válidos.
 */
static bool ParseExpression(int64_t *valor) {
    if (!ParseTerm(valor)) {
        return false;
    }
    return ParseExpressionTail(valor);
}

/**
 * @brief Acumula sumas y restas de izquierda a derecha.
 *
 * Cada operando derecho se obtiene como un término completo, por lo que las
 * multiplicaciones se resuelven antes que la suma o la resta. Cada resultado
 * intermedio se valida en Semantics; no se espera a calcular el resultado final
 * para detectar un valor fuera del dominio. `)` y `;` terminan este nivel y
 * quedan pendientes para quien analizó la agrupación o la sentencia.
 *
 * @param[in,out] acumulado Valor del primer término y resultado parcial/final.
 * @return `true` al alcanzar un delimitador válido; `false` ante un fallo.
 */
static bool ParseExpressionTail(int64_t *acumulado) {
    for (;;) {
        if (!Disponible()) {
            return false;
        }
        TipoToken operador = sesion.actual.tipo;
        if (operador != TOKEN_PLUSLE && operador != TOKEN_MINUN) {
            if (operador == TOKEN_SEMICOLON || operador == TOKEN_RPAREN) {
                return true;
            }
            return Fallar(E_SYNTAX, Posicion(), "Se esperaba plusle, minun, ')' o ';'.");
        }
        SourcePosition posicion = Posicion();
        ConsumirToken();
        int64_t operandoDerecho;
        if (!ParseTerm(&operandoDerecho)) {
            return false;
        }
        ResultadoSem resultado;
        if (operador == TOKEN_PLUSLE) {
            resultado = SemSumar(*acumulado, operandoDerecho);
        } else {
            resultado = SemRestar(*acumulado, operandoDerecho);
        }
        if (!UsarResultado(resultado, posicion, acumulado)) {
            return false;
        }
    }
}

/**
 * @brief Evalúa `term -> unary term_tail`, nivel de las multiplicaciones.
 * @param[out] valor Resultado del término si el análisis tiene éxito.
 * @return `true` si todos sus factores y productos son válidos.
 */
static bool ParseTerm(int64_t *valor) {
    if (!ParseUnary(valor)) {
        return false;
    }
    return ParseTermTail(valor);
}

/**
 * @brief Acumula productos `tandemaus` sin consumir operadores de menor precedencia.
 *
 * Evalúa los factores de izquierda a derecha mediante ParseUnary(). Un operador
 * de suma/resta devuelve el control a ParseExpressionTail(); un delimitador
 * devuelve el control al análisis de la agrupación o de la sentencia.
 *
 * @param[in,out] acumulado Valor del primer factor y resultado parcial/final.
 * @return `true` si termina correctamente el término; `false` ante un fallo.
 */
static bool ParseTermTail(int64_t *acumulado) {
    for (;;) {
        if (!Disponible()) {
            return false;
        }
        if (sesion.actual.tipo != TOKEN_TANDEMAUS) {
            TipoToken tipoActual = sesion.actual.tipo;
            if (tipoActual == TOKEN_PLUSLE || tipoActual == TOKEN_MINUN ||
                tipoActual == TOKEN_SEMICOLON || tipoActual == TOKEN_RPAREN) {
                return true;
            }
            return Fallar(E_SYNTAX, Posicion(), "Se esperaba un operador, ')' o ';'.");
        }
        SourcePosition posicion = Posicion();
        ConsumirToken();
        int64_t operandoDerecho;
        if (!ParseUnary(&operandoDerecho)) {
            return false;
        }
        ResultadoSem resultado = SemMultiplicar(*acumulado, operandoDerecho);
        if (!UsarResultado(resultado, posicion, acumulado)) {
            return false;
        }
    }
}

/**
 * @brief Evalúa una negación prefija recursiva o un primario.
 *
 * Al llamar otra vez a este mismo nivel, varias negaciones se aplican desde la
 * más interna hacia afuera. Su precedencia es mayor que la de la multiplicación.
 * La profundidad se incrementa al entrar y se reduce al salir, también cuando
 * falla el análisis, para limitar únicamente el anidamiento simultáneo.
 *
 * @param[out] valor Valor del primario con las negaciones que correspondan.
 * @return `true` si se obtuvo un valor válido; `false` ante un fallo o E_RESOURCE.
 */
static bool ParseUnary(int64_t *valor) {
    if (!Disponible()) {
        return false;
    }
    if (sesion.profundidad >= MAX_PROFUNDIDAD) {
        return Fallar(E_RESOURCE, Posicion(), "Se agoto la pila disponible para expresiones anidadas.");
    }
    ++sesion.profundidad;
    bool analisisCorrecto;
    if (sesion.actual.tipo == TOKEN_MINUN) {
        SourcePosition operador = Posicion();
        ConsumirToken();
        /* La llamada recursiva resuelve primero lo que sigue a minun.
         * Solo se niega ese resultado si el análisis interno tuvo éxito. */
        analisisCorrecto = ParseUnary(valor);
        if (analisisCorrecto) {
            ResultadoSem resultado = SemNegar(*valor);
            analisisCorrecto = UsarResultado(resultado, operador, valor);
        }
    } else {
        analisisCorrecto = ParsePrimary(valor);
    }
    /* También se resta al fallar: cuenta llamadas activas, no las realizadas
     * durante toda la sesión. No se retorna antes de restablecer este contador. */
    --sesion.profundidad;
    return analisisCorrecto;
}

/**
 * @brief Evalúa literales, referencias a variables, agrupaciones y `ditto(...)`.
 *
 * Los naturales se validan con la marca de rango producida por el scanner. Las
 * variables consultan la tabla semántica. Los paréntesis reinician el análisis
 * desde el nivel de expresión; `ditto` hace lo mismo y duplica el valor obtenido
 * únicamente después de aceptar el paréntesis de cierre.
 *
 * @param[out] valor Valor del primario si el reconocimiento y la semántica son válidos.
 * @return `true` si se aceptó un primario; `false` ante un fallo.
 */
static bool ParsePrimary(int64_t *valor) {
    if (!Disponible()) {
        return false;
    }
    SourcePosition posicion = Posicion();
    switch (sesion.actual.tipo) {
        case TOKEN_NATURAL: {
            int64_t natural = sesion.actual.valor;
            bool fueraDeRango = sesion.actual.fuera_de_rango;
            ConsumirToken();
            ResultadoSem resultado = SemNatural(natural, fueraDeRango);
            return UsarResultado(resultado, posicion, valor);
        }
        case TOKEN_IDENTIFIER: {
            ResultadoSem resultado = SemVariable(sesion.actual.lexema);
            ConsumirToken();
            return UsarResultado(resultado, posicion, valor);
        }
        case TOKEN_LPAREN:
            ConsumirToken();
            if (!ParseExpression(valor)) {
                return false;
            }
            return Esperar(TOKEN_RPAREN);
        case TOKEN_DITTO: {
            ConsumirToken();
            if (!Esperar(TOKEN_LPAREN)) {
                return false;
            }
            if (!ParseExpression(valor)) {
                return false;
            }
            if (!Esperar(TOKEN_RPAREN)) {
                return false;
            }
            ResultadoSem resultado = SemDuplicar(*valor);
            return UsarResultado(resultado, posicion, valor);
        }
        default:
            return Fallar(E_SYNTAX, posicion, "Se esperaba un literal, variable, minun, '(' o ditto.");
    }
}

/**
 * @brief Inicializa los módulos y traduce el resultado de la sesión a un estado de proceso.
 * @param trace Habilita el registro de cada token solicitado al scanner.
 * @return 0 sin rechazos, 1 con errores recuperables o 2 ante un error fatal.
 * @see Parser.h
 */
int ParserRun(bool trace) {
    /* memset pone a cero el estado completo; &sesion indica dónde comienza. */
    memset(&sesion, 0, sizeof sesion);
    sesion.trace = trace;
    ScannerReset();
    SemReset();
    if (!ParseTranslationUnit()) {
        return 2;
    }
    if (sesion.fatal) {
        return 2;
    }
    if (sesion.huboError) {
        return 1;
    }
    return 0;
}
