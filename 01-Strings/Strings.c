#include "Strings.h"

/* Funciones que consultan cadenas */

bool IsEmpty (const char *s) {
    assert(s != NULL); // Precondición: s no debe ser NULL porque no esta definido en el dominio de la función.
    return *s == '\0';
}

size_t GetLength (const char *s){
    return IsEmpty(s) ? 0 :1 + GetLength(s + 1);
}

bool AreEqual(const char *s1, const char *s2){
    return IsEmpty(s1) && IsEmpty(s2)   // Si ambas cadenas están vacías, son iguales
           ? true
           : IsEmpty(s1) || IsEmpty(s2)
             ? false
             : (*s1 == *s2 && AreEqual(s1 + 1, s2 + 1));// Compruebo la condición matemática y avanzo a la siguiente posición de ambas cadenas
    // Nota: sentencia de arriba cumple el cortocircuito que describimos en la función por el &&
}

bool AreDecimalDigits(const char *s){
    return IsEmpty(s) || // Esta vacía?
           ((*s >= '0' && *s <= '9') && // h pertenece al rango de dígitos decimales? y...
            AreDecimalDigits(s + 1)); // ...avanza a la siguiente posición de la cadena para seguir evaluando
}

bool Contains(const char *s, char c){
    return !IsEmpty(s) &&   // Si la cadena no está vacía, entonces...
           (*s == c || Contains(s + 1, c)); // ...si el primer carácter es igual a c, retorna true; de lo contrario, avanza a la siguiente posición de la cadena para seguir buscando
    // Nota: sentencia de arriba cumple el cortocircuito que describimos en la función por el operador ||
}

bool IsPalindrome(const char *s) {
    size_t len = GetLength(s);

    for (size_t i = 0; i < len / 2; i++)
        if (s[i] != s[len - 1 - i]) return false;
    
    return true;
}

/* Funciones que crean o modifican cadenas */

char *Concatenate(const char *s1, const char *s2) {
    size_t len1 = GetLength(s1); // Dame la longitud de s1
    size_t len2 = GetLength(s2); // Portate bien y dame la longitud de s2
    char *result = malloc(len1 + len2 + 1); // Reservame la memoria para s1+s2+'\0'

    // Me pongo defensivo y hago fail fast si no hay memoria
    assert(result != NULL); // Podría hacer que el caller o invoker maneje el error, pero nah

    for (size_t i = 0; i < len1; i++) // copiamo' la primer cadena
        *(result+i) = *(s1+i);
    for (size_t i = 0; i < len2; i++) // copiamo' la segunda cadena
        *(result+len1 + i) = *(s2+i);
    
    *(result+len1 + len2) = '\0'; // barrita cero

    return result;
}

char *Power(const char *s, unsigned n) {
    assert(s != NULL); // Verificamos la precondición

    // Si n es 0 o la cadena es vacía, el resultado es la cadena vacía.
    if (n == 0 || IsEmpty(s)) {
        char *result = malloc(1);
        assert(result != NULL); // Nos ponemos defensivos, de nuevo, para variar...

        *result = '\0'; // Inicializa la cadena vacía.
        return result;
    }

    // Obtiene la longitud de la cadena base para calcular la memoria necesaria.
    size_t len = GetLength(s);

    // Reserva memoria para almacenar n repeticiones de s más el carácter nulo.
    char *result = malloc(len * n + 1);
    assert(result != NULL); // Nos ponemos defensivos, de nuevo, para variar...

    // Puntero auxiliar que indica la próxima posición de escritura.
    char *dest = result;

    // Copia la cadena s exactamente n veces dentro del buffer resultado.
    for (size_t i = 0; i < n; i++) {
        for (const char *src = s; *src != '\0'; src++) { // src recorre la cadena s desde el inicio hasta el final
            *dest++ = *src;
        }
    }

    // Agrega el '\0' al final de la cadena resultante para marcar su fin.
    *dest = '\0';

    return result;
}

char *Reverse(const char *s) {
    assert(s != NULL); // Precondición: s no debe ser NULL.

    size_t len = GetLength(s);

    char *result = malloc(len + 1);
    assert(result != NULL); // Verificamos tener la memoria suficiente para guardar la cadena invertida

    char *aux = result; // Puntero auxiliar para escribir en la cadena resultante

    // Recorremos s desde el final hacia el inicio.
    for (const char *src = s + len; src > s;) { // src comienza apuntando al carácter nulo al final de s, y se mueve hacia el inicio de s
        src--;
        *aux = *src;
        aux++;
    }

    // Agrega el terminador de cadena.
    *aux = '\0';

    return result;
}
