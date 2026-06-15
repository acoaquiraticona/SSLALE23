# 📘 Trabajo Práctico N°1 — Strings

> **Sintaxis y Semántica de los Lenguajes**
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
| 1 | Eva Olave | 152.952-3 |
| 2 | Alejandro Coaquira Ticona | 222.229-2 |
| 3 | Joaquín Benicio Domínguez | 233.839-7 |
| 4 | Juan Gabriel Laterza Rosa | 163.171-8 |
| 5 | Julian Clemente | 222.226-7 |
| 6 | Thiago Elias Silva | 222.709-5 |
| 7 | Thiago Gabriel Salles | 234.176-1 |
| 8 | Victor Fernández | 174.545-1 |

---

## 📂 Estructura del Repositorio

```01-Strings/
├── README.md               # Este archivo — descripción general del trabajo
├── AnálisisComparativo.md  # Análisis comparativo entre lenguaje C y Python
├── String.md               # Documentación del módulo String
├── Makefile                # Reglas de compilación y ejecución
│
├── 🔷 Módulo String
│   ├── String.h            # Interfaz pública del TAD String
│   ├── String.c            # Implementación del TAD String
│   └── StringTest.c        # Pruebas unitarias del módulo String
│
├── 🔷 Módulo Conversión
│   ├── Conversion.h        # Interfaz del módulo de conversión
│   ├── Conversion.c        # Implementación del módulo de conversión
│   └── ConversionTest.c    # Pruebas unitarias del módulo Conversión
│
└── 🔷 Programas con argumentos de línea de comando
    ├── enlineas.c          # Muestra cada argumento en una línea propia
    ├── longitudes.c        # Longitud de cada argumento
    ├── mayorlongitud.c     # Argumento con mayor longitud
    ├── todosiguales.c      # Uno si son todos los argumentos iguales, si no, cero
    └── suma.c              # Suma de todos los argumentos que son enteros en base 10. Variación: signo opcional
```

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

- [`String.md`](./String.md) — Descripción detallada de la interfaz del TAD String: funciones, parámetros, valores de retorno y ejemplos de uso.  
- [`AnálisisComparativo.md`](./AnálisisComparativo.md) — Comparación entre Lenguaje C y Python.

---

## 📝 Notas

- Se respetó el paradigma de **programación modular**, separando interfaz (`.h`) de implementación (`.c`).
- Los archivos de test (`*Test.c`) son independientes y pueden compilarse y ejecutarse por separado.
