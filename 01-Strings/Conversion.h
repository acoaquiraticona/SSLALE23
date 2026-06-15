#ifndef CONVERSION_H
#define CONVERSION_H

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h> // Agrego assert para forzar la definición de las precondiciones en las funciones.
#include "Strings.h"

/** 
 * @brief Convierte una cadena de dígitos en un número entero.
 *
 * @pre  s != NULL
 * @pre  IsEmpty(s) == false       (la cadena no debe estar vacía)
 * @pre  AreDecimalDigits(s) == true  (la cadena debe contener solo dígitos)
 *
 * @post El contenido apuntado por s no es modificado.
 * @post Retorna el valor entero representado por s (ej: "142" -> 142).
 *
 * @param s La cadena a convertir.
 * @return  El número entero representado por la cadena.
 */
int ToInteger(const char *s);

#endif
