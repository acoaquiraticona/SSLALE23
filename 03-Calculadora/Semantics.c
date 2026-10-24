#include "Semantics.h"
 
#include <stdio.h>
#include <string.h>
 
typedef struct {
    bool ocupada;
    char nombre[SEM_MAX_NOMBRE + 1];  // le sumamos uno por el /0
    int64_t valor;
} Variable;
 
static Variable tabla[SEM_MAX_VARIABLES];
 
 
// armamos el resultado correcto
static ResultadoSem Exito(int64_t valor){
    ResultadoSem resultado = {0};
 
    resultado.ok = true;
    resultado.valor = valor;
    return resultado;
}
 
// Aca armamos el resultado fallido
static ResultadoSem Fallo(const char *codigo, const char *detalle){
    ResultadoSem resultado = {0};
 
    resultado.ok = false;
    resultado.codigo = codigo;
    snprintf(resultado.detalle, sizeof resultado.detalle, "%s", detalle);
    return resultado;
}
 
// Fallo con E_undefined para la variable
static ResultadoSem NoExiste(const char *nombre){
    ResultadoSem resultado = Fallo("E_UNDEFINED", "");
 
    snprintf(resultado.detalle, sizeof resultado.detalle, "La variable %s no existe.", nombre);
    return resultado;
}
 
//Buscamos la el nombre de la variable, si la encuentra nos da el puntero sino NULL
static Variable *Buscar(const char *nombre){
    for (int lugar = 0; lugar < SEM_MAX_VARIABLES; lugar++)
        if (tabla[lugar].ocupada && strcmp(tabla[lugar].nombre, nombre) == 0)
            return &tabla[lugar];
    return NULL;
}
 
//evuelve el primer lugar libre, o NULL si la tabla esta llena. */
static Variable *LugarLibre(void){
    for (int lugar = 0; lugar < SEM_MAX_VARIABLES; lugar++)
        if (!tabla[lugar].ocupada)
            return &tabla[lugar];
    return NULL;
}

//6.2 Identifcadores
//Consultamos el valor en la tabla de simbolos
ResultadoSem SemVariable(const char *nombre){
    const Variable *variable = Buscar(nombre);
 
    if (variable == NULL)
        return NoExiste(nombre);
    return Exito(variable->valor);
}

//6.3 captura el parser la llama despues de leer el ';', asi si la sentencia falla la tabla no cambia
ResultadoSem SemAsignar(const char *nombre, int64_t valor){
    Variable *variable = Buscar(nombre);
 
    if (variable == NULL) {   //nombre nuevo, hay que crearlo 
        variable = LugarLibre();
        if (variable == NULL)
            return Fallo("E_LIMIT", "La tabla ya tiene 32 variables.");
        variable->ocupada = true;
        snprintf(variable->nombre, sizeof variable->nombre, "%s", nombre);
    }
    variable->valor = valor;  //nuevo o existente entonces, guardar el valor 
    return Exito(valor);
}

//6.4 liberacio
ResultadoSem SemLiberar(const char *nombre){
    Variable *variable = Buscar(nombre);
 
    if (variable == NULL)
        return NoExiste(nombre);
    variable->ocupada = false;
    return Exito(0);
}

//6.1 Un literal o resultado intermedio fuera del dominio produce E_RANGE
static ResultadoSem ControlarRango(int64_t valor){
    if (valor < SEM_VALOR_MINIMO || valor > SEM_VALOR_MAXIMO)
        return Fallo("E_RANGE", "El resultado esta fuera de [-999999, 999999].");
    return Exito(valor);
}

//6.2 fila natural
ResultadoSem SemNatural(int64_t valor, bool fuera_de_rango){
    if (fuera_de_rango)
        return Fallo("E_RANGE", "El literal supera 999999.");
    return Exito(valor);
}

//6.2 primerSumando plusle segundoSumando
ResultadoSem SemSumar(int64_t primerSumando, int64_t segundoSumando){
    return ControlarRango(primerSumando + segundoSumando);
}

// 6.2 minuendo minun sustraendo 
ResultadoSem SemRestar(int64_t minuendo, int64_t sustraendo){
    return ControlarRango(minuendo - sustraendo);
}

// 6.2  multiplicando tandemaus multiplicador 
ResultadoSem SemMultiplicar(int64_t multiplicando, int64_t multiplicador){
    return ControlarRango(multiplicando * multiplicador);
}

//6.2 dividendo falinks divisor
ResultadoSem SemDividir(int64_t dividendo, int64_t divisor){
    if (divisor == 0)
        return Fallo("E_DIV_ZERO", "Division por cero.");
    return ControlarRango(dividendo / divisor);
}

//6.2 minum operando
ResultadoSem SemNegar(int64_t operando){
    return ControlarRango(-operando);
}

// 6.2 ditto(operando) 
ResultadoSem SemDuplicar(int64_t operando){
    return ControlarRango(2 * operando);
}

//tabla seccion 2.1, aca ponemos el indice como digito
static const char *const digitos[10] = {
    "mew",        
    "bulbasaur",  
    "ivysaur",
    "venusaur",
    "squirtle",
    "wartortle",
    "blastoise",
    "charmander",
    "charmeleon",
    "charizard"
};

//este metodo que usamos aca es similar al de itoa en el K&R 3.6
void SemAPokemon(int64_t valor, char *destino, size_t tamanio){
    int digitosAlReves[6];   //999999 tiene como maximo 6 digitos
    int cantidad = 0;
    int64_t magnitud = (valor < 0) ? -valor : valor;
    size_t escritos;   //caracteres ya escritos en destino
 
    // separamos los digitos del ultimo al primero
    do {
        digitosAlReves[cantidad++] = (int)(magnitud % 10);
        magnitud /= 10;
    } while (magnitud > 0 && cantidad < 6);
 
    // signo y corchete de apertura. 
    escritos = (size_t)snprintf(destino, tamanio, "%s[",
                             valor < 0 ? "minun " : "");
 
    //los nombres en el orden correcto separados por un espacio
    while (cantidad > 0 && escritos < tamanio) {
        cantidad--;
        escritos += (size_t)snprintf(destino + escritos, tamanio - escritos, "%s%s",
                                  digitos[digitosAlReves[cantidad]],
                                  cantidad > 0 ? " " : "");
    }
 
    //corchete de cierre
    if (escritos < tamanio)
        snprintf(destino + escritos, tamanio - escritos, "]");
}


