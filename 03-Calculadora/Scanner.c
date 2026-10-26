/**
 * @file Scanner.c
 * @brief Reconocimiento manual de tokens con un byte de anticipación.
 *
 * La lectura se divide en tres niveles: mirar o consumir bytes manteniendo la
 * posición, reconocer palabras y literales completos, y devolver al parser un
 * Token con sus atributos. El texto guardado tiene capacidad fija, pero el
 * scanner sigue consumiendo la unidad completa aunque ya no quepa su lexema.
 * Así, un recorte de diagnóstico no altera dónde comienza el token siguiente.
 */
#include "Scanner.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

#define VALOR_MAXIMO INT64_C(999999) /**< Mayor natural representable en el lenguaje. */
#define LONGITUD_MAXIMA_IDENTIFICADOR 31 /**< Límite del nombre de una variable. */
#define CAPACIDAD_LEXEMA 128 /**< Capacidad de la muestra textual, incluido su NUL. */

/**
 * @name Estado de lectura de la sesión
 * El carácter anticipado ya se obtuvo de stdin, pero aún no se consumió
 * lógicamente. Por eso mirar no cambia la posición y consumir sí la actualiza.
 * El valor -2 indica que hace falta leer; EOF se conserva sin volver a pedir
 * bytes. Las columnas cuentan bytes, y un par CRLF cuenta como un solo salto.
 * @{
 */
static bool limitePosicion;
/* Se usa int para distinguir cualquier byte de EOF y de -2 (sin anticipación). */
static int caracterAnticipado = -2;
static int lineaActual = 1;
static int columnaActual = 1;
static int anteriorFueCR = 0;
static int finDeArchivoAlcanzado = 0;
static int errorLecturaInformado = 0;
/** @} */

/**
 * @brief Consulta el próximo byte sin avanzar la posición lógica.
 * @return Byte como int o EOF; conserva el resultado para consultas repetidas.
 * @details getchar sólo se invoca cuando no hay anticipación disponible y no
 * se alcanzó EOF. El control de ferror se centraliza luego en GetNextToken.
 */
static int MirarCaracter(void) {
    if (caracterAnticipado == -2) {
        if (finDeArchivoAlcanzado) {
            caracterAnticipado = EOF;
        } else {
            caracterAnticipado = getchar();
            if (caracterAnticipado == EOF) {
                finDeArchivoAlcanzado = 1;
            }
        }
    }
    return caracterAnticipado;
}

/**
 * @brief Consume la anticipación y actualiza la ubicación del próximo byte.
 * @return Byte consumido o EOF si no queda entrada.
 * @details CR inicia una línea; LF sólo la incrementa si no sigue a CR.
 * Los demás bytes avanzan una columna, incluido cada byte de un texto UTF-8.
 * Antes de incrementar se comprueba INT_MAX: al agotarse la representación se
 * activa limitePosicion para producir E_RESOURCE, sin desbordar un entero C.
 */
static int ConsumirCaracter(void) {
    int caracter = MirarCaracter();
    caracterAnticipado = -2;
    if (caracter == '\r') {
        if (lineaActual == INT_MAX) {
            limitePosicion = true;
        } else {
            lineaActual++;
        }
        columnaActual = 1;
        anteriorFueCR = 1;
    } else if (caracter == '\n') {
        if (!anteriorFueCR) {
            if (lineaActual == INT_MAX) {
                limitePosicion = true;
            } else {
                lineaActual++;
            }
        }
        columnaActual = 1;
        anteriorFueCR = 0;
    } else if (caracter != EOF) {
        if (columnaActual == INT_MAX) {
            limitePosicion = true;
        } else {
            columnaActual++;
        }
        anteriorFueCR = 0;
    }
    return caracter;
}

/**
 * @brief Construye un token con categoría y posición, dejando los atributos en cero.
 * @param tipo Categoría reconocida o señal especial de fin/error.
 * @param linea Línea inicial del token, desde uno.
 * @param columna Columna inicial en bytes, desde uno.
 * @return Token listo para completar con lexema, valor o diagnóstico.
 */
