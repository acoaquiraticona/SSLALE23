#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

/* Funciones de consulta */

/** 
 * @brief Controla si una cadena está vacía, es decir, si el primer
 *        carácter es el carácter nulo.
 *
 * @pre  s != NULL
 *
 * @post El contenido apuntado por s no es modificado.
 * @post Retorna true  si s[0] == '\0' (cadena vacía = epsilon).
 * @post Retorna false si s[0] != '\0' (cadena con al menos un carácter).
 *
 * @param s La cadena a evaluar.
 * @return  true si la cadena está vacía, false en caso contrario.
 */
bool IsEmpty (const char *s);


/** 
 * @brief Obtiene la longitud de una cadena.
 *
 * @pre  s != NULL
 *
 * @post El contenido apuntado por s no es modificado.
 * @post Retorna la cantidad de caracteres de s antes del '\0' terminal.
 * @post Retorna 0 si la cadena está vacía.
 *
 * @param s La cadena a evaluar.
 * @return  La longitud de la cadena.
 */
size_t GetLength(const char *s);    // size_t es un tipo sin signo adecuado para representar longitudes (se ajusta por arquitectura)


/** 
 * @brief Compara dos cadenas para determinar si son iguales.
 *
 * @pre  s1 != NULL
 * @pre  s2 != NULL
 *
 * @post El contenido apuntado por s1 y s2 no es modificado.
 * @post Retorna true si s1 y s2 tienen la misma longitud y sus caracteres
 *       son idénticos en cada posición.
 * @post Retorna false en cualquier otro caso.
 *
 * @param s1 La primera cadena a comparar.
 * @param s2 La segunda cadena a comparar.
 * @return   true si las cadenas son iguales, false en caso contrario.
 */
bool AreEqual(const char *s1, const char *s2);


/** 
 * @brief Controla si una cadena contiene solo dígitos decimales.
 *
 * @pre  s != NULL
 * @pre  IsEmpty(s) == false  (la cadena no debe estar vacía)
 *
 * @post El contenido apuntado por s no es modificado.
 * @post Retorna true  si todos los caracteres de s están entre '0' y '9'.
 * @post Retorna false si al menos un carácter no es un dígito decimal.
 *
 * @param s La cadena a evaluar.
 * @return  true si la cadena contiene solo dígitos decimales, false en caso contrario.
 */
bool AreDecimalDigits(const char *s);


/** 
 * @brief Busca un carácter específico en una cadena.
 *
 * @pre  s != NULL
 *
 * @post El contenido apuntado por s no es modificado.
 * @post Retorna true  si c aparece al menos una vez en s.
 * @post Retorna false si c no se encuentra en s, o si s está vacía.
 *
 * @param s La cadena a evaluar.
 * @param c El carácter a buscar.
 * @return  true si el carácter está presente, false en caso contrario.
 */
bool Contains(const char *s, char c);


/** 
 * @brief Convierte una cadena de dígitos en un número entero positivo.
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
unsigned int ToInteger(const char *s);  // unsigned int es un tipo sin signo adecuado para representar números enteros positivos


/** 
 * @brief Controla si una cadena es un palíndromo.
 *
 * @pre  s != NULL
 *
 * @post El contenido apuntado por s no es modificado.
 * @post Retorna true  si s se lee igual de izquierda a derecha
 *       que de derecha a izquierda (ej: "ana", "reconocer").
 * @post Retorna true  si la cadena está vacía o tiene un solo carácter.
 * @post Retorna false en cualquier otro caso.
 *
 * @param s La cadena a evaluar.
 * @return  true si la cadena es un palíndromo, false en caso contrario.
 */
bool IsPalindrome(const char *s);

/* Funciones que crean o modifican cadenas */

/** 
 * @brief Concatena dos cadenas.
 *
 * @pre  s1 != NULL
 * @pre  s2 != NULL
 *
 * @post El contenido apuntado por s1 y s2 no es modificado.
 * @post Retorna un puntero a una nueva cadena con el contenido de s1
 *       seguido del contenido de s2.
 * @post La cadena retornada fue reservada dinámicamente; el invocador
 *       es responsable de liberar la memoria con free().
 * @post Retorna NULL si no hay memoria suficiente.
 *
 * @param s1 La primera cadena.
 * @param s2 La segunda cadena.
 * @return   Un puntero a la cadena resultante, o NULL si falla la reserva.
 */
char *Concatenate(const char *s1, const char *s2);


/** 
 * @brief Repite una cadena un número específico de veces.
 *
 * @pre  s != NULL
 *
 * @post El contenido apuntado por s no es modificado.
 * @post Retorna un puntero a una nueva cadena formada por n repeticiones de s.
 * @post Retorna un puntero a una cadena vacía si n == 0.
 * @post La cadena retornada fue reservada dinámicamente; el invocador
 *       es responsable de liberar la memoria con free().
 * @post Retorna NULL si no hay memoria suficiente.
 *
 * @param s La cadena a repetir.
 * @param n El número de veces que se repite la cadena.
 * @return  Un puntero a la cadena resultante, o NULL si falla la reserva.
 */
char *Power(const char *s, unsigned n);


/** 
 * @brief Invierte una cadena.
 *
 * @pre  s != NULL
 *
 * @post El contenido apuntado por s no es modificado.
 * @post Retorna un puntero a una nueva cadena con los caracteres de s
 *       en orden inverso (ej: "hola" -> "aloh").
 * @post Retorna un puntero a una cadena vacía si s está vacía.
 * @post La cadena retornada fue reservada dinámicamente; el invocador
 *       es responsable de liberar la memoria con free().
 * @post Retorna NULL si no hay memoria suficiente.
 *
 * @param s La cadena a invertir.
 * @return  Un puntero a la cadena resultante, o NULL si falla la reserva.
 */
char *Reverse(const char *s);