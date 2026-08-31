# 📘 Repo Sintaxis y Semántica de los Lenguajes - Grupo 9

> **Trabajos Prácticos N°1 y N°2**
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
| 5 | Thiago Gabriel Salles | 234.176-1 |
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
│   ├── DefinicionSumarX/                   # Documentación y diagramas de SumarX
│   │   ├── DefinicionSumarX.md             # Definición del sumador X
│   │   ├── SumarX.gv                       # Diagrama de SumarX
│   │   └── SumaXME.md                      # Material adicional del ejercicio
│   ├── SumarXAux/                          # Módulos auxiliares para SumarX
│   │   ├── PokeArithmetic/                 # Lógica aritmética y pruebas
│   │   │   ├── PokeArithmetic.c
│   │   │   └── PokeArithmetic.h
│   │   ├── Pokedex/                        # Módulo de pokédex auxiliar
│   │   │   ├── Pokedex.c
│   │   │   └── Pokedex.h
│   │   └── PokeParser/                     # Parser de datos auxiliar
│   │       ├── ParserErrors.h
│   │       ├── PokeParser.c
│   │       └── PokeParser.h
│   └── Tests/                              # Pruebas del TP2
│       └── PokeTests/                      # Casos de prueba del módulo Poke
│           ├── DefinicionPokeTests.md
│           ├── TestPokeAdder.c
│           ├── TestPokeArithmetic.c
│           ├── TestPokeIntegracion.c
│           └── TestPokeParser.c
└──
```

> Este repo agrupa los dos trabajos prácticos que vimos hasta ahora: el TP de cadenas y el TP de sumadores, cada uno con su propia estructura y archivos de documentación y pruebas.

---

## 🧩 Descripción de los Módulos

### `String.h / String.c`

Implementación de un Tipo Abstracto de Dato (TAD) **String** en C. Encapsula las operaciones básicas de consulta y creación sobre cadenas de caracteres como IsEmpty, Reverse, Power, etc.

### `Conversion.h / Conversion.c`

Módulo de **conversión** entre tipos de datos y cadenas. Permite transformar cadenas validas a enteros.

### Programas con argumentos de línea de comando

Serie de programas que esperan argumentos en su línea de comandos. Todos los programas envían su salida a stdout.

---

## ⚙️ Compilación y Ejecución

Este proyecto incluye un `Makefile` para simplificar la compilación. Desde la carpeta `01-Strings/`, ejecutar:

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

> **Requisitos:** compilador `gcc` y utilidad `make` instalados en el sistema.

---

## 📄 Documentación

- [`01-Strings/Strings.md`](./01-Strings/Strings.md) — Descripción detallada de la interfaz del TAD String: funciones, parámetros, valores de retorno y ejemplos de uso.  
- [`01-Strings/AnálisisComparativo.md`](./01-Strings/AnálisisComparativo.md) — Comparación entre Lenguaje C y Python.
- [`02-Sumadores/readme.md`](./02-Sumadores/readme.md) — Resumen y especificaciones del trabajo práctico de sumadores.

---

## 📝 Notas

- Se respetó el paradigma de **programación modular**, separando interfaz (`.h`) de implementación (`.c`).
- Los archivos de test (`*Test.c`) son independientes y pueden compilarse y ejecutarse por separado.