static Token CrearToken(TipoToken tipo, int linea, int columna) {
    Token token = {0};
    token.tipo = tipo;
    token.linea = linea;
    token.columna = columna;
    return token;
}

/**
 * @brief Empaqueta un error léxico o fatal en el contrato común de diagnósticos.
 * @param codigo Categoría del problema definida por errors.h.
 * @param detalle Explicación del problema sin incluir aún el texto de entrada.
 * @param lexema Texto implicado, posiblemente abreviado y marcado como recortado.
 * @param linea Línea inicial de la unidad que falló.
 * @param columna Columna inicial de la unidad que falló.
 * @return TOKEN_ERROR con diagnóstico estructurado y muestra del lexema.
 * @details Adjunta el lexema al detalle para que el mensaje identifique la
 * entrada problemática. El diagnóstico y el lexema tienen capacidad acotada
 * y terminación NUL.
 */
static Token CrearError(ErrorCode codigo, const char *detalle,
                       const char *lexema, int linea, int columna) {
    Token token = CrearToken(TOKEN_ERROR, linea, columna);
    char mensaje[512];
    snprintf(mensaje, sizeof mensaje, "%s Entrada: \"%s\"", detalle, lexema);
    SourcePosition posicion = {linea, columna};
    token.diagnostico = MakeDiagnostic(codigo, posicion, mensaje);
    snprintf(token.lexema, sizeof token.lexema, "%s", lexema);
    token.lexema_recortado = strstr(lexema, "(recortado)") != NULL;
    return token;
}

/**
 * @brief Agrega un byte sólo si también queda espacio para la terminación NUL.
 * @param[in,out] cadena Búfer que contiene una cadena terminada en NUL.
 * @param capacidad Capacidad total del búfer, incluido el terminador.
 * @param[in,out] longitud Cantidad de bytes almacenados, sin contar el NUL.
 * @param caracter Byte que se desea anexar.
 */
static void AgregarCaracter(char *cadena, size_t capacidad,
                            size_t *longitud, int caracter) {
    /* *longitud lee o modifica la variable del llamador, no una copia local. */
    size_t posicion = *longitud;
    if (posicion + 1 < capacidad) {
        cadena[posicion] = (char)caracter;
        posicion++;
        *longitud = posicion;
        cadena[posicion] = '\0';
    }
}

/**
 * @brief Reconoce letras ASCII sin depender de la configuración regional.
 * @param c Byte a clasificar o EOF.
 * @return Valor distinto de cero para A-Z o a-z; cero en otro caso.
 */
