# TP 2 — Sumadores

## 4. Decimales

### a. Formalización de `decimal-constant`

Siguiendo la especificación estricta de Kernighan & Ritchie (K&R - 2.5.1), una constante decimal en C está formada por una secuencia que inicia obligatoriamente con un dígito distinto de cero (`1` al `9`), seguido de cero o más dígitos (`0` al `9`). El `0` como primer dígito queda excluido, ya que en la gramática de C representa el inicio de una constante octal.

Para este TP se incorpora el prefijo de signo opcional (`+` o `-`).

---

#### i. Por extensión (operaciones de strings y conjuntos)

* $\Sigma_{\text{signo}} = \{+, -\}$
* $\Sigma_{\text{nocero}} = \{1, 2, 3, 4, 5, 6, 7, 8, 9\}$
* $\Sigma_{\text{digito}} = \{0, 1, 2, 3, 4, 5, 6, 7, 8, 9\}$

**Construcción de los lenguajes:**

* $L_{\text{signo}} = \Sigma_{\text{signo}} \cup \{\varepsilon\} = \{+, -, \varepsilon\}$
* $L_{\text{nocero}} = \Sigma_{\text{nocero}} = \{1, 2, 3, 4, 5, 6, 7, 8, 9\}$
* $L_{\text{resto}} = (\Sigma_{\text{digito}})^* = \{0, 1, 2, 3, 4, 5, 6, 7, 8, 9\}^*$

$$
L_{\text{decimal}} = L_{\text{signo}} \cdot L_{\text{nocero}} \cdot L_{\text{resto}}
$$

$$
L_{\text{decimal}} = \{+, -, \varepsilon\} \cdot \{1, \dots, 9\} \cdot \{0, \dots, 9\}^*
$$

> **Cómo lo razonamos:**
>
> Concatenamos tres conjuntos: el signo opcional ($\varepsilon$ si no viene signo), el primer dígito que obligatoriamente pertenece a $\Sigma_{\text{nocero}}$ para respetar la definición de C, y el resto de la cadena generado mediante la clausura de Kleene ($^*$) sobre todos los dígitos posibles.
>
> **Texto de apoyo:** Muchnik Vol. 1, Capítulo 1 (*Operaciones de unión, concatenación y clausura*) y K&R 2.5.1.

---

#### ii. Con Expresiones Regulares teóricas (Kleene)

$$
ER = (+ + - + \varepsilon) \cdot (1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9) \cdot (0 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9)^*
$$

> **Cómo lo razonamos:**  
>
> Tradujimos la concatenación de conjuntos a los operadores formales de Kleene: disyunción ($+$), concatenación ($\cdot$) y repetición ($*$).
>
> **Texto de apoyo:** Muchnik Vol. 1, Capítulo 5 (*Expresiones Regulares y Álgebra de Kleene*).

---

#### iii. Con RegEx (Sintaxis POSIX / analizadores léxicos)

```regex
[+-]?[1-9][0-9]*
```

> **Cómo lo razonamos:**
>
> Usamos `[+-]?` para indicar el signo opcional, `[1-9]` para exigir un primer dígito distinto de cero, y `[0-9]*` para admitir cero o más dígitos subsiguientes.
>
> **Texto de apoyo:** Muchnik Vol. 1, Sección 5.6 (*Metalenguaje de Expresiones Regulares*).

---

#### iv. Con Gramáticas Formales

Definimos una **Gramática Regular (GR)** lineal a derecha $G = (V_N, V_T, P, S)$:

* $V_N = \{S, A, B\}$
* $V_T = \{+, -, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9\}$
* Axioma: $S$

**Producciones ($P$):**

```text
S → + A | - A | 1 B | 2 B | … | 9 B | 1 | 2 | … | 9
A → 1 B | 2 B | … | 9 B | 1 | 2 | … | 9
B → 0 B | 1 B | … | 9 B | 0 | 1 | … | 9
```

