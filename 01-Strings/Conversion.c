#include "Conversion.h"

/*
Respondiendo a las consultas del enunciado:

a - ¿Es correcto que ToInteger sea parte de la biblioteca de Strings? 
b - ¿Debería ser parte de otra biblioteca implementada en, por ejemplo, Conversion.h y Conversion.c 
    y tener su propio programa de prueba ConversionTest.c?

a)  Para nosotros, no es correcto que ToInteger sea parte de nuesta biblioteca de Strings, porque no 
    es una función que consulte o modifique cadenas, sino que convierte una cadena a un número entero. 
    Por lo tanto, consideramos que ToInteger debería ser parte de otra biblioteca, como Conversion.h 
    y Conversion.c, para mantener una separación clara de responsabilidades (SRE - Single Responsibility Principle),
    facilitar la mantenibilidad y extensibilidad del código.

b)  Sí, creemos que ToInteger debería tener su propio programa de prueba, como ConversionTest.c, para
    asegurar su correcto funcionamiento y facilitar la integración con otras partes del sistema.
    Por algo estamos armando estos archivos, no?
*/

int ToInteger(const char *s) {
    assert(AreDecimalDigits(s)); // Precondición: s contiene únicamente dígitos decimales.

    int result = 0;

    for (const char *p = s; *p != '\0'; p++) {  // La hago iterativa porque es más eficiente que la recursiva
        result = result * 10 + (*p - '0');
    }

    return result;
}