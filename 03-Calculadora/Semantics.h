#ifndef SEMANTICS_H
#define SEMANTICS_H

/*
 * Declaraciones de funciones y estructuras para la semantica de la calculadora, asi devuelve los resultados al parse y sigue la especificacin de calc.md
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SEM_VALOR_MAXIMO  999999
#define SEM_VALOR_MINIMO  (-999999)
#define SEM_MAX_VARIABLES 32
#define SEM_MAX_NOMBRE    31

typedef struct {
    bool ok;
    int64_t valor;
    const char *codigo;
    char detalle[96];
}ResultadoSem;

//valor de un literal, como lo especificamos falla si supera 999999 E_RANGE
ResultadoSem SemNatural(int64_t valor, bool fuera_de_rango);
//valor guardado de una variable, falla si no existe E_UNDEFINED
ResultadoSem SemVariable(const char *nombre);

//Operaciones
ResultadoSem SemSumar(int64_t primerSumando, int64_t segundoSumando);  //Semantica para plusle
ResultadoSem SemRestar(int64_t minuendo, int64_t sustraendo);    // minun 
ResultadoSem SemMultiplicar(int64_t multiplicando, int64_t multiplicador); // tandemaus 
ResultadoSem SemDividir(int64_t dividendo, int64_t divisor);     //falinks
ResultadoSem SemNegar(int64_t operando);         // minun unario
ResultadoSem SemDuplicar(int64_t operando);     // ditto

//Tabla de simbolos
ResultadoSem SemAsignar(const char *nombre, int64_t valor); //es como la asignacion crear o remplazar un valor para una variable

ResultadoSem SemLiberar(const char *nombre);

void SemAPokemon(int64_t valor, char *destino, size_t tamanio);




#endif 