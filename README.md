# 📘 Repo Sintaxis y Semántica de los Lenguajes - Grupo 9

> **Trabajos Prácticos N°1, N°2 y N°3**
>
> **Ingeniería en Sistemas de la Información** · **Universidad Tecnológica Nacional - Facultad Regional Buenos Aires**

---

## 📋 Información General

| Campo        | Detalle                                           |
|:------------ |:--------------------------------------------------|
| 📚 Materia   | Sintaxis y Semántica de los Lenguajes             |
| 🎓 Carrera   | Ingeniería en Sistemas de la Información          |
| 🏫 Curso     | K2052                                             |
| 📅 Año       | 2026                                              |
| 👨‍🏫 Docente   | Ing. José María Sola                              |

---

## 👥 Integrantes

| N° | Nombre Completo | Legajo |
| :--- | :------- | :----- |
| 1 | Alejandro Coaquira Ticona | 222.229-2 |
| 2 | Joaquín Benicio Domínguez | 233.839-7 |
| 3 | Juan Gabriel Laterza Rosa | 163.171-8 |
| 4 | Julian Clemente | 222.226-7 |
| 5 | Thiago Gabriel Vazquez | 234.176-1 |
| 6 | Victor Fernández | 174.545-1 |

---

## 📂 Estructura del Repositorio