static int EsLetra(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/**
 * @brief Reconoce cifras arábigas para rechazarlas como una unidad completa.
 * @param c Byte a clasificar o EOF.
 * @return Valor distinto de cero para 0-9; cero en otro caso.
 */
static int EsDigito(int c) {
    return c >= '0' && c <= '9';
}

/**
 * @brief Comprueba el primer byte permitido de un nombre: letra ASCII o guion bajo.
 * @param c Byte a clasificar o EOF.
 * @return Valor distinto de cero si puede iniciar un identificador.
 */
static int EsInicioIdentificador(int c) {
    return EsLetra(c) || c == '_';
}

/**
 * @brief Amplía la regla de inicio con los dígitos permitidos después del primer byte.
 * @param c Byte a clasificar o EOF.
 * @return Valor distinto de cero si puede continuar un identificador.
 */
static int EsCaracterIdentificador(int c) {
    return EsInicioIdentificador(c) || EsDigito(c);
}

/**
 * @brief Reconoce los únicos separadores admitidos dentro de un literal Pokémon.
 * @param c Byte a clasificar o EOF.
 * @return Valor distinto de cero para espacio o tabulación; excluye los saltos de línea.
 */
static int EsEspacioHorizontal(int c) {
    return c == ' ' || c == '\t';
}

/**
 * @brief Avanza hasta una unidad significativa fuera de los literales.
 * @details Ignora espacios, tabulaciones y saltos de línea. Desde # descarta
 * todo hasta el salto o EOF, incluidos los puntos y coma del comentario.
 * Deja el primer byte significativo en la anticipación para su reconocimiento.
 */
static void OmitirEspaciosYComentarios(void) {
    while (true) {
        int caracter = MirarCaracter();
        if (caracter == ' ' || caracter == '\t' || caracter == '\r' || caracter == '\n') {
            ConsumirCaracter();
        } else if (caracter == '#') {
            caracter = MirarCaracter();
            while (caracter != EOF && caracter != '\r' && caracter != '\n') {
                ConsumirCaracter();
                caracter = MirarCaracter();
            }
        } else {
            return;
        }
    }
}

/**
 * @brief Distingue los seis nombres reservados de los identificadores comunes.
 * @param palabra Palabra completa terminada en NUL.
 * @return Categoría de operador/comando o TOKEN_IDENTIFIER si no coincide.
 * @details La comparación es exacta y distingue mayúsculas. Un nombre como
 * plusle_total sigue siendo un identificador.
 */
static TipoToken PalabraReservada(const char *palabra) {
    static const struct {
        const char *nombre;
        TipoToken tipo;
    } palabras[] = {
        {"plusle", TOKEN_PLUSLE},
        {"minun", TOKEN_MINUN},
        {"tandemaus", TOKEN_TANDEMAUS},
        {"ditto", TOKEN_DITTO},
        {"captura", TOKEN_CAPTURA},
        {"liberar", TOKEN_LIBERAR}
    };
    /* Tamaño total / tamaño de un elemento = cantidad de elementos del arreglo. */
    size_t cantidadPalabras = sizeof palabras / sizeof palabras[0];
    for (size_t i = 0; i < cantidadPalabras; i++) {
        if (strcmp(palabra, palabras[i].nombre) == 0) {
            return palabras[i].tipo;
        }
    }
    return TOKEN_IDENTIFIER;
}

/**
 * @brief Convierte un nombre de Pokémon en su dígito decimal del lenguaje.
 * @param palabra Nombre completo terminado en NUL y escrito en minúsculas.
 * @return Dígito entre 0 y 9, o -1 si el nombre no pertenece a la tabla.
 * @details El índice de la tabla es el valor: mew representa cero y los nueve
 * nombres restantes representan uno a nueve según la especificación Calc.md.
 */
static int ObtenerDigitoPokemon(const char *palabra) {
    /* Ambos const impiden cambiar los nombres y los punteros guardados en la tabla. */
    static const char *const nombres[] = {
        "mew", "bulbasaur", "ivysaur", "venusaur", "charmander",
        "charmeleon", "charizard", "squirtle", "wartortle", "blastoise"
    };
    for (int i = 0; i < 10; i++) {
        if (strcmp(palabra, nombres[i]) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Conserva una muestra acotada del lexema y registra si se perdió texto.
 * @param[in,out] texto Búfer terminado en NUL.
 * @param capacidad Capacidad total del búfer.
 * @param[in,out] longitud Bytes efectivamente almacenados, sin el terminador.
 * @param[in,out] recortado Marca que permanece activa cuando algún byte no cabe.
 * @param c Byte ya consumido que se intenta almacenar.
 * @details No detiene el reconocimiento al llenar el búfer: los llamadores
 * siguen leyendo hasta el delimitador real y sólo se abrevia la representación.
 */
static void GuardarByte(char *texto, size_t capacidad, size_t *longitud,
                        bool *recortado, int c) {
    if (*longitud + 1 >= capacidad) {
        /* El puntero permite que el llamador conserve la marca de recorte. */
        *recortado = true;
    } else {
        AgregarCaracter(texto, capacidad, longitud, c);
    }
}

/**
 * @brief Hace visible que el texto guardado no contiene el lexema completo.
 * @param[in,out] texto Búfer que GuardarByte llenó hasta su capacidad útil.
 * @param capacidad Capacidad total de texto.
 * @param recortado Indica si debe sustituirse el final por "... (recortado)".
 * @details La marca incluye su NUL y sólo se escribe si cabe entera.
 */
static void MarcarRecorte(char *texto, size_t capacidad, bool recortado) {
    static const char marca[] = "... (recortado)";
    if (recortado && capacidad >= sizeof marca) {
        size_t inicioMarca = capacidad - sizeof marca;
        /* & obtiene la dirección desde la que memcpy reemplaza el final del texto. */
        memcpy(&texto[inicioMarca], marca, sizeof marca);
    }
}

/**
 * @brief Lee una palabra completa y decide si es nombre, comando u operador.
 * @return Token de la palabra o error por longitud o por dígito Pokémon suelto.
 * @details Consume todo el identificador antes de comprobar el límite de 31
 * bytes. Los nombres exactos de los dígitos, como bulbasaur, requieren corchetes
 * y generan E_MISSINGNO; otros nombres con ese prefijo sí pueden ser variables.
 * La primera unidad que ya no pertenece a la palabra queda sin consumir.
 */
static Token LeerPalabra(void) {
    char palabra[CAPACIDAD_LEXEMA] = "";
    size_t longitud = 0;
    bool recortado = false;
    int linea = lineaActual;
    int columna = columnaActual;
    while (EsCaracterIdentificador(MirarCaracter())) {
        int caracter = ConsumirCaracter();
        /* Se pasan las direcciones para actualizar longitud y recortado. */
        GuardarByte(palabra, sizeof palabra, &longitud, &recortado, caracter);
    }
    MarcarRecorte(palabra, sizeof palabra, recortado);
    if (recortado || longitud > LONGITUD_MAXIMA_IDENTIFICADOR) {
        return CrearError(E_LIMIT, "El identificador supera los 31 caracteres.", palabra, linea, columna);
    }
    TipoToken tipo = PalabraReservada(palabra);
    if (tipo == TOKEN_IDENTIFIER && ObtenerDigitoPokemon(palabra) >= 0) {
        return CrearError(E_MISSINGNO, "Un digito Pokemon requiere corchetes.", palabra, linea, columna);
    }
    Token token = CrearToken(tipo, linea, columna);
    snprintf(token.lexema, sizeof token.lexema, "%s", palabra);
    return token;
}

/**
 * @brief Reconoce un natural como lista de dígitos Pokémon entre corchetes.
 * @return TOKEN_NATURAL con valor y marca de rango, o TOKEN_ERROR con diagnóstico
 * E_MISSINGNO por mala forma.
 * @details Un literal necesita al menos un nombre válido, en minúsculas y
 * separado de los demás por espacio o tabulación. La longitud textual no
 * limita el reconocimiento: muchos ceros iniciales pueden formar un valor
 * válido aunque se recorte la copia del lexema.
 *
 * Si una palabra no es un dígito o falta el cierre, sigue hasta ] o hasta un
 * límite de recuperación. Puede consumir ], pero conserva ;, CR, LF y EOF
 * para que las capas siguientes mantengan la frontera entre sentencias.
 */
static Token LeerNumeroPokemon(void) {
    char lexema[CAPACIDAD_LEXEMA] = "";
    size_t longitud = 0;
    bool recortado = false;
    bool invalido = false;
    bool cerrado = false;
    bool tieneDigito = false;
    bool fueraDeRango = false;
    int64_t valor = 0;
    int linea = lineaActual;
    int columna = columnaActual;
    /** El corchete inicial pertenece al lexema; su ubicación identifica todo el literal. */
    int caracter = ConsumirCaracter();
    GuardarByte(lexema, sizeof lexema, &longitud, &recortado, caracter);
    while (true) {
        while (EsEspacioHorizontal(MirarCaracter())) {
            caracter = ConsumirCaracter();
            GuardarByte(lexema, sizeof lexema, &longitud, &recortado, caracter);
        }
        caracter = MirarCaracter();
        if (caracter == ']') {
            caracter = ConsumirCaracter();
            GuardarByte(lexema, sizeof lexema, &longitud, &recortado, caracter);
            cerrado = true;
            break;
        }
        if (caracter == EOF || caracter == ';' || caracter == '\r' || caracter == '\n') {
            break;
        }
        /**
         * Cada tramo entre separadores se valida entero. Los bytes ilegales
         * se consumen aunque no se copien al nombre, evitando aceptar sólo un
         * prefijo válido y asegurando que la lectura siempre progrese.
         */
        char palabra[32] = "";
        size_t longitudPalabra = 0;
        bool palabraInvalida = false;
        caracter = MirarCaracter();
        while (caracter != EOF && caracter != ']' && caracter != ';' &&
               caracter != '\r' && caracter != '\n' && !EsEspacioHorizontal(caracter)) {
            caracter = ConsumirCaracter();
            GuardarByte(lexema, sizeof lexema, &longitud, &recortado, caracter);
            if (caracter < 'a' || caracter > 'z' || longitudPalabra + 1 >= sizeof palabra) {
                palabraInvalida = true;
            } else {
                AgregarCaracter(palabra, sizeof palabra, &longitudPalabra, caracter);
            }
            caracter = MirarCaracter();
        }
        int digito;
        if (palabraInvalida) {
            digito = -1;
        } else {
            digito = ObtenerDigitoPokemon(palabra);
        }
        if (digito < 0) {
            invalido = true;
        } else {
            tieneDigito = true;
            /**
             * La conversión es posicional: valor * 10 + dígito. Se comprueba
             * antes el límite del lenguaje; una vez excedido, se continúa
             * validando la forma sin seguir acumulando ni desbordar int64_t.
             */
            if (!fueraDeRango) {
                if (valor > (VALOR_MAXIMO - digito) / 10) {
                    fueraDeRango = true;
                } else {
                    valor = valor * 10 + digito;
                }
            }
        }
    }
    /** La mala forma es léxica; un natural bien formado fuera de rango se delega al parser. */
    MarcarRecorte(lexema, sizeof lexema, recortado);
    if (!cerrado || invalido || !tieneDigito) {
        return CrearError(E_MISSINGNO, "MissingNo: numero Pokemon mal formado.", lexema, linea, columna);
    }
    Token token = CrearToken(TOKEN_NATURAL, linea, columna);
    token.valor = valor;
    token.fuera_de_rango = fueraDeRango;
    token.lexema_recortado = recortado;
    snprintf(token.lexema, sizeof token.lexema, "%s", lexema);
    return token;
}

/**
 * @brief Agrupa una secuencia de cifras arábigas y la rechaza con E_MISSINGNO.
 * @return Token de error situado en la primera cifra, con muestra del texto.
 * @details Aunque el formato no esté permitido, se consume el número completo
 * para informar una sola unidad inválida y no un error independiente por cifra.
 */
static Token LeerNumeroDecimal(void) {
    char numero[CAPACIDAD_LEXEMA] = "";
    size_t longitud = 0;
    bool recortado = false;
    int linea = lineaActual;
    int columna = columnaActual;
    while (EsDigito(MirarCaracter())) {
        int caracter = ConsumirCaracter();
        GuardarByte(numero, sizeof numero, &longitud, &recortado, caracter);
    }
    MarcarRecorte(numero, sizeof numero, recortado);
    return CrearError(E_MISSINGNO, "Los numeros arabigos no estan permitidos.", numero, linea, columna);
}

/**
 * @brief Coordina el reconocimiento de una unidad léxica y sus fallos de lectura.
 * @return Próximo token significativo, TOKEN_EOF o TOKEN_ERROR con diagnóstico.
 * @details La primera anticipación selecciona un reconocedor de palabra,
 * literal o decimal inválido. Los signos de un byte se resuelven directamente;
 * todo byte desconocido se consume y se informa para asegurar avance.
 * @see GetNextToken en Scanner.h para el contrato público de lectura.
 */
Token GetNextToken(void) {
    OmitirEspaciosYComentarios();
    int caracter = MirarCaracter();
    int linea = lineaActual;
    int columna = columnaActual;
    Token token;
    if (caracter == EOF) {
        token = CrearToken(TOKEN_EOF, linea, columna);
    } else if (EsInicioIdentificador(caracter)) {
        token = LeerPalabra();
    } else if (caracter == '[') {
        token = LeerNumeroPokemon();
    } else if (EsDigito(caracter)) {
        token = LeerNumeroDecimal();
    } else {
        TipoToken tipo = TOKEN_ERROR;
        switch (caracter) {
            case '=':
                tipo = TOKEN_EQUAL;
                break;
            case ';':
                tipo = TOKEN_SEMICOLON;
                break;
            case '(':
                tipo = TOKEN_LPAREN;
                break;
            case ')':
                tipo = TOKEN_RPAREN;
                break;
            default:
                break;
        }
        ConsumirCaracter();
        if (tipo == TOKEN_ERROR) {
            char detalle[64];
            char simbolo[2] = {(char)caracter, '\0'};
            snprintf(detalle, sizeof detalle, "Byte no reconocido (0x%02X).", (unsigned)caracter);
            token = CrearError(E_LEXICAL, detalle, simbolo, linea, columna);
        } else {
            token = CrearToken(tipo, linea, columna);
            token.lexema[0] = (char)caracter;
        }
    }
    /**
     * Los fallos de infraestructura prevalecen sobre el token parcial. El
     * error de lectura se informa una vez; el agotamiento de posiciones queda
     * marcado hasta ScannerReset y exige terminar la sesión desde el parser.
     */
    if (ferror(stdin) && !errorLecturaInformado) {
        errorLecturaInformado = 1;
        return CrearError(E_IO, "Fallo al leer stdin.", "", linea, columna);
    }
    if (limitePosicion) {
        return CrearError(E_RESOURCE, "Se agoto el rango de posiciones de fuente.", "", linea, columna);
    }
    return token;
}

/**
 * @brief Restablece las variables privadas de anticipación, posición y errores.
 * @details No altera stdin: el flujo puede ser reposicionado o reemplazado
 * por el llamador antes de iniciar otra sesión, como ocurre en las pruebas.
 */
void ScannerReset(void) {
    caracterAnticipado = -2;
    lineaActual = 1;
    columnaActual = 1;
    anteriorFueCR = 0;
    finDeArchivoAlcanzado = 0;
    errorLecturaInformado = 0;
    limitePosicion = false;
}

/**
 * @brief Traduce una categoría a la etiqueta usada para mostrar la traza léxica.
 * @param tipo Categoría de token a describir.
 * @return Cadena estática; ERROR si la categoría no forma parte del lenguaje activo.
 */
const char *ScannerTokenName(TipoToken tipo) {
    switch (tipo) {
        case TOKEN_EOF:
            return "EOF";
        case TOKEN_ERROR:
            return "ERROR";
        case TOKEN_NATURAL:
            return "NATURAL";
        case TOKEN_IDENTIFIER:
            return "IDENTIFIER";
        case TOKEN_PLUSLE:
            return "PLUSLE";
        case TOKEN_MINUN:
            return "MINUN";
        case TOKEN_TANDEMAUS:
            return "TANDEMAUS";
        case TOKEN_DITTO:
            return "DITTO";
        case TOKEN_CAPTURA:
            return "CAPTURA";
        case TOKEN_LIBERAR:
            return "LIBERAR";
        case TOKEN_EQUAL:
            return "'='";
        case TOKEN_SEMICOLON:
            return "';'";
        case TOKEN_LPAREN:
            return "'('";
        case TOKEN_RPAREN:
            return "')'";
        default:
            return "ERROR";
    }
}
