# Definición Formal del Lenguaje X — SumarX (Pokémon)

## Contenido

1. [Descripción general](#1-descripción-general)
2. [Alfabeto del lenguaje](#2-alfabeto-del-lenguaje)
3. [Definición por extensión](#3-definición-por-extensión)
4. [Definición mediante operaciones de strings y conjuntos](#4-definición-mediante-operaciones-de-strings-y-conjuntos)
5. [Definición mediante Expresiones Regulares](#5-definición-mediante-expresiones-regulares)
6. [Definición mediante RegEx](#6-definición-mediante-regex)
7. [Gramática formal](#7-gramática-formal)
8. [Definición mediante BNF](#8-definición-mediante-bnf)
9. [BNF extendida (EBNF)](#9-bnf-extendida-ebnf)
10. [Autómata Finito](#10-autómata-finito)
11. [AFD formal (5-upla)](#11-afd-formal-5-upla)
12. [Ejemplos de pertenencia al lenguaje](#12-ejemplos-de-pertenencia-al-lenguaje)
13. [Relación entre lenguaje y semántica](#13-relación-entre-lenguaje-y-semántica)
14. [Correspondencia con la implementación](#14-correspondencia-con-la-implementación)

---

## 1. Descripción general

El lenguaje **X** utilizado por `SumarX` está formado por los **nombres exactos** de los primeros 151 Pokémon de la Pokédex.

Cada nombre representa un valor entero en el rango 0 a 150:

| Pokémon   | Valor |
|-----------|:-----:|
| Mew       | 0     |
| Bulbasaur | 1     |
| Ivysaur   | 2     |
| …         | …     |
| Mewtwo    | 150   |

El programa lee de la entrada estándar una secuencia de estos nombres (uno por token) y va acumulando sus valores. El acumulador se mantiene en **base 151**, de modo que cuando la suma supera 150 se genera un *carry* y aparece un nuevo dígito (representado también como un Pokémon).

> **Importante**  
> El lenguaje formal X describe **un solo token** (un nombre de Pokémon).  
> La suma y el manejo del carry pertenecen a la **semántica** del programa, no al lenguaje que reconoce `GetNextX`.

---

## 2. Alfabeto del lenguaje

$$\Sigma = \text{letras latinas (A-Z, a-z)} \cup \{♀, ♂, \text{apóstrofo}, \text{punto}, \text{espacio}, \text{salto de línea}\}$$

| Categoría              | Ejemplos                          |
|------------------------|-----------------------------------|
| Letras                 | `P`, `i`, `k`, `a`, `c`, `h`, `u` |
| Caracteres especiales  | `♀` (Nidoran♀), `♂` (Nidoran♂), `'` (Farfetch'd), `.` (Mr. Mime) |
| Separadores            | espacio, tabulador, salto de línea (`\n`) |
| Fin de flujo           | `EOF`                             |

---

## 3. Definición por extensión

Sea \( P \) el conjunto de los 151 nombres válidos:

$$P = \{\text{Mew}, \text{Bulbasaur}, \text{Ivysaur}, \ldots, \text{Mewtwo}\}$$

El lenguaje se define simplemente como:

$$
L_X = P
$$

Es decir, una cadena \( w \) pertenece a \( L_X \) si y solo si \( w \) es exactamente uno de los 151 nombres de la Pokédex.

---

## 4. Definición mediante operaciones de strings y conjuntos

$$
L_X = \bigcup_{p \in P} \{p\}
$$

donde \( \{p\} \) es el conjunto unitario que contiene al string \( p \).

En la implementación esta pertenencia se resuelve con:

```c
strcmp(name, pokedex[i]) == 0
```

para algún \( 0 \le i < 151 \).

La función de interpretación (semántica) es:

$$f : P \rightarrow \{0,1,\ldots,150\}$$

donde $f(\text{Mew}) = 0$, $f(\text{Bulbasaur}) = 1$, …, $f(\text{Mewtwo}) = 150$.

---

## 5. Definición mediante Expresiones Regulares

Como el vocabulario es finito, el lenguaje se expresa como la unión de todos los nombres:

$$L_X = \text{Mew} + \text{Bulbasaur} + \text{Ivysaur} + \cdots + \text{Mewtwo}$$

(usando la notación clásica de expresiones regulares donde $+$ denota unión).

---

## 6. Definición mediante RegEx

```regex
^(Mew|Bulbasaur|Ivysaur|Venusaur|Charmander|Charmeleon|Charizard|Squirtle|Wartortle|Blastoise|Caterpie|Metapod|Butterfree|Weedle|Kakuna|Beedrill|Pidgey|Pidgeotto|Pidgeot|Rattata|Raticate|Spearow|Fearow|Ekans|Arbok|Pikachu|Raichu|Sandshrew|Sandslash|Nidoran♀|Nidorina|Nidoqueen|Nidoran♂|Nidorino|Nidoking|Clefairy|Clefable|Vulpix|Ninetales|Jigglypuff|Wigglytuff|Zubat|Golbat|Oddish|Gloom|Vileplume|Paras|Parasect|Venonat|Venomoth|Diglett|Dugtrio|Meowth|Persian|Psyduck|Golduck|Mankey|Primeape|Growlithe|Arcanine|Poliwag|Poliwhirl|Poliwrath|Abra|Kadabra|Alakazam|Machop|Machoke|Machamp|Bellsprout|Weepinbell|Victreebel|Tentacool|Tentacruel|Geodude|Graveler|Golem|Ponyta|Rapidash|Slowpoke|Slowbro|Magnemite|Magneton|Farfetch'd|Doduo|Dodrio|Seel|Dewgong|Grimer|Muk|Shellder|Cloyster|Gastly|Haunter|Gengar|Onix|Drowzee|Hypno|Krabby|Kingler|Voltorb|Electrode|Exeggcute|Exeggutor|Cubone|Marowak|Hitmonlee|Hitmonchan|Lickitung|Koffing|Weezing|Rhyhorn|Rhydon|Chansey|Tangela|Kangaskhan|Horsea|Seadra|Goldeen|Seaking|Staryu|Starmie|Mr\. Mime|Scyther|Jynx|Electabuzz|Magmar|Pinsir|Tauros|Magikarp|Gyarados|Lapras|Ditto|Eevee|Vaporeon|Jolteon|Flareon|Porygon|Omanyte|Omastar|Kabuto|Kabutops|Aerodactyl|Snorlax|Articuno|Zapdos|Moltres|Dratini|Dragonair|Dragonite|Mewtwo)$
```

> Nota: el punto de `Mr. Mime` se escapa como `Mr\. Mime`.

---

## 7. Gramática formal

Gramática \( G = (V,\; T,\; P,\; S) \):

| Componente     | Definición                          |
|----------------|-------------------------------------|
| Variables $V$ | $\{S\}$ |
| Terminales $T$ | $P$ (los 151 nombres) |
| Símbolo inicial  | $S$ |
| Producciones     | $S \rightarrow p$ para todo $p \in P$ |

Es decir, hay exactamente 151 producciones, una por cada Pokémon.

---

## 8. Definición mediante BNF

```bnf
<pokemon> ::= "Mew"
            | "Bulbasaur"
            | "Ivysaur"
            | "Venusaur"
            | "Charmander"
            | "Charmeleon"
            | "Charizard"
            | "Squirtle"
            | "Wartortle"
            | "Blastoise"
            | "Caterpie"
            | "Metapod"
            | "Butterfree"
            | "Weedle"
            | "Kakuna"
            | "Beedrill"
            | "Pidgey"
            | "Pidgeotto"
            | "Pidgeot"
            | "Rattata"
            | "Raticate"
            | "Spearow"
            | "Fearow"
            | "Ekans"
            | "Arbok"
            | "Pikachu"
            | "Raichu"
            | "Sandshrew"
            | "Sandslash"
            | "Nidoran♀"
            | "Nidorina"
            | "Nidoqueen"
            | "Nidoran♂"
            | "Nidorino"
            | "Nidoking"
            | "Clefairy"
            | "Clefable"
            | "Vulpix"
            | "Ninetales"
            | "Jigglypuff"
            | "Wigglytuff"
            | "Zubat"
            | "Golbat"
            | "Oddish"
            | "Gloom"
            | "Vileplume"
            | "Paras"
            | "Parasect"
            | "Venonat"
            | "Venomoth"
            | "Diglett"
            | "Dugtrio"
            | "Meowth"
            | "Persian"
            | "Psyduck"
            | "Golduck"
            | "Mankey"
            | "Primeape"
            | "Growlithe"
            | "Arcanine"
            | "Poliwag"
            | "Poliwhirl"
            | "Poliwrath"
            | "Abra"
            | "Kadabra"
            | "Alakazam"
            | "Machop"
            | "Machoke"
            | "Machamp"
            | "Bellsprout"
            | "Weepinbell"
            | "Victreebel"
            | "Tentacool"
            | "Tentacruel"
            | "Geodude"
            | "Graveler"
            | "Golem"
            | "Ponyta"
            | "Rapidash"
            | "Slowpoke"
            | "Slowbro"
            | "Magnemite"
            | "Magneton"
            | "Farfetch'd"
            | "Doduo"
            | "Dodrio"
            | "Seel"
            | "Dewgong"
            | "Grimer"
            | "Muk"
            | "Shellder"
            | "Cloyster"
            | "Gastly"
            | "Haunter"
            | "Gengar"
            | "Onix"
            | "Drowzee"
            | "Hypno"
            | "Krabby"
            | "Kingler"
            | "Voltorb"
            | "Electrode"
            | "Exeggcute"
            | "Exeggutor"
            | "Cubone"
            | "Marowak"
            | "Hitmonlee"
            | "Hitmonchan"
            | "Lickitung"
            | "Koffing"
            | "Weezing"
            | "Rhyhorn"
            | "Rhydon"
            | "Chansey"
            | "Tangela"
            | "Kangaskhan"
            | "Horsea"
            | "Seadra"
            | "Goldeen"
            | "Seaking"
            | "Staryu"
            | "Starmie"
            | "Mr. Mime"
            | "Scyther"
            | "Jynx"
            | "Electabuzz"
            | "Magmar"
            | "Pinsir"
            | "Tauros"
            | "Magikarp"
            | "Gyarados"
            | "Lapras"
            | "Ditto"
            | "Eevee"
            | "Vaporeon"
            | "Jolteon"
            | "Flareon"
            | "Porygon"
            | "Omanyte"
            | "Omastar"
            | "Kabuto"
            | "Kabutops"
            | "Aerodactyl"
            | "Snorlax"
            | "Articuno"
            | "Zapdos"
            | "Moltres"
            | "Dratini"
            | "Dragonair"
            | "Dragonite"
            | "Mewtwo"
```

---

## 9. BNF extendida (EBNF)

```ebnf
pokemon = "Mew"
        | "Bulbasaur"
        | "Ivysaur"
        | ...
        | "Mewtwo" ;
```

(La elipsis representa el resto de los 151 nombres listados en la sección anterior.)

---

## 10. Autómata Finito

El lenguaje es regular y puede ser reconocido por un Autómata Finito Determinista (AFD).

Estados principales:

| Estado | Significado                              |
|:------:|------------------------------------------|
| $q_0$ | Inicio / saltando espacios             |
| $q_1$ | Leyendo caracteres del nombre          |
| $q_A$ | Aceptación (se leyó un nombre completo)|
| $q_E$ | Error                                  |

El diagrama completo se encuentra en el archivo `ConstanteDecimalX.gv`.

---

## 11. AFD formal (5-upla)

$$A = (Q, \Sigma, \delta, q_0, F)$$

| Componente | Definición |
|---|---|
| $Q$ | $\{q_0, q_1, q_A, q_E\}$ |
| $\Sigma$ | letras + {♀, ♂, apóstrofo, punto, espacio, salto de línea} |
| $q_0$ | estado inicial |
| $F$ | $\{q_A\}$ |

**Función de transición (resumida):**

| Estado | espacio/newline | carácter | otro |
|---|---|---|---|
| $q_0$ | $q_0$ | $q_1$ | $q_E$ |
| $q_1$ | $q_A$ | $q_1$ | $q_E$ |
| $q_A$ | $q_A$ | $q_E$ | $q_E$ |
| $q_E$ | $q_E$ | $q_E$ | $q_E$ |

La verificación de que el lexema reconocido pertenece realmente al conjunto \( P \) se realiza mediante una tabla de búsqueda (`pokemon_to_value`), tal como se implementa en el código.

---

## 12. Ejemplos de pertenencia al lenguaje

**Pertenecen a \( L_X \)**

```text
Mew
Bulbasaur
Pikachu
Nidoran♀
Nidoran♂
Farfetch'd
Mr. Mime
Mewtwo
```

**No pertenecen a \( L_X \)**

```text
pikachu          (minúsculas)
Pikachuu         (nombre inexistente)
Pikachu +        (contiene operador)
+ Charmander
PokemonFalso
```

---

## 13. Relación entre lenguaje y semántica

| Concepto     | Pregunta que responde                          | Ejemplo                     |
|--------------|------------------------------------------------|-----------------------------|
| **Lenguaje** | ¿Es un token válido?                           | `Pikachu` ∈ \( L_X \)      |
| **Semántica**| ¿Qué valor numérico representa?                | $f(\text{Pikachu}) = 25$ |
| **Aritmética**| ¿Cómo se acumulan los valores? (base 151 + carry) | 25 + 150 = 175 → 1×151 + 24 |

El lenguaje solo se ocupa del reconocimiento del token.  
La interpretación numérica y la suma con *carry* en base 151 son responsabilidad de la semántica del programa.

---

## 14. Correspondencia con la implementación

| Elemento formal              | Implementación en C                          |
|------------------------------|----------------------------------------------|
| Conjunto \( P \)             | Array estático `pokedex[151]`                |
| Función \( f \)              | `pokemon_to_value()`                         |
| Reconocimiento del token     | `GetNextX()` (máquina de estados enum+switch)|
| Autómata                     | Estados `S_START`, `S_READING`, `S_DONE`, `S_ERROR` |
| Acumulador en base 151       | Estructura `PokemonNumber` + `number_add()`  |

De esta forma, la especificación formal y el código quedan alineados de manera clara y verificable.
