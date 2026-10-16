# Especificación de la Calculadora Infija Manual (TP #23)

## 1.  Descripcion general
Primero vamos a especificar un analizador lexico (*scanner*) y un analizador sintactico (*Parser*). El sistema procesa expresiones aritméticas infijas compuestas por la representación original de números naturales (Pokédex 0–150), identificadores de variables y operadores aritméticos temáticos del mundo Pokémon.

---
## 2. Nivel Lexico

### 2.1. Alfabeto de Entrada (Σ) Σ = { 'a'..'z', 'A'..'Z', '0'..'9', ' ', '\t', '\n', EOF, '♀', '♂', '.', '\'' }

**Nota sobre el alfabeto:** Se excluyeron los símbolos `+`, `-`, `*`, `/` debido a que todas las operaciones aritméticas en este lenguaje se representan mediante palabras compuestas por caracteres alfabéticos.


### 2.2. Categorías Léxicas (Tokens)

El scanner ignora los espacios en blanco (`' '`, `'\t'`, `'\r'`) y clasifica la entrada en las siguientes categorías:

| Token            | Descripción                                   | RegEx                                |
|------------------|-----------------------------------------------|--------------------------------------|
| `numero`         | Nombre de uno de los 151 Pokémon (0 a 150)    | `Mew\|Bulbasaur\|Ivysaur\|...\|Mewtwo` |
| `identificador`  | Nombre de variable                            | `[a-zA-Z][a-zA-Z0-9]*`               |
| `suma`           | Operador de suma                              | `Plusle`                             |
| `resta`          | Operador de resta                             | `Minun`                              |
| `multiplicacion` | Operador de multiplicación                    | `Evolucion`                          |
| `division`       | Operador de división                          | `Corte`                              |
| `fdt`            | Fin de texto                                  | `\n\|EOF`                             |
| `lexerror`       | Cualquier secuencia que no pertenezca al lenguaje | -                                |

Los nombres de Pokémon y los operadores son palabras reservadas. Como también cumplen el patrón de `identificador`, el scanner primero los busca en la tabla de reservadas y, si no están, los toma como identificador.

## 3. Nivel Sintáctico (Gramatica BNF tipo Bison)
El axioma inicial de la gramatica es **<translation-unit>** como se nos indica en las restricciones.

```bnf
<translation-unit> ::= <sentencia>
                     | <translation-unit> <sentencia>

<sentencia>        ::= <expresion> fdt

<expresion>        ::= <expresion> Plusle <termino>
                     | <expresion> Minum <termino>
                     | <termino>

<termino>          ::= <termino> Evolucion <factor>
                     | <termino> Corte <factor>
                     | <factor>

<factor>           ::= identificador
                     | numero
                     | (<expresion>)
```

## 4. Nivel Semantico
### 4.1. Interpretación de operandos

- Un token `numero` se evalúa al entero que corresponde a su posición en la Pokédex. Por ejemplo: `Mew` = 0, `Bulbasaur` = 1, `Pikachu` = 25.
- Un `identificador` se evalúa al valor que tiene guardado en la tabla de símbolos.

### 4.2. Evaluación de operaciones

| Operador    | Operación                                                 |
|-------------|-----------------------------------------------------------|
| `Plusle`    | Suma de ambos operandos                                   |
| `Minun`     | Resta del operando derecho al izquierdo                   |
| `Evolucion` | Producto de ambos operandos                               |
| `Corte`     | División entera del operando izquierdo por el derecho     |

### 4.3. Errores semánticos

- La división por cero (por ejemplo, `Bulbasaur Corte Mew`) aborta la evaluación y se informa como error semántico.


## 5. Casos de Prueba

### 5.1. Cadenas válidas

| # | Cadena                                          | Equivale a       | Resultado |
|---|-------------------------------------------------|------------------|:---------:|
| 1 | `Bulbasaur Plusle Pikachu Evolucion Charmander` | 1 + 25 × 4       | 101       |
| 2 | `Pikachu Evolucion Charmander Corte Ivysaur`    | 25 × 4 / 2       | 50        |
| 3 | `A Plusle Pikachu`                              | A + 25           | depende del valor de `A` |

### 5.2. Cadenas inválidas (errores sintácticos)

| # | Cadena               | Error                                  |
|---|----------------------|----------------------------------------|
| 1 | `Plusle`             | Falta el operando izquierdo            |
| 2 | `Bulbasaur Plusle`   | Falta el operando derecho              |
| 3 | `Plusle Charmander`  | No se permiten operaciones unarias     |


## 6. Arquitectura de Módulos y Dependencias

El sistema está dividido en módulos, cada uno con una responsabilidad propia:

```text
                +-------------+
                |   main.c    |
                +------+------+
                       |
                       | #include "Parser.h"
                       v
                +-------------+
                |  Parser.h   |
                | (Parser.c)  |
                +------+------+
                       |
          +------------+------------+
          |                         |
          | #include "Scanner.h"    | #include "Sem.h"
          v                         v
   +-------------+           +-------------+
   |  Scanner.h  |           |    Sem.h    |
   | (Scanner.c) |           |   (Sem.c)   |
   +------+------+           +-------------+
          |
          | getchar()
          v
      [ stdin ]
```

| Módulo    | Responsabilidad                                                        |
|-----------|------------------------------------------------------------------------|
| `main.c`  | Punto de entrada. Inicia el análisis.                                  |
| `Parser`  | Análisis sintáctico. Pide tokens al Scanner y llama a Sem para evaluar. |
| `Scanner` | Análisis léxico. Lee caracteres de `stdin` y los agrupa en tokens.     |
| `Sem`     | Análisis semántico. Tabla de símbolos, evaluación y errores semánticos. |