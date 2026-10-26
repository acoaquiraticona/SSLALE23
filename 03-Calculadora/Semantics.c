/**
 * @file Semantics.c
 * @brief Evaluación aritmética y tabla de símbolos de capacidad fija.
 *
 * Las funciones no conocen tokens ni posiciones de entrada: reciben valores
 * ya reconocidos y devuelven ResultadoSem. La separación permite probar las
 * reglas semánticas sin ejecutar el parser.
 */
#include "Semantics.h"

#include <stdio.h>
#include <string.h>

/** @brief Una celda reutilizable de la tabla de variables de la sesión. */
typedef struct {
    bool ocupada;                       /**< Determina si el nombre está vigente. */
    char nombre[SEM_MAX_NOMBRE + 1];     /**< Identificador y su terminador nulo. */
    int64_t valor;                      /**< Último valor asignado con éxito. */
} Variable;

/**
 * @brief Almacenamiento privado, sin memoria dinámica, de hasta 32 variables.
 * Las búsquedas recorren el arreglo; una celda libre puede conservar bytes
 * antiguos, pero estos no se consultan mientras @c ocupada sea false.
 */
static Variable tabla[SEM_MAX_VARIABLES];

/**
 * @brief Construye un resultado exitoso con el resto de los campos en cero.
 * @param valor Entero producido o recuperado por la operación.
 * @return Resultado con @c ok activo y sin diagnóstico de fallo.
 */
static ResultadoSem Exito(int64_t valor){
    ResultadoSem resultado = {0};

    resultado.ok = true;
    resultado.valor = valor;
    return resultado;
}

/**
 * @brief Unifica la representación de un fallo semántico.
 * @param codigo Código del problema definido por ErrorCode.
 * @param detalle Explicación del rechazo terminada en nulo.
 * @return Resultado fallido con el diagnóstico común del problema.
 *
 * La posición queda en {0, 0}; corresponde al parser asociar el error con la
 * fuente. El mensaje se copia en el diagnóstico del resultado.
 */
static ResultadoSem Fallo(ErrorCode codigo, const char *detalle) {
    ResultadoSem resultado = {0};
    resultado.ok = false;
    resultado.diagnostico = MakeDiagnostic(codigo, (SourcePosition){0, 0}, detalle);
    return resultado;
}

/**
 * @brief Incluye el identificador ausente en un diagnóstico E_UNDEFINED.
 * @param nombre Variable que no se encontró en una entrada ocupada.
 * @return Fallo compartido por la consulta y la liberación de variables.
 */
static ResultadoSem NoExiste(const char *nombre) {
    char detalle[96];
    snprintf(detalle, sizeof detalle, "La variable %s no existe.", nombre);
    return Fallo(E_UNDEFINED, detalle);
}

/**
 * @brief Busca por nombre únicamente entre las entradas ocupadas.
 * @param nombre Identificador comparado literalmente, con distinción de mayúsculas.
 * @return Puntero a la celda interna o NULL si no existe.
 */
static Variable *Buscar(const char *nombre){
    for (int lugar = 0; lugar < SEM_MAX_VARIABLES; lugar++)
        if (tabla[lugar].ocupada && strcmp(tabla[lugar].nombre, nombre) == 0)
            return &tabla[lugar];
    return NULL;
}

/**
 * @brief Localiza una celda disponible sin modificarla ni reservar memoria.
 * @return Primera entrada desocupada o NULL cuando se alcanzó la capacidad.
 */
static Variable *LugarLibre(void){
    for (int lugar = 0; lugar < SEM_MAX_VARIABLES; lugar++)
        if (!tabla[lugar].ocupada)
            return &tabla[lugar];
    return NULL;
}

/** @brief Resuelve el identificador en la tabla o construye el error de ausencia. */
ResultadoSem SemVariable(const char *nombre){
    const Variable *variable = Buscar(nombre);

    if (variable == NULL)
        return NoExiste(nombre);
    return Exito(variable->valor);
}

/** @brief Guarda un valor validado, reutilizando una variable o una celda libre. */
ResultadoSem SemAsignar(const char *nombre, int64_t valor){
    Variable *variable = Buscar(nombre);

    /** Un nombre nuevo necesita capacidad; el fallo ocurre antes de cambiar
     * la tabla. Para una reasignación se conserva la misma celda. */
    if (variable == NULL) {
        variable = LugarLibre();
        if (variable == NULL)
            return Fallo(E_LIMIT, "La tabla ya tiene 32 variables.");
        variable->ocupada = true;
        snprintf(variable->nombre, sizeof variable->nombre, "%s", nombre);
    }
    variable->valor = valor;
    return Exito(valor);
}