```text
26-052-09/
├── README.md                               # Descripción general del proyecto
├── 26-052-09.code-workspace                # Configuración del workspace
├── 01-Strings/                             # Trabajo Práctico N°1: Strings
│   ├── AnálisisComparativo.md              # Comparación entre C y Python
│   ├── Conversion.c                        # Implementación del módulo de conversión
│   ├── Conversion.h                        # Interfaz del módulo de conversión
│   ├── ConversionTest.c                    # Pruebas del módulo de conversión
│   ├── enlineas.c                          # Muestra cada argumento por línea
│   ├── longitudes.c                        # Longitud de cada argumento
│   ├── mayorlongitud.c                     # Recupera el argumento de mayor longitud
│   ├── Makefile                            # Compilación y ejecución del TP1
│   ├── Strings.c                           # Implementación del TAD String
│   ├── Strings.h                           # Interfaz del TAD String
│   ├── Strings.md                          # Documentación del módulo String
│   ├── StringsTest.c                       # Pruebas del módulo String
│   ├── suma.c                              # Suma de enteros presentes en argumentos
│   └── todosiguales.c                      # Verifica si todos los argumentos son iguales
├── 02-Sumadores/                           # Trabajo Práctico N°2: Sumadores
│   ├── Makefile                            # Compilación y ejecución del TP2
│   ├── readme.md                           # Resumen del ejercicio
│   ├── SumarBytes.c                        # Suma de bytes
│   ├── SumarDecimalesScanf.c               # Sumador con scanf
│   ├── SumarDecimalGetNextDecimal.c        # Variante de lectura decimal
│   ├── SumarInts.c                         # Suma de enteros
│   ├── SumarInt_Ns.c                       # Versión con enteros sin signo
│   ├── SumarX.c                            # Solución principal del sumador X
│   ├── ConstanteDecimalC23.gv              # Diagrama relacionado con constante decimal
│   ├── ConstanteDecimalX.gv                # Diagrama relacionado con constante decimal X
│   ├── DefinicionSumarX.md                 # Definición formal del lenguaje X (Pokémon)
│   ├── OverEngineered SumarX/              # Versión modular y completa de SumarX
│   │   ├── SumarX.c                        # Implementación principal
│   │   ├── DefinicionSumarX/               # Documentación detallada
│   │   │   ├── DefinicionSumarX.md         # Especificación formal del lenguaje
│   │   │   └── SumaXME.md                  # Material adicional del ejercicio
│   │   ├── SumarXAux/                      # Módulos auxiliares
│   │   │   ├── PokeArithmetic/             # Lógica aritmética en base 151
│   │   │   │   ├── PokeArithmetic.c
│   │   │   │   └── PokeArithmetic.h
│   │   │   ├── Pokedex/                    # Gestión de la pokédex
│   │   │   │   ├── Pokedex.c
│   │   │   │   └── Pokedex.h
│   │   │   └── PokeParser/                 # Parser de entrada
│   │   │       ├── ParserErrors.h
│   │   │       ├── PokeParser.c
│   │   │       └── PokeParser.h
│   │   └── PokeTests/                      # Casos de prueba
│   │       ├── DefinicionPokeTests.md      # Especificación de tests
│   │       ├── TestPokeAdder.c             # Pruebas del sumador
│   │       ├── TestPokeArithmetic.c        # Pruebas de aritmética
│   │       ├── TestPokeIntegracion.c       # Pruebas de integración
│   │       └── TestPokeParser.c            # Pruebas del parser
└── 03-Calculadora/                         # Trabajo Práctico N°3: Calculadora Pokémon
    ├── Calc.md                            # Especificación del lenguaje y casos de aceptación
    ├── Makefile                           # Compilación de pokecalc y ejecución de pruebas
    ├── main.c                             # Punto de entrada y opción de traza --trace
    ├── Scanner.c                          # Implementación del analizador léxico
    ├── Scanner.h                          # Tokens e interfaz del analizador léxico
    ├── Parser.c                           # Análisis sintáctico y coordinación del intérprete
    ├── Parser.h                           # Interfaz del analizador sintáctico
    ├── Semantics.c                        # Evaluación de expresiones y gestión de variables
    ├── Semantics.h                        # Interfaz y límites del módulo semántico
    ├── errors.c                           # Formato y emisión de diagnósticos y trazas
    ├── errors.h                           # Códigos de error y tipos de diagnóstico
    ├── expresiones.txt                    # Sesión de ejemplo para la entrada estándar
    ├── tests/                             # Pruebas unitarias y de aceptación
    │   ├── scanner_test.c                 # Reconocimiento de tokens y errores léxicos
    │   ├── parser_test.c                  # Pruebas del parser con un scanner simulado
    │   ├── semantics_test.c               # Operaciones, variables y límites numéricos
    │   ├── errors_test.c                  # Formato de diagnósticos y registros
    │   ├── acceptance_test.c              # Verificación del ejecutable, salidas y estados
    │   ├── casos.h                        # Tabla de casos de aceptación
    │   └── test_support.h                 # Utilidades compartidas por las pruebas
    └── assets/                            # Imágenes utilizadas en Calc.md
        ├── pokecalc-banner.png            # Banner de la calculadora
        ├── comandos/                      # Ilustraciones de los comandos de variables
        │   ├── captura.png
        │   └── liberar.png
        ├── errores/                       # Ilustración del error MissingNo
        │   └── missigno.png
        ├── operadores/                    # Ilustraciones de los operadores del lenguaje
        │   ├── plusle.png
        │   ├── minun.png
        │   ├── tandemaus.png
        │   └── ditto.png
        └── pokemones/                     # Pokémon que representan los dígitos del 0 al 9
            ├── 0_mew.png
            ├── 1_bulbasaur.png
            ├── 2_ivysaur.png
            ├── 3_venusaur.png
            ├── 4_charmander.png
            ├── 5_charmeleon.png
            ├── 6_charizard.png
            ├── 7_squirtle.png
            ├── 8_wartortle.png
            └── 9_blastoise.png
```

> Este repo agrupa tres trabajos prácticos: Strings, Sumadores y Calculadora Pokémon, cada uno con su propia estructura, documentación y archivos de implementación.

> Nota: la calculadora no fue realizado en un fork debido a que la organizacion de SSL no lo permite.

---

## 🧩 Descripción de los Módulos

### `String.h / String.c`

Implementación de un Tipo Abstracto de Dato (TAD) **String** en C. Encapsula las operaciones básicas de consulta y creación sobre cadenas de caracteres como IsEmpty, Reverse, Power, etc.

### `Conversion.h / Conversion.c`