> **Cómo lo razonamos:**
>
> El axioma $S$ deriva el signo y pasa a $A$, o deriva directamente el primer dígito no cero pasando a $B$ (o terminando si la palabra tiene un solo dígito). El no terminal $A$ fuerza a que lo que siga al signo sea sí o sí del 1 al 9. El no terminal $B$ acumula cualquier dígito del 0 al 9 mediante recursión a derecha.
>
> **Texto de apoyo:** Muchnik Vol. 1, Capítulo 2 (*Gramáticas Tipo 3*).

---

#### v. Con BNF (Estándar K&R §A2.5.1)

```bnf
<constante-decimal-con-signo> ::= <signo-opc> <constante-decimal>

<signo-opc> ::= "+" | "-" | ε

<constante-decimal> ::= <digito-no-cero>
                      | <constante-decimal> <digito>

<digito-no-cero> ::= "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"

<digito> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
```

> **Cómo lo razonamos:**  
> Tomamos la gramática del apéndice de K&R, que usa recursión a izquierda para reflejar el orden de reducción de un parser ascendente, agregando la producción de signo opcional.
>
> **Texto de apoyo:** K&R, Apéndice A (Gramática 2.5.1 y 13).

---

#### vi. Con Autómata Finito

El autómata se encuentra diagramado en `ConstanteDecimalC23.gv`.

##### Definición formal como 5-upla

$$
M = (Q, \Sigma, T, q_0, F)
$$

donde:

* **$Q$ (Estados):** `{STATE_START, STATE_SIGN, STATE_DIGITS, STATE_SUCCESS, STATE_FAIL}`
* **$\Sigma$ (Alfabeto):** $\{+, -\} \cup \{0, 1, \dots, 9\} \cup \Sigma_{\text{blanco}} \cup \Sigma_{\text{otro}}$
* **$q_0$ (Estado inicial):** `STATE_START`
* **$F$ (Estados de aceptación):** `{STATE_SUCCESS}`

##### Tabla de Transiciones (TT)

| Estado actual     | `isspace`                  | `'+'`, `'-'`               | `'1'..'9'`     | `'0'`                    | `otro`                     | `EOF`          |
|-------------------|----------------------------|----------------------------|----------------|--------------------------|----------------------------|----------------|
| **STATE_START**   | STATE_START                | STATE_SIGN                 | STATE_DIGITS   | STATE_FAIL *(ungetc)*    | STATE_FAIL *(ungetc)*      | STATE_FAIL     |
| **STATE_SIGN**    | STATE_FAIL *(ungetc)*      | STATE_FAIL *(ungetc)*      | STATE_DIGITS   | STATE_FAIL *(ungetc)*    | STATE_FAIL *(ungetc)*      | STATE_FAIL     |
| **STATE_DIGITS**  | STATE_SUCCESS *(ungetc)*   | STATE_SUCCESS *(ungetc)*   | STATE_DIGITS   | STATE_DIGITS             | STATE_SUCCESS *(ungetc)*   | STATE_SUCCESS  |
| **STATE_SUCCESS** | —                          | —                          | —              | —                        | —                          | —              |
| **STATE_FAIL**    | —                          | —                          | —              | —                        | —                          | —              |

> **Nota:** En la implementación en C, la llegada de `EOF` se gestiona como condición de corte del flujo.




# Sumador X — "PokeDecimal"

Problema 5 del trabajo **Sumadores**: lenguaje regular propio para representar enteros. Se eligió un alfabeto de dígitos basado en las iniciales de diez Pokémon.

## 1. El lenguaje X (PokeDecimal)

Cada dígito decimal se representa con una letra (mayúscula o minúscula),
inicial de un Pokémon:

| Letra | Pokémon    | Valor |
|:-----:|------------|:-----:|
|  B    | Bulbasaur  |   0   |
|  C    | Charmander |   1   |
|  S    | Squirtle   |   2   |
|  P    | Pikachu    |   3   |
|  J    | Jigglypuff |   4   |
|  M    | Meowth     |   5   |
|  E    | Eevee      |   6   |
|  G    | Gengar     |   7   |
|  O    | Onix       |   8   |
|  N    | Ninetales  |   9   |