/** @brief Marca como reutilizable la celda de una variable existente. */
ResultadoSem SemLiberar(const char *nombre){
    Variable *variable = Buscar(nombre);

    if (variable == NULL)
        return NoExiste(nombre);
    variable->ocupada = false;
    return Exito(0);
}

/**
 * @brief Rechaza un resultado intermedio que excede el dominio del lenguaje.
 * @param valor Resultado aritmético calculado en int64_t.
 * @return Éxito con el valor o E_RANGE; no intenta corregir ni saturar el resultado.
 *
 * Los operandos válidos están acotados a seis dígitos, de modo que incluso
 * su producto cabe en int64_t. El control posterior corresponde al dominio
 * de la calculadora, más pequeño que el rango del tipo de C.
 */
static ResultadoSem ControlarRango(int64_t valor){
    if (valor < SEM_VALOR_MINIMO || valor > SEM_VALOR_MAXIMO)
        return Fallo(E_RANGE, "El resultado esta fuera de [-999999, 999999].");
    return Exito(valor);
}

/** @brief Comprueba tanto la marca del scanner como el rango natural del valor. */
ResultadoSem SemNatural(int64_t valor, bool fuera_de_rango) {
    if (fuera_de_rango || valor < 0 || valor > SEM_VALOR_MAXIMO)
        return Fallo(E_RANGE, "El literal esta fuera de [0, 999999].");
    return Exito(valor);
}

/** @brief Calcula la suma y valida inmediatamente su resultado. */
ResultadoSem SemSumar(int64_t primerSumando, int64_t segundoSumando){
    return ControlarRango(primerSumando + segundoSumando);
}

/** @brief Calcula la diferencia manteniendo el orden de los operandos. **/
ResultadoSem SemRestar(int64_t minuendo, int64_t sustraendo){
    return ControlarRango(minuendo - sustraendo);
}

/** @brief Calcula el producto en int64_t y comprueba el dominio del lenguaje. */
ResultadoSem SemMultiplicar(int64_t multiplicando, int64_t multiplicador){
    return ControlarRango(multiplicando * multiplicador);
}

/** @brief Invierte el signo y pasa el resultado por el control común de rango. */
ResultadoSem SemNegar(int64_t operando){
    return ControlarRango(-operando);
}

/** @brief Multiplica por dos y rechaza el resultado si sale del dominio. */
ResultadoSem SemDuplicar(int64_t operando){
    return ControlarRango(2 * operando);
}

/** @brief El índice decimal selecciona su nombre Pokémon. */
static const char *const digitos[10] = {
    "mew", "bulbasaur", "ivysaur", "venusaur", "charmander",
    "charmeleon", "charizard", "squirtle", "wartortle", "blastoise"
};

/** @brief Convierte la magnitud por divisiones sucesivas y luego emite sus nombres. */
void SemAPokemon(int64_t valor, char *destino, size_t tamanio){
    int digitosAlReves[6];   /**< Hasta seis dígitos para un valor válido. */
    int cantidad = 0;
    int64_t magnitud = (valor < 0) ? -valor : valor;
    size_t escritos;   /**< Longitud que snprintf habría producido sin recortar. */

    /** La división entera extrae unidades, decenas y siguientes posiciones.
     * Se guardan al revés para recorrerlas luego desde la más significativa.
     * El do-while garantiza un dígito también para el valor cero. */
    do {
        digitosAlReves[cantidad++] = (int)(magnitud % 10);
        magnitud /= 10;
    } while (magnitud > 0 && cantidad < 6);

    /** La representación usa la magnitud y expresa el signo con un operador.
     * snprintf permite contabilizar el espacio requerido aunque recorte. */
    escritos = (size_t)snprintf(destino, tamanio, "%s[",
                             valor < 0 ? "minun " : "");

    /** Se recupera el orden original, separando los nombres con espacios.
     * Solo se avanza dentro del destino si todavía queda capacidad. */
    while (cantidad > 0 && escritos < tamanio) {
        cantidad--;
        escritos += (size_t)snprintf(destino + escritos, tamanio - escritos, "%s%s",
                                  digitos[digitosAlReves[cantidad]],
                                  cantidad > 0 ? " " : "");
    }

    /** El corchete de cierre se agrega únicamente si aún hay espacio. */
    if (escritos < tamanio)
        snprintf(destino + escritos, tamanio - escritos, "]");
}

/** @brief Reinicia todas las celdas, incluidos sus indicadores de ocupación. */
void SemReset(void) {
    memset(tabla, 0, sizeof tabla);
}
