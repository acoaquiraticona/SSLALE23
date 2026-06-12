#include "Strings.h"

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
           : (*s1 == *s2 && AreEqual(s1 + 1, s2 + 1)); // Compruebo la condición matemática y avanzo a la siguiente posición de ambas cadenas
    // Nota: sentencia de arriba cumple el cortocircuito que describimos en la función por el &&
}

bool AreDecimalDigits(const char *s){
    return IsEmpty(s) || // Esta vacía?
           ((*s >= '0' && *s <= '9') && // h pertenece al rango de dígitos decimales? y...
            AreDecimalDigits(s + 1)); // ...avanza a la siguiente posición de la cadena para seguir evaluando
}

bool Contains(const char *s, char c)
{
    return !IsEmpty(s) &&   // Si la cadena no está vacía, entonces...
           (*s == c || Contains(s + 1, c)); // ...si el primer carácter es igual a c, retorna true; de lo contrario, avanza a la siguiente posición de la cadena para seguir buscando
    // Nota: sentencia de arriba cumple el cortocircuito que describimos en la función por el operador ||
}

int ToInteger (const char *s){
    int resultado = 0;
    for(int i = 0; s[i] != '\0'; i++){
        resultado = resultado * 10 + (s[i] - '0');
    }
    return resultado;
}

bool IsPalindrome (const char *s){
    int length = (int)GetLength(s);
    int start = 0;
    int end = length - 1;

  if (IsEmpty(s)) {
        return true; 
    }
    while (start < end) {
        if (s[start] != s[end]) {
            return false; 
        }
        start++;
        end--;
    }
    return true;
}
char *Concatenate (const char *s1, const char *s2){
    size_t len1 = GetLength(s1);
    size_t len2 = GetLength(s2);
    char *result = malloc(len1 + len2 + 1);
    
    for(size_t i = 0; i < len1; i++){
        result[i] = s1[i];
    }
    for(size_t j = 0; j < len2; j++){
        result[len1 + j] = s2[j];
    }
    result[len1 + len2] = '\0';
    return result;
}

char *Power (const char *s, unsigned n){
    size_t len = GetLength(s);
    char *result = malloc(len * n + 1);
    
    for(unsigned i = 0; i < n; i++){
        for(size_t j = 0; j < len; j++){
            result[i * len + j] = s[j];
        }
    }
    result[len * n] = '\0';
    return result;
}

char *Reverse (const char *s){
    size_t len = GetLength(s);
    char *result = malloc(len + 1);
    
    for(size_t i = 0; i < len; i++){
        result[i] = s[len - 1 - i];
    }
    result[len] = '\0';
    return result;
}
