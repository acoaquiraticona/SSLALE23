# Análisis Comparativo: Strings en C vs Python

En el siguiente documento, haremos una comparación directa de cómo se gestionan los textos en el lenguaje C y en Python, entre otras cuestiones.

El documento está segmentado para analizar y entender las diferencias en cuanto a la naturaleza del tipo, como se gestiona la memoria, el paso y retorno en funciones, encoding, rendimiento y manejo de errores.

Destacamos que cada sección posee ejemplos de código comentados para ambos lenguajes, ya que, quedarnos únicamente con el concepto téorico no es suficiente como para entender de forma completa aquello que analizamos.

---

## Ficha del documento

| Campo | Detalle |
| :------ | :-------- |
| **Versión** | 1.0 |
| **Fecha** | 31 de mayo de 2026 |

### Participantes

| Rol | Nombre | Legajo |
| :---- | :------- | :----- |
| Autor | Juan Gabriel Laterza Rosa | 163.171-8 |
| Revisor | Eva Olave | 152.952-3 |
| Revisor | Alejandro Coaquira Ticona | 222.229-2 |
| Revisor | Joaquín Benicio Domínguez | 233.839-7 |
| Revisor | Julian Clemente | 222.226-7 |
| Revisor | Thiago Elias Silva | 222.709-5 |
| Revisor | Thiago Gabriel Salles | 234.176-1 |
| Revisor | Victor Fernández | 174.545-1 |

---

## Índice