Un número puede llevar signo opcional, igual que `decimal-constant`.

Ejemplo: `-PSC` = -(3)(2)(1) = **-321**

### 1.i. Por extensión (strings y conjuntos)

Sea `D = {B,C,S,P,J,M,E,G,O,N,b,c,s,p,j,m,e,g,o,n}` (un símbolo por dígito,
sin distinguir mayúscula/minúscula).

    PokeNumero = { s·d1·d2·...·dn : n ≥ 1, di ∈ D, s ∈ {"", "+", "-"} }

### 1.ii/1.iii. Expresión Regular / RegEx

    [+-]?[BCSPJMEGONbcspjmegon]+

### 1.iv. Gramática Formal (GIC)

    PokeNumero  -> Signo PokeDigitos
    Signo       -> '+' | '-' | ε
    PokeDigitos -> PokeDigito PokeDigitos | PokeDigito
    PokeDigito  -> 'B'|'C'|'S'|'P'|'J'|'M'|'E'|'G'|'O'|'N'
                 | 'b'|'c'|'s'|'p'|'j'|'m'|'e'|'g'|'o'|'n'

### 1.v. BNF

    <poke-numero>   ::= <signo> <poke-digitos>
    <signo>         ::= "+" | "-" | ""
    <poke-digitos>  ::= <poke-digito> | <poke-digito> <poke-digitos>
    <poke-digito>   ::= "B" | "C" | "S" | "P" | "J" | "M" | "E" | "G" | "O" | "N"
                       | "b" | "c" | "s" | "p" | "j" | "m" | "e" | "g" | "o" | "n"

### 1.vi. Autómata Finito

Ver `ConstanteDecimalX.gv`. Estados:

- `start`: inicial, consume espacios.
- `sign_leido`: se leyó `+`/`-`, todavía no hay ningún dígito.
- `primer_digito`: se exige el primer PokeDigito (si no aparece, error).
- `acumulando`: estado de aceptación; permanece mientras entren PokeDigitos.

n-upla formal: `(Q, Σ, δ, q0, F)` con

- `Q = {start, sign_leido, primer_digito, acumulando, error}`
- `Σ = {'+','-',espacio} ∪ D ∪ {otros}`
- `q0 = start`
- `F = {acumulando}`
- `δ` según las transiciones dibujadas en el `.gv`.

## 2. Implementación elegida: goto (Implementación #2)

En `SumarX.c`, `GetNextX` implementa el autómata de arriba con **etiquetas
como estados** y **goto como transición**, siguiendo 3.8 *Goto and labels*
de K&R, tal como pide la consigna.

## 3. ¿Tiene alguna aplicación el goto hoy en día?

Sí, en casos puntuales y acotados, aunque sigue siendo evitado como
herramienta de control de flujo general:

- **Limpieza de recursos / manejo de errores en C**: el patrón
  `goto cleanup;` al final de una función (liberar múltiples recursos en
  orden inverso) es idiomático en proyectos como el kernel de Linux, porque
  C no tiene `finally` ni RAII.
- **Salir de loops anidados**: cuando `break` solo rompe un nivel, `goto`
  evita banderas auxiliares.
- **Código generado automáticamente**: generadores de parsers/lexers
  (herramientas al estilo Ragel) o traducciones de máquinas de estado
  suelen emitir `goto`s porque el código no lo escribe ni lo mantiene una
  persona.
- **Lenguajes modernos que lo conservan**: C, C++ y Go incluyen `goto`
  (Go lo agregó deliberadamente, con restricciones sintácticas para
  evitar los peores abusos). Python, Java, Rust y JavaScript no lo tienen
  como tal — Java se reservó la palabra pero nunca la implementó, y Rust
  ofrece `break`/`continue` con etiquetas como alternativa más segura para
  el caso de loops anidados.

En síntesis, la crítica de Dijkstra sigue vigente para el uso
indiscriminado del `goto` como reemplazo de estructuras de control, pero
sobrevive como herramienta puntual para patrones muy específicos que el
lenguaje no resuelve de otra forma.