Módulo de **conversión** entre tipos de datos y cadenas. Permite transformar cadenas validas a enteros.

### Programas con argumentos de línea de comando

Serie de programas que esperan argumentos en su línea de comandos. Todos los programas envían su salida a stdout.

### Calculadora Pokémon (`03-Calculadora/`)

Intérprete modular en C que separa el análisis léxico (`Scanner`), el análisis sintáctico (`Parser`), la evaluación y gestión de variables (`Semantics`) y los diagnósticos (`errors`). Los nombres Pokémon representan dígitos decimales; el lenguaje permite sumar, restar, multiplicar, duplicar valores con `ditto` y administrar variables con `captura` y `liberar`.

---

## ⚙️ Compilación y Ejecución

Cada trabajo práctico incluye su propio `Makefile`. Desde la carpeta `01-Strings/`, ejecutar:

```bash
# Compilar todos los módulos y programas
make

# Compilar y ejecutar los tests del módulo String
make test_strings

# Compilar y ejecutar los tests del módulo Conversión
make test_conversion

# Eliminar archivos compilados
make clean
```

Desde la carpeta `03-Calculadora/`, ejecutar:

```bash
# Compilar el intérprete
make

# Ejecutar la sesión de ejemplo
./pokecalc < expresiones.txt

# Ejecutar la misma sesión con traza de tokens en stderr
./pokecalc --trace < expresiones.txt

# Compilar y ejecutar las cinco suites de pruebas
make test

# Eliminar el ejecutable y los archivos generados en .build/
make clean
```

La calculadora lee sentencias terminadas en `;` desde la entrada estándar. Los resultados se escriben en `stdout`; los diagnósticos, registros y trazas, en `stderr`.

> **Requisitos:** compilador C y utilidad `make` instalados en el sistema. Para el TP3, el compilador debe admitir C23 (opción `-std=c23` o `-std=c2x`) y las pruebas requieren herramientas de shell POSIX; en Windows se puede usar MSYS2.

---

## 📄 Documentación

- [`01-Strings/Strings.md`](./01-Strings/Strings.md) — Descripción detallada de la interfaz del TAD String: funciones, parámetros, valores de retorno y ejemplos de uso.  
- [`01-Strings/AnálisisComparativo.md`](./01-Strings/AnálisisComparativo.md) — Comparación entre Lenguaje C y Python.
- [`02-Sumadores/readme.md`](./02-Sumadores/readme.md) — Resumen y especificaciones del trabajo práctico de sumadores.
- [`02-Sumadores/DefinicionSumarX.md`](./02-Sumadores/DefinicionSumarX.md) — Definición formal del lenguaje X basado en nombres de Pokémon (151 primeros).
- [`02-Sumadores/OverEngineered SumarX/DefinicionSumarX/DefinicionSumarX.md`](./02-Sumadores/OverEngineered%20SumarX/DefinicionSumarX/DefinicionSumarX.md) — Especificación formal detallada del sumador con ejemplos de autómatas y gramáticas.
- [`02-Sumadores/OverEngineered SumarX/DefinicionSumarX/SumaXME.md`](./02-Sumadores/OverEngineered%20SumarX/DefinicionSumarX/SumaXME.md) — Material complementario del ejercicio.
- [`03-Calculadora/Calc.md`](./03-Calculadora/Calc.md) — Especificación de la Calculadora Pokémon: tokens, gramática, semántica, diagnósticos y casos de aceptación.
- [`03-Calculadora/expresiones.txt`](./03-Calculadora/expresiones.txt) — Sesión de ejemplo con asignación, evaluación, duplicación y liberación de una variable.

---

## 📝 Notas

- Se respetó el paradigma de **programación modular**, separando interfaz (`.h`) de implementación (`.c`).
- Las pruebas de Strings usan archivos `*Test.c`; las de Calculadora, `tests/*_test.c`. Para ejecutar las de Calculadora, usar `make test` desde `03-Calculadora/`: el Makefile enlaza las dependencias de cada suite y las ejecuta en secuencia porque comparten archivos temporales.
