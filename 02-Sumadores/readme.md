# TP 20 — Sumadores
## 4. Decimales

### a. Formalización de `decimal-constant`

Siguiendo la especificación estricta de Kernighan & Ritchie (K&R §A2.5.1), una constante decimal en C está formada por una secuencia que inicia obligatoriamente con un dígito distinto de cero (`1` al `9`), seguido de cero o más dígitos (`0` al `9`). El `0` como primer dígito queda excluido ya que en la gramática de C representa el inicio de una constante octal. Para este TP se incorpora el prefijo de signo opcional (`+` o `-`).

---

#### i. Por extensión (operaciones de strings y conjuntos)

* $\Sigma_{\text{signo}} = \{+, -\}$
* $\Sigma_{\text{no\_cero}} = \{1, 2, 3, 4, 5, 6, 7, 8, 9\}$
* $\Sigma_{\text{dígito}} = \{0, 1, 2, 3, 4, 5, 6, 7, 8, 9\}$

Construcción de los lenguajes:
* $L_{\text{signo}} = \Sigma_{\text{signo}} \cup \{\varepsilon\} = \{+, -, \varepsilon\}$
* $L_{\text{no\_cero}} = \Sigma_{\text{no\_cero}} = \{1, 2, 3, 4, 5, 6, 7, 8, 9\}$
* $L_{\text{resto}} = (\Sigma_{\text{dígito}})^* = \{0, 1, 2, 3, 4, 5, 6, 7, 8, 9\}^*$

$$L_{\text{decimal}} = L_{\text{signo}} \cdot L_{\text{no\_cero}} \cdot L_{\text{resto}}$$
$$L_{\text{decimal}} = \{+, -, \varepsilon\} \cdot \{1, \dots, 9\} \cdot \{0, \dots, 9\}^*$$

> **Cómo lo razonamos:** Concatenamos tres conjuntos: el signo opcional ($\varepsilon$ si no viene signo), el primer dígito que obligatoriamente pertenece a $\Sigma_{\text{no\_cero}}$ para respetar la definición de C, y el resto de la cadena generado mediante la clausura de Kleene ($^*$) sobre todos los dígitos posibles.
>
> **Texto de apoyo:** Muchnik Vol. 1, Capítulo 1 (*Operaciones de unión, concatenación y clausura*) y K&R §A2.5.1.

---

#### ii. Con Expresiones Regulares teóricas (Kleene)

$$ER = (+ + - + \varepsilon) \cdot (1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9) \cdot (0 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9)^*$$

> **Cómo lo razonamos:** Tradujimos la concatenación de conjuntos a los operadores formales de Kleene: disyunción ($+$), concatenación ($\cdot$) y repetición ($*$).
>
> **Texto de apoyo:** Muchnik Vol. 1, Capítulo 5 (*Expresiones Regulares y Álgebra de Kleene*).

---

#### iii. Con RegEx (Sintaxis POSIX / analizadores léxicos)

```regex
[+-]?[1-9][0-9]*
```

> **Cómo lo razonamos:** Usamos `[+-]?` para indicar el signo opcional, `[1-9]` para exigir un primer dígito distinto de cero, y `[0-9]*` para admitir cero o más dígitos subsiguientes.
>
> **Texto de apoyo:** Muchnik Vol. 1, Sección 5.6 (*Metalenguaje de Expresiones Regulares*).

---

#### iv. Con Gramáticas Formales

Definimos una **Gramática Regular (GR)** lineal a derecha $G = (V_N, V_T, P, S)$:
* $V_N = \{S, A, B\}$
* $V_T = \{+, -, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9\}$
* Axioma: $S$

Producciones ($P$):
```text
S -> + A | - A | 1 B | 2 B | ... | 9 B | 1 | 2 | ... | 9
A -> 1 B | 2 B | ... | 9 B | 1 | 2 | ... | 9
B -> 0 B | 1 B | ... | 9 B | 0 | 1 | ... | 9
```

> **Cómo lo razonamos:** El axioma $S$ deriva el signo y pasa a $A$, o deriva directamente el primer dígito no cero pasando a $B$ (o terminando si la palabra tiene un solo dígito). El no terminal $A$ fuerza a que lo que siga al signo sea sí o sí del 1 al 9. El no terminal $B$ acumula cualquier dígito del 0 al 9 mediante recursión a derecha.
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

> **Cómo lo razonamos:** Tomamos la gramática del apéndice de K&R, que usa recursión a izquierda para reflejar el orden de reducción de un parser ascendente, agregando la producción de signo opcional.
>
> **Texto de apoyo:** K&R, Apéndice A (Gramática §A2.5.1 y §A13).

---

#### vi. Con Autómata Finito

El autómata se encuentra diagramado en `ConstanteDecimalC23.gv`.

##### Definición formal como 5-upla:
$$M = (Q, \Sigma, T, q_0, F)$$

* $Q = \{\text{STATE\_START}, \text{STATE\_SIGN}, \text{STATE\_DIGITS}, \text{STATE\_SUCCESS}, \text{STATE\_FAIL}\}$
* $\Sigma = \{+, -, 0..9\} \cup \{\text{' '}, \text{'\\t'}, \text{'\\n'}\} \cup \Sigma_{\text{otro}}$
* $q_0 = \text{STATE\_START}$
* $F = \{\text{STATE\_SUCCESS}\}$

##### Tabla de Transiciones (TT):

| Estado actual | `isspace` | `'+'`, `'-'` | `'1'..'9'` | `'0'` | `otro` | `EOF` |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **STATE_START** | STATE_START | STATE_SIGN | STATE_DIGITS | STATE_FAIL *(ungetc)* | STATE_FAIL *(ungetc)* | STATE_FAIL |
| **STATE_SIGN** | STATE_FAIL *(ungetc)* | STATE_FAIL *(ungetc)* | STATE_DIGITS | STATE_FAIL *(ungetc)* | STATE_FAIL *(ungetc)* | STATE_FAIL |
| **STATE_DIGITS** | STATE_SUCCESS *(ungetc)* | STATE_SUCCESS *(ungetc)* | STATE_DIGITS | STATE_DIGITS | STATE_SUCCESS *(ungetc)* | STATE_SUCCESS |
| **STATE_SUCCESS** | — | — | — | — | — | — |
| **STATE_FAIL** | — | — | — | — | — | — |

*(Nota: En la implementación en C, la llegada de `EOF` se gestiona como condición de corte del flujo).*