0. [Definición de Python](#definición-de-python)
   - 0.1 [Características principales de Python](#características-principales-de-python)
1. [Tabla comparativa](#tabla-comparativa)
   - 1.1 [Tipo y naturaleza](#tipo-y-naturaleza)
   - 1.2 [Memoria y ciclo de vida](#memoria-y-ciclo-de-vida)
   - 1.3 [Semántica y ciudadanía en el lenguaje](#semántica-y-ciudadanía-en-el-lenguaje)
   - 1.4 [Paso y retorno en funciones](#paso-y-retorno-en-funciones)
   - 1.5 [Encoding y comparación](#encoding-y-comparación)
   - 1.6 [Operaciones y performance](#operaciones-y-performance)
   - 1.7 [Errores, formateo e I/O](#errores-formateo-e-io)
2. [Riesgos frecuentes y buenas prácticas](#riesgos-frecuentes-y-buenas-prácticas)
3. [Conclusión](#conclusión)
4. [Glosario de términos técnicos](#glosario-de-términos-técnicos)
5. [Referencias generales](#referencias-generales)

---

## Definición de Python

> `Python` es un lenguaje de programación de `alto nivel`, `interpretado` y de `propósito general`, creado por Guido van Rossum en 1991. Su filosofía central es la legibilidad del código y la simplicidad sintáctica, resumida en el famoso `"Zen de Python"`: _"Debería haber una, y preferiblemente solo una, forma obvia de hacerlo"_.

### Características principales de Python

| Característica | Descripción |
| :--- | :--- |
| **Sintaxis clara y legible** | El código se parece casi al pseudocódigo en inglés. Usa indentación obligatoria para definir bloques, lo que fuerza un estilo limpio. |
| **Tipado dinámico y fuerte** | No necesitamos declarar el tipo de una variable; Python lo infiere en tiempo de ejecución. Sin embargo, no hace conversiones implícitas entre tipos incompatibles. |
| **Interpretado** | El código se ejecuta línea a línea a través de un intérprete (como CPython), sin necesidad de compilar antes de correr. |
| **Multiparadigma** | Soporta programación orientada a objetos, funcional e imperativa dentro del mismo lenguaje. |
| **Gestión automática de memoria** | Incluye un recolector de basura (Garbage Collector - GC) que libera memoria automáticamente. |
| **Ecosistema gigantesco** | Tiene una librería estándar muy completa y un repositorio de paquetes (PyPI) con cientos de miles de librerías: NumPy, Pandas, Django, TensorFlow, etc. |

---

## Tabla comparativa

En las siguientes subsecciones presentamos cada aspecto del comportamiento de strings en ambos lenguajes, con tablas comparativas y ejemplos en código comentados.

---

### Tipo y naturaleza

> ¿Qué es un string? ¿Es un tipo del lenguaje, forma parte de su biblioteca estándar, o de ninguno de los dos?

| N° | Pregunta | C | Python |
| :-- | :--------- | :-- | :------- |
| 1 | ¿El tipo es parte del lenguaje en algún nivel? | No existe el tipo `string` como tipo nativo del lenguaje. Los strings son arrays de `char` terminados en `'\0'`. C solo conoce `char`. | Sí. Existe el tipo `str` para representar a los Strings y esta "built-in" en el núcleo del intérprete (implementado en C en CPython). |
| 2 | El tipo, ¿Es parte de la biblioteca? | No; Si bien `<string.h>` provee funciones (`strlen`, `strcpy`…), el tipo `char[]` es del lenguaje, no de la biblioteca. | No. `str` es nativo. El módulo `string` de la stdlib solo aporta constantes y utilidades adicionales opcionales. |
| 3 | ¿Qué alfabeto usa? | Usa ASCII (7-bit) nativo. Luego C11 añadió `wchar_t`, `char16_t`, `char32_t` para Unicode, con soporte dependiente de la plataforma. | Unicode completo. `str` es una secuencia de code points Unicode (hasta U+10FFFF). Internamente usa Latin-1, UCS-2 o UCS-4 según el contenido (PEP 393). |

#### Ejemplo: naturaleza del tipo

```c
/* C: un string es un array de char terminado en '\0' */
#include <stdio.h>
#include <string.h>

int main(void) {
    /* array en stack: 5 elementos → {'h','o','l','a','\0'} */
    char saludo[] = "hola";

    /* puntero a literal: apunta a .rodata (solo lectura, NO modificar) */
    const char *literal = "mundo";

    /* strlen cuenta bytes hasta '\0', sin incluirlo */
    printf("largo de saludo: %zu\n", strlen(saludo)); /* 4 */
    printf("largo de literal: %zu\n", strlen(literal)); /* 5 */

    return 0;
}
```

```python
# Python: str es un tipo nativo del intérprete, no un array de bytes
saludo = "hola"

print(type(saludo))   # <class 'str'>
print(len(saludo))    # 4  — cuenta code points Unicode, no bytes
print(saludo[0])      # "h" — acceso por índice retorna un str de 1 carácter
```

---

### Memoria y ciclo de vida

> ¿Quién gestiona la memoria? ¿Los strings son mutables? ¿Qué pasa cuando ya no los necesitamos?

| N° | Pregunta | C | Python |
| :-- | :--------- | :-- | :------- |
| 4 | ¿Cómo se resuelve la alocación de memoria? | **Manual.** Los arrays se guardan en el stack (`char s[N]`) o en el heap (`malloc`/`free`). Nosotros tenemos que reservar, copiar y liberar memoria. No existe la alocación "automática". | **Automática.** El intérprete se encarga de gestionar con un garbage collector (reference counting + cycle collector). Nadie interviene, es automático. |
| Extra | ¿Cómo se almacenan los string literales en memoria? ¿Son verdaderamente inmutables? | Los literales como (`"hola"`) se ubican en el segmento `.rodata` (Read-Only Data). Intentar modificarlo vía punteros nos da un _Undefined Behavior_. Un array inicializado con literal (`char s[] = "hola"`) se copia al stack y es mutable. | Todos los objetos `str` viven en el heap y son verdaderamente inmutables: la API no expone ninguna vía para modificar su contenido. El intérprete aplica _string interning_ para reutilizar objetos de strings pequeños o con forma de identificador. |

#### Ejemplo: alocación de memoria

```c
/* C: alocación manual en heap con malloc/free */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *copiar(const char *src) {
    size_t n = strlen(src) + 1;   /* +1 porque necesita el '\0' al final */
    char *dst = malloc(n);         /* reserva n bytes en el heap */
    if (dst == NULL) return NULL;  /* malloc puede fallar: SIEMPRE lo tenemos que verificar */
    strcpy(dst, src);              /* copia src a dst, incluyendo '\0' */
    return dst;                    /* el llamador -caller- es responsable de liberar */
}

int main(void) {
    char *s = copiar("hola");      /* acá tenes a tu caller */
    if (s == NULL) {
        fputs("Error: Ups, no tengo memoria, querido querido\n", stderr);
        return 1;
    }
    printf("%s\n", s);  /* "hola" */
    free(s);            /* OBLIGATORIO: tenemos que liberar la memoria del heap */
    s = NULL;           /* buena práctica: evitar dangling pointer */
    return 0;
}
```

```python
# Python: alocación completamente automática, sin intervención del programador
def copiar(s: str) -> str:
    # str es inmutable: retornar la misma referencia es completamente seguro.
    # No hay riesgo de que el caller modifique el objeto original.
    return s

s = copiar("hola")
print(s)  # "hola"
# No hay que liberar memoria: el GC decrementa el reference count
# y libera el objeto cuando ya no quedan referencias activas.
```

#### Ejemplo: literales e inmutabilidad

```c
/* C: diferencia entre un literal y un array de char */
#include <stdio.h>
#include <ctype.h>

int main(void) {
    /* literal → .rodata (solo lectura) */
    const char *literal = "hola";
    /* literal[0] = 'H';  →  Undefined Behavior: NO debemos hacer este chistecito */

    /* array inicializado con literal → copia al stack → es modificable */
    char mutable[] = "hola";
    mutable[0] = (char)toupper((unsigned char)mutable[0]); /* 'h' → 'H' */
    printf("%s\n", mutable); /* "Hola" */

    return 0;
}
```

```python
# Python: str siempre es inmutable, sin excepción
s = "hola"
# s[0] = "H"  →  TypeError: 'str' object does not support item assignment

# Para "modificar" un string hay que crear un nuevo objeto
s = "H" + s[1:]    # nuevo objeto str en memoria; el original "hola" queda sin referencias
print(s)            # "Hola"

# String interning: CPython reutiliza objetos para literales cortos
a = "hola"
b = "hola"
print(a is b)   # True en CPython (implementación interna, no garantizado en todos los contextos)
print(a == b)   # True siempre (compara contenido)
```

---

### Semántica y ciudadanía en el lenguaje

> ¿Un string puede asignarse, pasarse como argumento y compararse directamente? ¿Es un first-class citizen?

| N° | Pregunta | C | Python |
| :-- | :--------- | :-- | :------- |
| 5 | ¿El tipo tiene mutabilidad o es inmutable? | Para los arrays de `char`: son mutables (se puede escribir en cualquier byte). Los String literales en `.rodata`: son de facto inmutables (modificarlos es UB). | Son completamente inmutables. Toda operación sobre un `str` produce un nuevo objeto distinto. |
| 6 | ¿El tipo es un first-class citizen? | **No.** Los arrays no son first-class: porque no se pueden asignar con `=`, comparar con `==`, pasar por valor ni retornar por valor de una función. | **Sí.** `str` es completamente first-class: asignable, pasable, retornable, almacenable en cualquier estructura de datos. |

#### Ejemplo: first-class citizen

```c
/* C: los arrays NO son first-class citizens */
#include <stdio.h>

int main(void) {
    char a[] = "hola";
    /* char b[] = a;  →  ERROR de compilación: no se puede asignar arrays */
    /* if (a == b)    →  compara punteros (direcciones), no contenido */

    /* los punteros sí son asignables, pero no hacen al string first-class */
    char *p = a;              /* p apunta al mismo array: no es una copia */
    printf("%s\n", p);        /* "hola" */
    p[0] = 'H';               /* modifica el array original a través de p */
    printf("%s\n", a);        /* "Hola": el array original cambió */
    return 0;
}
```

```python
# Python: str es completamente first-class
a = "hola"
b = a                    # asignación directa: b referencia el mismo objeto
print(a == b)            # True  — compara contenido
print(type(b))           # <class 'str'>

# Se puede pasar como argumento y retornar desde funciones
def transformar(s: str) -> str:
    return s.upper()     # retorna un nuevo objeto str

print(transformar(a))    # "HOLA"  — a no se modifica

# Se puede guardar en cualquier estructura de datos
lista = ["uno", "dos", "tres"]
dicc  = {"clave": "valor", "otro": a}
tupla = (a, "mundo")
print(dicc["otro"])      # "hola"
```

---

### Paso y retorno en funciones

> ¿Qué pasa cuando pasamos un string a una función o cuando se retorna?

| N° | Pregunta | C | Python |
| :-- | :--------- | :-- | :------- |
| 7 | ¿Cuál es la mecánica al pasar strings como argumentos? | En realidad, se pasa un puntero `char*` por valor (la dirección del primer byte). La función puede modificar el contenido original del array en memoria. | Se pasa una referencia al objeto `str` (pass-by-object-reference). Al ser inmutable, no existe el riesgo de modificación accidental desde la función. |
| 8 | ¿Y cuando son retornados por una función? | Retorna un `char*`. Retornar puntero a memoria local (stack) es un error clásico (_dangling pointer_). Lo correcto es retornar puntero a memoria dinámica (heap) que el caller debe después liberar con `free()`. | Retorna una referencia al objeto. El GC gestiona automáticamente el ciclo de vida. Sin riesgo de dangling pointers ni memory leaks accidentales. |

#### Ejemplo: paso y retorno

```c
/* C: paso de char* y retorno de puntero a heap */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* recibe un char*: o sea, puede modificar el contenido del array original */
void poner_mayuscula(char *s) {
    if (s != NULL && s[0] != '\0') {
        /* castea a unsigned char: requerido por toupper para evitar UB con chars negativos */
        s[0] = (char)toupper((unsigned char)s[0]);
    }
}

/* retorna un puntero a memoria dinámica: el LLAMADOR debe liberar con free() */
char *duplicar(const char *s) {
    size_t n = strlen(s) + 1;
    char *copia = malloc(n);
    if (copia == NULL) return NULL; /* malloc puede fallar */
    strcpy(copia, s);
    return copia;
}

int main(void) {
    char saludo[] = "hola";          /* array en stack: modificable */
    poner_mayuscula(saludo);         /* modificamos el array in-place */
    printf("%s\n", saludo);         /* "Hola" */

    char *d = duplicar("mundo");
    if (d != NULL) {
        printf("%s\n", d);          /* "mundo" */
        free(d);                    /* OBLIGATORIO: liberamos memoria del heap */
        d = NULL;
    }
    return 0;
}
```

```python
# Python: paso por referencia de objeto; str inmutable protege el original
def poner_mayuscula(s: str) -> str:
    # str es inmutable: no podemos escribir s[0] = s[0].upper()
    # → por eso, creamos y retornamos un nuevo objeto str
    if not s:
        return s
    return s[0].upper() + s[1:]   # acá tenes el nuevo objeto; el original queda intacto

def duplicar(s: str) -> str:
    # En Python no tiene sentido "copiar" un str... porque inmutable:
    # múltiples referencias al mismo objeto son completamente seguras.
    # Esta función existe solo para ilustrar que el retorno es una referencia.
    return s   # retorna la misma referencia; el GC maneja el ciclo de vida

saludo = "hola"
nuevo  = poner_mayuscula(saludo)
print(saludo)  # "hola"  — el original NO cambió (inmutable)
print(nuevo)   # "Hola"  — nuevo objeto

d = duplicar("mundo")
print(d)       # "mundo"
# No hay que liberar: el GC lo hace automáticamente cuando d queda fuera de scope
```

---

### Encoding y comparación

> ¿Qué codificación de caracteres usa cada lenguaje? ¿Cómo se comparan dos strings entre sí?

| N° | Pregunta | C | Python |
| :-- | :--------- | :-- | :------- |
| 9 | ¿Qué soporte tiene para ASCII, Unicode y UTF-8? | C usa ASCII nativo. UTF-8 puede almacenarse en `char*` pero el lenguaje no lo interpreta: puede usar `strlen`, `toupper`, etc. pero no son Unicode-aware. Tiene soporte parcial vía `wchar_t` (impl-dependiente). | Python posee Unicode nativo completo. Usa UTF-8 como encoding por defecto para archivos fuente (PEP 3120). `encode()`/`decode()` para conversiones explícitas. Soporta todos los planos Unicode. |
| Extra | ¿Cómo se comparan strings por igualdad: por identidad o por valor? | `==` compara punteros (identidad de dirección). Para comparar contenido se usa `strcmp()`. | `==` compara por valor (contenido). `is` compara por identidad de objeto. El interning puede hacer que `is` sea `True` para algunos strings, pero no está garantizado. |

#### Ejemplo: encoding y Unicode

```c
/* C: strlen cuenta bytes, no caracteres Unicode */
#include <stdio.h>
#include <string.h>
#include <wchar.h>   /* para wchar_t y wcslen */
#include <locale.h>  /* para setlocale */

int main(void) {
    /* UTF-8: 'ñ' se codifica en 2 bytes → strlen devuelve 5, no 4 */
    const char *utf8 = "años";
    printf("strlen (bytes): %zu\n", strlen(utf8));  /* 5, no 4 */

    /* wchar_t: depende de la plataforma (2 bytes en Windows, 4 en Linux/macOS) */
    setlocale(LC_ALL, "");                          /* activar locale del sistema */
    const wchar_t *wide = L"años";
    printf("wcslen (chars): %zu\n", wcslen(wide)); /* 4 en la mayoría de plataformas */

    return 0;
}
```

```python
# Python: len() cuenta code points Unicode, no bytes
s = "años"
print(len(s))           # 4  — 4 caracteres (code points)

# encode() convierte str → bytes con el encoding indicado
b = s.encode("utf-8")
print(b)                # b'a\xc3\xb1os'  — 5 bytes ('ñ' → 2 bytes en UTF-8)
print(len(b))           # 5

# decode() convierte bytes → str
s2 = b.decode("utf-8")
print(s2 == s)          # True

# Manejo explícito de errores de encoding
b_latin = s.encode("latin-1", errors="replace")  # 'ñ' → '?' si no cabe
print(b_latin)          # b'a\xf1os'  — 'ñ' sí existe en Latin-1
```

#### Ejemplo: comparación de strings

```c
/* C: == compara punteros; strcmp() compara contenido */
#include <stdio.h>
#include <string.h>

int main(void) {
    char a[] = "hola";
    char b[] = "hola";  /* array distinto, misma dirección diferente */

    /* MAL: compara direcciones de memoria, no el contenido */
    if (a == b) {
        puts("mismo puntero");  /* nunca se imprime: son arrays distintos */
    }

    /* BIEN: strcmp compara byte a byte */
    if (strcmp(a, b) == 0) {
        puts("mismo contenido");  /* "mismo contenido" */
    }

    /* strcmp también sirve para ordenar: retorna <0, 0 o >0 */
    printf("strcmp(\"abc\", \"abd\"): %d\n", strcmp("abc", "abd")); /* negativo */

    return 0;
}
```

```python
# Python: == compara contenido; is compara identidad de objeto
a = "hola"
b = "hola"

print(a == b)    # True  — compara contenido (siempre correcto)
print(a is b)    # True  — CPython interna literales cortos (no está garantizado)

# Con strings construidos en tiempo de ejecución, is puede ser False
x = "ho" + "la"  # construido dinámicamente
y = "hola"
print(x == y)    # True   — mismo contenido
print(x is y)    # True o False según el intérprete → no confiar en is para comparar
```

---

### Operaciones y performance

> ¿Cuál es el costo de operaciones frecuentes como concatenar en un loop? ¿Cómo podemos evitar ese problema?

| N° | Pregunta | C | Python |
| :-- | :--------- | :-- | :------- |
| Extra | ¿Qué tan costosa es la concatenación repetida y cómo se mitiga? | `strcat` es O(n) por llamada (busca el final cada vez). Loops de concatenación → O(n²). Como mitigación: debemos hacer una gestión manual de los buffers con offset de puntero, `snprintf`, o pre-alocar con `malloc` suficiente. | Concatenación con `+` crea un nuevo objeto por operación → O(n²) en loops. Como mitigación podemos hacer `"".join(lista)` que es O(n) total. También podemos usar `io.StringIO` para escritura incremental. |

#### Ejemplo: concatenación eficiente

```c
/* C: antipatrón O(n²) vs buena práctica con snprintf y offset */
#include <stdio.h>
#include <string.h>

#define N      100
#define BUFSZ  4096

int main(void) {
    /* ANTIPATRÓN: strcat recorre todo el buffer hasta '\0' en cada iteración */
    char mal[BUFSZ] = "";
    for (int i = 0; i < N; i++) {
        strcat(mal, "palabra ");   /* O(n) por llamada → O(n²) total */
    }

    /* BUENA PRÁCTICA: mantener un offset para escribir directamente al final */
    char bien[BUFSZ] = "";
    int offset = 0;
    for (int i = 0; i < N; i++) {
        /* snprintf retorna cuántos chars escribiría (sin contar el '\0'), pero siempre escribe el carácter nulo al final si hay espacio suficiente; avanzamos el offset teniendo esto en cuenta para evitar errores de off-by-one en buffers justos */
        int escrito = snprintf(bien + offset, BUFSZ - offset, "palabra ");
        // snprintf retorna la cantidad de caracteres que habría escrito (sin contar el '\0').
        // Si escrito < 0 hay error; si escrito >= espacio disponible, se previene overflow del buffer.
        if (escrito < 0 || escrito >= BUFSZ - offset) break; /* overflow: salir */
        offset += escrito;
    }

    printf("Chars escritos: %d\n", offset);
    return 0;
}
```

```python
# Python: antipatrón O(n²) vs join() O(n)
import io

palabras = [f"palabra{i}" for i in range(100)]

# ANTIPATRÓN: + crea un nuevo objeto str por iteración → O(n²)
resultado_mal = ""
for p in palabras:
    resultado_mal += p + " "   # cada += copia todo el string acumulado

# BUENA PRÁCTICA: join() calcula el tamaño total primero y alocaa una sola vez → O(n)
resultado_bien = " ".join(palabras)

# Alternativa con io.StringIO: es útil cuando el contenido se construye de forma compleja
buf = io.StringIO()
for p in palabras:
    buf.write(p)
    buf.write(" ")
resultado_io = buf.getvalue()

print(resultado_bien[:40])   # "palabra0 palabra1 palabra2 ..."
```

---

### Errores, formateo e I/O

> ¿Qué mecanismos existen para el manejo de errores, formateo de texto y operaciones de entrada/salida (I/O)?

| N° | Pregunta | C | Python |
| :-- | :--------- | :-- | :------- |
| Extra | ¿Qué mecanismos existen para manejar errores en operaciones sobre strings? | El scope es muy limitado. Las funciones retornan `NULL` o `-1`. Sin excepciones: Los buffer overflows son silenciosos y EXTREMADAMENTE peligrosos. Mitigación parcial: `strncpy`, `snprintf`, `strncat`. | Excepciones extremadamente buenas: `ValueError`, `IndexError`, `UnicodeDecodeError`, `UnicodeEncodeError`, etc. Capturables con `try/except`. Esto hace que las excepciones y errores sean claras y explícitas. |
| Extra | ¿El lenguaje ofrece soporte nativo para interpolación o formateo? | No en el lenguaje. La biblioteca aporta `printf` / `sprintf` / `fprintf` con format strings (`%s`, `%d`, `%f`…). | Sí, existen múltiples mecanismos como: f-strings (`f"{var}"`, Python 3.6+), `.format()`, `%`-formatting (legacy) y `string.Template`. |
| Extra | ¿Qué soporte nativo existe para búsqueda de patrones y expresiones regulares? | No posee soporte nativo en el estándar C básico. POSIX define `<regex.h>`. La mayoría de proyectos recurren a bibliotecas externas como PCRE. | Módulo `re` está incluido en la stdlib (regex Perl-compatible completo). Además, existen métodos nativos como: `str.find()`, `str.startswith()`, `str.replace()`, `str.split()`, etc. |
| Extra | ¿Cómo interactúa el tipo con las operaciones de I/O? | `scanf`/`printf` para consola, `fgets`/`fputs` para archivos. Manejo manual de buffers. Sin protección automática contra overflow (salvo usar `fgets` con tamaño o `%Ns`). | `input()` retorna directamente un `str`. `print()` acepta cualquier objeto. `open()` con `read()`/`write()` maneja encoding automáticamente (UTF-8 por defecto en Python 3). Integración fluida y segura. |

#### Ejemplo: manejo de errores

```c
/* C: manejo de errores manual, sin excepciones */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(void) {
    FILE *f = fopen("archivo.txt", "r");
    if (f == NULL) {                           /* SIEMPRE verificar NULL */
        perror("Error al abrir el archivo");   /* imprime la descripción del error del SO */
        return EXIT_FAILURE;
    }

    char buf[256];
    /* fgets limita el largo leído: más seguro que gets() o scanf("%s") */
    if (fgets(buf, sizeof(buf), f) == NULL) {
        if (ferror(f)) puts("Error de lectura");
        fclose(f);
        return EXIT_FAILURE;
    }

    /* fgets incluye '\n' en el buffer: hay que eliminarlo manualmente */
    buf[strcspn(buf, "\n")] = '\0';
    printf("Leído: %s\n", buf);

    fclose(f);   /* OBLIGATORIO: liberar el descriptor de archivo */
    return EXIT_SUCCESS;
}
```

```python
# Python: manejo de errores con excepciones tipadas y bloque with
try:
    # with cierra el archivo automáticamente aunque ocurra una excepción
    with open("archivo.txt", "r", encoding="utf-8") as f:
        linea = f.readline().rstrip("\n")   # rstrip elimina '\n' sin necesidad de índices
        print(f"Leído: {linea}")

except FileNotFoundError:
    print("El archivo no existe")
except PermissionError:
    print("Sin permisos de lectura")
except UnicodeDecodeError as e:
    # Se puede inspeccionar el error: posición del byte inválido, etc.
    print(f"Error de encoding en posición {e.start}: {e.reason}")
```

#### Ejemplo: formateo de strings

```c
/* C: formateo con printf y snprintf (biblioteca estándar, no el lenguaje) */
#include <stdio.h>

int main(void) {
    const char *nombre = "Ana";
    int edad            = 30;
    float promedio      = 9.75f;

    /* printf: imprime directamente en stdout */
    printf("Nombre: %s, Edad: %d, Promedio: %.2f\n", nombre, edad, promedio);

    /* snprintf: escribe en buffer con límite de tamaño → más seguro que sprintf */
    char msg[128];
    snprintf(msg, sizeof(msg), "Hola, %s! Tenés %d años.", nombre, edad);
    puts(msg);

    return 0;
}
```

```python
# Python: múltiples mecanismos de formateo nativos en el lenguaje
nombre   = "Ana"
edad     = 30
promedio = 9.75

# f-strings (Python 3.6+, PEP 498) — recomendado: legible y el más rápido
print(f"Nombre: {nombre}, Edad: {edad}, Promedio: {promedio:.2f}")

# Las f-strings admiten expresiones arbitrarias dentro de {}
print(f"Nombre en mayúsculas: {nombre.upper()}")
print(f"Edad en 5 años: {edad + 5}")

# .format() — compatible con Python 2.6+
print("Nombre: {}, Edad: {}, Promedio: {:.2f}".format(nombre, edad, promedio))

# %-formatting — estilo legacy, similar a printf de C
print("Nombre: %s, Edad: %d, Promedio: %.2f" % (nombre, edad, promedio))
```

#### Ejemplo: expresiones regulares

```c
/* C: regex con POSIX <regex.h> (no parte del estándar C básico) */
#include <stdio.h>
#include <regex.h>

int main(void) {
    regex_t re;
    const char *texto  = "Contacto: user@example.com";
    const char *patron = "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}";

    /* compilar el patrón: REG_EXTENDED activa sintaxis ERE */
    if (regcomp(&re, patron, REG_EXTENDED) != 0) {
        puts("Error compilando regex");
        return 1;
    }

    regmatch_t match;
    /* regexec: 0 si hay coincidencia, REG_NOMATCH si no */
    if (regexec(&re, texto, 1, &match, 0) == 0) {
        /* match.rm_so y rm_eo son los offsets de inicio y fin */
        printf("Email: %.*s\n",
               (int)(match.rm_eo - match.rm_so),
               texto + match.rm_so);   /* "user@example.com" */
    }

    regfree(&re);   /* libera recursos del regex compilado */
    return 0;
}
```

```python
# Python: módulo re de la stdlib (Perl-compatible, incluido sin instalar nada)
import re

texto  = "Contacto: user@example.com"
patron = r"[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}"
#          ^ raw string: la \ no se interpreta como escape de Python

match = re.search(patron, texto)
if match:
    print(f"Email: {match.group()}")   # "user@example.com"
    print(f"Inicio: {match.start()}, Fin: {match.end()}")

# Para búsquedas simples, los métodos nativos de str son suficientes y más rápidos
print("user" in texto)                 # True
print(texto.startswith("Contacto"))    # True
print(texto.replace("user", "admin"))  # "Contacto: admin@example.com"
```

#### Ejemplo: entrada/salida (I/O)

```c
/* C: I/O con buffers explícitos y manejo manual del '\n' */
#include <stdio.h>
#include <string.h>

int main(void) {
    char nombre[64];

    printf("Ingresá tu nombre: ");
    fflush(stdout);   /* forzar que el prompt se imprima antes de esperar input */

    /* fgets es más seguro que scanf("%s"): respeta el límite del buffer */
    if (fgets(nombre, sizeof(nombre), stdin) != NULL) {
        /* fgets incluye el '\n': strcspn devuelve el índice del primer '\n' */
        nombre[strcspn(nombre, "\n")] = '\0';
        printf("Hola, %s!\n", nombre);
    }

    /* escribir a archivo */
    FILE *f = fopen("salida.txt", "w");
    if (f != NULL) {
        fprintf(f, "Nombre: %s\n", nombre);
        fclose(f);   /* OBLIGATORIO cerrar el archivo */
    }
    return 0;
}
```

```python
# Python: I/O integrado, encoding automático, sin manejo manual de buffers
nombre = input("Ingresá tu nombre: ")   # retorna str sin '\n', sin límite de buffer
print(f"Hola, {nombre}!")

# Escritura a archivo: encoding UTF-8 explícito (buena práctica en Python 3)
with open("salida.txt", "w", encoding="utf-8") as f:
    f.write(f"Nombre: {nombre}\n")   # write() acepta str directamente

# Lectura de archivo
with open("salida.txt", "r", encoding="utf-8") as f:
    contenido = f.read()    # retorna un str con todo el contenido
    print(contenido)
```

---

## Riesgos frecuentes y buenas prácticas

A continuación, detallamos los errores más comunes y recomendaciones para evitarlos, en cada lenguaje.

| # | C | Python |
| :-: | :-- | :------- |
| 1 | Usár siempre buffers suficientemente grandes o pre-alocados. Evitar usar `gets()` (eliminado en C11) y `strcat()` en bucles sin control de tamaño. | Recordar que `str` es inmutable: `s[0] = "H"` falla, pero `s = "Hola"` crea un nuevo objeto. |
| 2 | Nunca retornar un puntero a una variable local de función: causa `dangling pointer`. | Para concatenar muchas cadenas, es preferente usar `"".join(lista)` en vez de `+` en un loop. |
| 3 | Liberar con `free()` toda la memoria asignada con `malloc()`, y verificar que `malloc()` no retornó `NULL` antes de usarla. | Manejar errores de texto con excepciones como `UnicodeDecodeError` o `ValueError`. |
| 4 | Preferiblemente usar `snprintf()` y `fgets(..., size, ...)` para reducir el riesgo de buffer overflow. | Usar f-strings para mantener el código legible y seguro. |
| 5 | Usar `strcmp()` (nunca `==`) para comparar contenido de strings. | Especificar siempre el encoding al abrir archivos (`open(..., encoding="utf-8")`): el encoding ,por defecto, puede variar entre plataformas. |

---

## Conclusión

- En **C**, los strings son arrays de `char` y nosostros controlamos la memoria. Esto permite optimizaciones de bajo nivel, pero también exige atención a como usamos `malloc`/`free`, alocaciones correctas y buffer overflow.
- En **Python**, `str` es un tipo inmutable con Unicode integrado. Es más seguro y expresivo para manipular texto, aunque conviene aplicar buenas prácticas para operaciones repetitivas como la concatenación.
- En C, la comparación de contenido se hace con `strcmp()`; en Python, `==` compara contenido directamente.

### ¿Cuándo debemos elegir cada lenguaje?

| Criterio | C | Python |
| :--------- | :-- | :------- |
| **Control de memoria** | Cuando necesitamos control absoluto a bajo nivel | Cuando no necesitamos control, delegando al GC |
| **Rendimiento bajo nivel** | Es Óptimo | Depende del caso y su implementación |
| **Seguridad de texto / Unicode** | Manual y dependiente de plataforma | Nativo y completo |
| **Velocidad de desarrollo** | Lenta | Alta |
| **Ecosistema de texto/datos/red** | Limitado (casi siempre tenemos que recurrir a bibliotecas externas) | Nativamente es muy rico |
| **Sistemas embebidos / drivers / SO** | Ideal | No aplica |

---

## Glosario de términos técnicos

A continuación, hacemos referencia a las definiciones de los conceptos técnicos mencionados a lo largo del documento, organizadas alfabéticamente.

---

### A

**ASCII** _(American Standard Code for Information Interchange)_ · `Encoding`
Estándar de codificación de caracteres de 7 bits que representa 128 caracteres: letras del inglés, dígitos, puntuación y caracteres de control. Es la base sobre la que se construyeron encodings posteriores como Latin-1 y UTF-8.
→ Ref.: [RFC 20 — ASCII format for Network Interchange](https://www.rfc-editor.org/rfc/rfc20)

---

### B

**Buffer overflow** · `Memoria`
Error que ocurre cuando se escribe más datos en un buffer de tamaño fijo de los que puede contener, sobreescribiendo memoria adyacente. En C es un vector clásico de bugs y vulnerabilidades de seguridad críticas.
→ Ref.: [CWE-120: Buffer Copy without Checking Size of Input](https://cwe.mitre.org/data/definitions/120.html)

**Built-in** · `Lenguaje`
Elemento (tipo, función, constante) que forma parte del núcleo del lenguaje o intérprete sin necesidad de importar ninguna biblioteca. En Python, `str`, `len()` y `print()` son built-ins.
→ Ref.: [Python Docs — Built-in Types](https://docs.python.org/3/library/stdtypes.html)

---

### C

**Code point** _(Punto de código)_ · `Encoding`
Valor numérico único asignado a cada carácter en el estándar Unicode. Se escribe como U+ seguido del número en hexadecimal. Por ejemplo, U+0041 = 'A' y U+1F600 = '😀'. Un code point no equivale necesariamente a un byte.
→ Ref.: [Unicode Standard — Glossary](https://www.unicode.org/glossary/#code_point)

**CPython** · `Herramientas`
La implementación de referencia del lenguaje Python, escrita en C. Es el intérprete que se descarga por defecto desde python.org. Existen otras implementaciones como PyPy (en Python) o Jython (en Java).
→ Ref.: [CPython source code (GitHub)](https://github.com/python/cpython)

**Cycle collector** _(Recolector de ciclos)_ · `Memoria`
Componente del garbage collector de Python que detecta grupos de objetos que se referencian circularmente entre sí. El reference counting solo no puede limpiarlos (el contador nunca llega a 0), por eso existe este módulo complementario.
→ Ref.: [Python Docs — gc module](https://docs.python.org/3/library/gc.html)

---

### D

**Dangling pointer** _(Puntero colgante)_ · `Memoria`
Puntero que apunta a una región de memoria que ya fue liberada o que dejó de ser válida (por ejemplo, una variable local tras salir de su función). Accederlo produce Undefined Behavior: crash, corrupción silenciosa o comportamiento impredecible.
→ Ref.: [CWE-416: Use After Free](https://cwe.mitre.org/data/definitions/416.html)

---

### E

**Encoding** _(Codificación de caracteres)_ · `Encoding`
Esquema que mapea caracteres abstractos (como code points Unicode) a secuencias concretas de bytes para poder almacenarlos o transmitirlos. Ejemplos: UTF-8 (longitud variable), Latin-1 (1 byte fijo), UCS-4 (4 bytes fijos).
→ Ref.: [Python Docs — codecs](https://docs.python.org/3/library/codecs.html)

---

### F

**First-class citizen** _(Ciudadano de primera clase)_ · `Lenguaje`
Un valor es first-class en un lenguaje si puede: ser asignado a una variable, pasado como argumento a una función, retornado por una función y almacenado en estructuras de datos. En Python los strings lo son; en C los arrays no lo son.
→ Ref.: Strachey, C. (1967). _Fundamental Concepts in Programming Languages_.

**f-string** _(Formatted string literal)_ · `Lenguaje`
Sintaxis de Python 3.6+ que permite interpolar expresiones directamente dentro de un string usando la sintaxis `f"{expresión}"`. Se evalúan en tiempo de ejecución. Son más legibles y rápidas que `.format()` o el operador `%`.
→ Ref.: [PEP 498 — Literal String Interpolation](https://peps.python.org/pep-0498/)

**Format string** _(Cadena de formato)_ · `Lenguaje`
Cadena que contiene marcadores especiales reemplazados por valores al ejecutar el formateo. En C se usan `%s`, `%d`, `%f` con `printf`/`sprintf`; en Python se usan `{}` con `.format()`.
→ Ref.: [cppreference — printf](https://en.cppreference.com/w/c/io/fprintf)

---

### G

**Garbage Collector** _(GC · Recolector de basura)_ · `Memoria`
Mecanismo automático que detecta objetos en memoria que ya no son alcanzables (ninguna variable los referencia) y los libera. Python usa reference counting como mecanismo principal más un cycle collector complementario.
→ Ref.: [Python Docs — gc module](https://docs.python.org/3/library/gc.html)

---

### H

**Heap** _(Memoria dinámica)_ · `Memoria`
Región de memoria gestionada en tiempo de ejecución, de tamaño flexible. En C se accede con `malloc()` y `free()` explícitos. En Python todos los objetos viven en el heap, pero el intérprete lo gestiona automáticamente.
→ Ref.: [cppreference — malloc](https://en.cppreference.com/w/c/memory/malloc)

---

### I

**Interpolación de strings** _(String interpolation)_ · `Lenguaje`
Técnica para insertar el valor de variables o expresiones dentro de un string en tiempo de ejecución, sin concatenación explícita. Python lo soporta nativamente con f-strings (`f"Hola {nombre}"`); C no tiene soporte nativo en el lenguaje.
→ Ref.: [PEP 498 — Literal String Interpolation](https://peps.python.org/pep-0498/)

---

### L

**Latin-1** _(ISO 8859-1)_ · `Encoding`
Codificación de 8 bits que extiende ASCII con 128 caracteres adicionales para lenguas europeas occidentales (acentos, ñ, ü, etc.). Python 3 la usa internamente para strings que solo contienen caracteres dentro de este rango.
→ Ref.: [ISO/IEC 8859-1](https://www.iso.org/standard/28245.html)

---

### M

**malloc / free** · `Memoria`
Funciones de C para gestionar memoria dinámica en el heap. `malloc(n)` reserva n bytes y retorna un puntero; `free(p)` libera la memoria apuntada por p. Olvidar el `free` produce memory leaks; usarlo dos veces es Undefined Behavior.
→ Ref.: [cppreference — Dynamic memory management](https://en.cppreference.com/w/c/memory)

**Mutabilidad** _(Mutable / Immutable)_ · `Lenguaje`
Capacidad de un objeto de ser modificado después de su creación. Los arrays de `char` en C son mutables (se puede cambiar cualquier byte). Los strings de Python (`str`) son completamente inmutables: toda operación crea un nuevo objeto.
→ Ref.: [Python Docs — Immutable Sequence Types](https://docs.python.org/3/library/stdtypes.html#immutable-sequence-types)

---

### O

**O(n) / O(n²)** _(Notación Big O · Complejidad algorítmica)_ · `Algoritmos`
Notación que describe cómo crece el tiempo de ejecución de un algoritmo según el tamaño de la entrada _n_. O(n) es lineal: si n se duplica, el tiempo se duplica. O(n²) es cuadrático: si n se duplica, el tiempo se cuadruplica. La concatenación repetida en loop es O(n²) en ambos lenguajes si se hace ingenuamente.
→ Ref.: Cormen, T. H. et al. (2009). _Introduction to Algorithms_ (3rd ed.), MIT Press. Cap. 3.

---

### P

**Pass-by-object-reference** _(Paso por referencia de objeto)_ · `Lenguaje`
Mecanismo de Python donde las funciones reciben una referencia al objeto original, no una copia. Si el objeto es mutable (como una lista), puede modificarse desde la función. Si es inmutable (como `str`), no hay forma de alterar el original.
→ Ref.: [Python FAQ — How do I write a function with output parameters?](https://docs.python.org/3/faq/programming.html#how-do-i-write-a-function-with-output-parameters-call-by-reference)

**Pass-by-value** _(Paso por valor)_ · `Lenguaje`
Mecanismo donde la función recibe una copia del valor. En C, pasar un `char*` es pass-by-value del puntero: la función recibe una copia de la dirección, pero el contenido del array al que apunta sigue siendo compartido y modificable.
→ Ref.: ISO/IEC 9899:2011 (C11), §6.5.2.2 — Function calls.

**PCRE** _(Perl Compatible Regular Expressions)_ · `Herramientas`
Biblioteca C que implementa un motor de expresiones regulares compatible con la sintaxis de Perl. Es una de las más usadas en proyectos C que necesitan regex avanzado, ya que el estándar C básico no incluye soporte nativo.
→ Ref.: [PCRE — Perl Compatible Regular Expressions](https://www.pcre.org/)

**PEP** _(Python Enhancement Proposal)_ · `Herramientas`
Documento de diseño oficial que describe nuevas características o cambios en el lenguaje Python. PEP 393 define la representación interna flexible de strings (Latin-1/UCS-2/UCS-4 según contenido). PEP 3120 establece UTF-8 como encoding por defecto de los archivos fuente.
→ Ref.: [PEP 393](https://peps.python.org/pep-0393/) · [PEP 3120](https://peps.python.org/pep-3120/)

---

### R

**Reference counting** _(Conteo de referencias)_ · `Memoria`
Técnica de garbage collection donde cada objeto mantiene un contador de cuántas referencias apuntan a él. Cuando el contador llega a 0, el objeto se libera inmediatamente. Es el mecanismo principal de Python, complementado por el cycle collector.
→ Ref.: [Python Docs — gc module](https://docs.python.org/3/library/gc.html)

**Regex** _(Expresión regular · Regular expression)_ · `Herramientas`
Secuencia de caracteres que define un patrón de búsqueda en texto. Permite buscar, extraer, validar o reemplazar substrings con notación compacta. Python incluye el módulo `re` en la stdlib. C no tiene soporte nativo estándar.
→ Ref.: [Python Docs — re module](https://docs.python.org/3/library/re.html)

**.rodata** _(Read-only data segment)_ · `Memoria`
Segmento del binario compilado destinado a datos de solo lectura. Los string literals de C (como `"hola"`) se almacenan aquí en tiempo de compilación. Intentar modificarlos en tiempo de ejecución es Undefined Behavior, ya que el sistema operativo puede proteger esa región de memoria.
→ Ref.: ISO/IEC 9899:2011 (C11), §6.4.5 — String literals.

---

### S

**Stack** _(Pila de ejecución)_ · `Memoria`
Región de memoria de tamaño limitado, gestionada automáticamente en forma LIFO (último en entrar, primero en salir). Las variables locales de funciones en C se alocan aquí. Es más rápida que el heap pero efímera: al salir de la función, sus datos se invalidan.
→ Ref.: [cppreference — Storage duration](https://en.cppreference.com/w/c/language/storage_duration)

**String interning** _(Internación de strings)_ · `Memoria`
Optimización donde el intérprete reutiliza un mismo objeto en memoria para strings con idéntico contenido, en lugar de crear duplicados. Python lo aplica automáticamente para strings pequeños y con forma de identificador. Permite que `is` retorne `True` entre dos strings con el mismo valor, aunque no está garantizado para todos los casos.
→ Ref.: [Python Docs — sys.intern](https://docs.python.org/3/library/sys.html#sys.intern)

---

### U

**UCS-2 / UCS-4** _(Universal Character Set)_ · `Encoding`
Codificaciones Unicode de ancho fijo. UCS-2 usa exactamente 2 bytes por carácter (cubre el Plano Multilingüe Básico). UCS-4 usa 4 bytes por carácter (cubre todos los planos Unicode). Python 3 usa alguna de estas internamente según el carácter más complejo del string (PEP 393).
→ Ref.: [PEP 393 — Flexible String Representation](https://peps.python.org/pep-0393/)

**Undefined Behavior** _(UB · Comportamiento indefinido)_ · `Lenguaje`
En C, una operación cuyo resultado no está definido por el estándar del lenguaje. El compilador puede hacer literalmente cualquier cosa: funcionar correctamente, producir resultados incorrectos, crashear o generar código con vulnerabilidades. Modificar un string literal o desreferenciar un dangling pointer son ejemplos clásicos.
→ Ref.: ISO/IEC 9899:2011 (C11), §3.4.3 — Undefined behavior.

**Unicode** · `Encoding`
Estándar universal que asigna un código numérico único (code point) a cada carácter de todos los sistemas de escritura del mundo: más de 140.000 caracteres de más de 150 escrituras. Es la base de encodings como UTF-8 y UTF-16.
→ Ref.: [The Unicode Standard](https://www.unicode.org/standard/standard.html)

**UTF-8** · `Encoding`
Codificación de longitud variable para Unicode: usa entre 1 y 4 bytes por code point según su valor. Es compatible hacia atrás con ASCII (los primeros 128 caracteres usan exactamente 1 byte). Es el encoding más usado en la web y el por defecto en Python 3 para archivos fuente e I/O de texto.
→ Ref.: [RFC 3629 — UTF-8, a transformation format of ISO 10646](https://www.rfc-editor.org/rfc/rfc3629)

---

## Referencias generales

### Estándar C

| Recurso | Descripción | Enlace |
| :-------- | :------------ | :------- |
| ISO/IEC 9899:2011 (C11) | Estándar oficial del lenguaje C | [Borrador público N1570](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf) |
| cppreference — C string library | Referencia de `<string.h>` | [en.cppreference.com/w/c/string](https://en.cppreference.com/w/c/string) |
| cppreference — Memory management | Referencia de `malloc`, `free`, etc. | [en.cppreference.com/w/c/memory](https://en.cppreference.com/w/c/memory) |
| POSIX `<regex.h>` | Especificación POSIX de regex en C | [opengroup.org](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/regex.h.html) |

### Python

| Recurso | Descripción | Enlace |
| :-------- | :------------ | :------- |
| Python Docs — `str` | Documentación oficial del tipo str | [docs.python.org/3/library/stdtypes.html](https://docs.python.org/3/library/stdtypes.html#text-sequence-type-str) |
| PEP 393 | Representación interna flexible de strings | [peps.python.org/pep-0393](https://peps.python.org/pep-0393/) |
| PEP 3120 | UTF-8 como encoding por defecto de fuentes | [peps.python.org/pep-3120](https://peps.python.org/pep-3120/) |
| PEP 498 | f-strings (Literal String Interpolation) | [peps.python.org/pep-0498](https://peps.python.org/pep-0498/) |
| Python Docs — `re` | Módulo de expresiones regulares | [docs.python.org/3/library/re.html](https://docs.python.org/3/library/re.html) |
| Python Docs — `io.StringIO` | I/O de texto en memoria | [docs.python.org/3/library/io.html](https://docs.python.org/3/library/io.html#io.StringIO) |
| Python Docs — `codecs` | Encodings y decodificaciones disponibles | [docs.python.org/3/library/codecs.html](https://docs.python.org/3/library/codecs.html) |

### Unicode y Encoding

| Recurso | Descripción | Enlace |
| :-------- | :------------ | :------- |
| The Unicode Standard | Estándar oficial de Unicode | [unicode.org/standard](https://www.unicode.org/standard/standard.html) |
| RFC 3629 | Especificación de UTF-8 | [rfc-editor.org/rfc/rfc3629](https://www.rfc-editor.org/rfc/rfc3629) |
| Joel Spolsky — The Absolute Minimum... | Introducción accesible a Unicode | [joelonsoftware.com](https://www.joelonsoftware.com/2003/10/08/the-absolute-minimum-every-software-developer-absolutely-positively-must-know-about-unicode-and-character-sets-no-excuses/) |

### Algoritmos

| Recurso | Descripción |
| :-------- | :------------ |
| Cormen et al. — _Introduction to Algorithms_ (MIT Press, 2009) | Referencia estándar para notación Big O y análisis de complejidad algorítmica. |
