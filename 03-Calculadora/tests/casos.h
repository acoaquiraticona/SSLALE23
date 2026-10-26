/**
 * @file casos.h
 * @brief Corpus de 76 casos de aceptación tomado de Calc.md, sección 11.1.
 *
 * Cada fila describe una sesión independiente del ejecutable. La tabla reúne
 * entradas válidas y fallidas con sus efectos observables, sin derivar las
 * expectativas de la implementación que se está verificando. Los identificadores
 * Txx conservan la numeración de la especificación, incluidos sus saltos.
 */
#ifndef CASOS_H
#define CASOS_H
/**
 * @brief Entrada y expectativas observables de un caso de aceptación.
 *
 * @details id identifica el caso de la especificación; entrada alimenta stdin;
 * salida es el contenido exacto esperado de stdout. codigo es el nombre del
 * error esperado o una cadena vacía si no se espera ninguno; linea y columna
 * localizan ese diagnóstico y solo se consultan cuando codigo no está vacío.
 * estado es el código de terminación del proceso e infos cuenta las sentencias
 * confirmadas mediante registros INFO. El mensaje del error se comprueba por
 * su estructura en Logs, por lo que no forma parte de cada fila.
 */
typedef struct { const char *id, *entrada, *salida, *codigo; int linea, columna, estado, infos; } AcceptanceCase;
/** @brief Casos recorridos por Tabla en el orden de la especificación. */
static const AcceptanceCase casos[] = {
    /** Entradas vacías, literales y reglas de precedencia y asociatividad. */
    {"T01", "", "", "", 0, 0, 0, 0},
    {"T02", "# comentario con ;\n", "", "", 0, 0, 0, 0},
    {"T03", "[mew];", "[mew] (0)\n", "", 0, 0, 0, 1},
    {"T04", "[bulbasaur mew];", "[bulbasaur mew] (10)\n", "", 0, 0, 0, 1},
    {"T05", "[mew bulbasaur];", "[bulbasaur] (1)\n", "", 0, 0, 0, 1},
    {"T06", "[ \tivysaur\tvenusaur ];", "[ivysaur venusaur] (23)\n", "", 0, 0, 0, 1},
    {"T07", "[ivysaur] plusle [venusaur];", "[charmeleon] (5)\n", "", 0, 0, 0, 1},
    {"T08", "[bulbasaur] plusle [ivysaur] tandemaus [venusaur];", "[squirtle] (7)\n", "", 0, 0, 0, 1},
    {"T09", "([bulbasaur] plusle [ivysaur]) tandemaus [venusaur];", "[blastoise] (9)\n", "", 0, 0, 0, 1},
    {"T10", "[blastoise] minun [venusaur] minun [ivysaur];", "[charmander] (4)\n", "", 0, 0, 0, 1},
    /** Negación unaria, negaciones consecutivas y duplicación con ditto. */
    {"T12", "minun [squirtle];", "minun [squirtle] (-7)\n", "", 0, 0, 0, 1},
    {"T13", "minun minun [squirtle];", "[squirtle] (7)\n", "", 0, 0, 0, 1},
    {"T14", "minun [mew];", "[mew] (0)\n", "", 0, 0, 0, 1},
    {"T15", "[charmeleon] minun minun [ivysaur];", "[squirtle] (7)\n", "", 0, 0, 0, 1},
    {"T20", "ditto([ivysaur venusaur]);", "[charmander charizard] (46)\n", "", 0, 0, 0, 1},
    {"T21", "ditto(minun [ivysaur]);", "minun [charmander] (-4)\n", "", 0, 0, 0, 1},
    {"T22", "ditto([mew]);", "[mew] (0)\n", "", 0, 0, 0, 1},
    /** Ciclo de vida de variables, reasignación y nombres sensibles a mayúsculas. */
    {"T23", "captura energia = [ivysaur]; energia plusle [venusaur];", "[charmeleon] (5)\n", "", 0, 0, 0, 2},
    {"T24", "captura energia = [ivysaur]; captura energia = energia tandemaus [venusaur]; energia;", "[charizard] (6)\n", "", 0, 0, 0, 3},
    {"T25", "captura energia = [ivysaur]; liberar energia;", "", "", 0, 0, 0, 2},
    {"T26", "captura energia = [ivysaur]; liberar energia; energia;", "", "E_UNDEFINED", 1, 47, 1, 2},
    {"T27", "liberar energia;", "", "E_UNDEFINED", 1, 9, 1, 0},
    {"T28", "captura energia = [ivysaur]; liberar energia; liberar energia;", "", "E_UNDEFINED", 1, 55, 1, 2},
    {"T29", "captura energia = [ivysaur]; liberar energia; captura energia = [venusaur]; energia;", "[venusaur] (3)\n", "", 0, 0, 0, 4},
    {"T30", "energia;", "", "E_UNDEFINED", 1, 1, 1, 0},
    {"T31", "captura energia = energia plusle [bulbasaur];", "", "E_UNDEFINED", 1, 19, 1, 0},
    {"T32", "captura energia = [ivysaur]; Energia;", "", "E_UNDEFINED", 1, 30, 1, 1},
    /** Errores léxicos y sintácticos: clasificación y posición del diagnóstico. */
    {"T35", "[pikachu];", "", "E_MISSINGNO", 1, 1, 1, 0},
    {"T36", "[bulbasaurmew];", "", "E_MISSINGNO", 1, 1, 1, 0},
    {"T37", "bulbasaur;", "", "E_MISSINGNO", 1, 1, 1, 0},
    {"T38", "12;", "", "E_MISSINGNO", 1, 1, 1, 0},
    {"T39", "[];", "", "E_MISSINGNO", 1, 1, 1, 0},
    {"T40", "[ivysaur;", "", "E_MISSINGNO", 1, 1, 1, 0},
    {"T41", "[missingno];", "", "E_MISSINGNO", 1, 1, 1, 0},
    {"T42", "[IVYSAUR];", "", "E_MISSINGNO", 1, 1, 1, 0},
    {"T43", "@;", "", "E_LEXICAL", 1, 1, 1, 0},
    {"T44", "[ivysaur] plusle ;", "", "E_SYNTAX", 1, 18, 1, 0},
    {"T45", "([ivysaur] plusle [venusaur];", "", "E_SYNTAX", 1, 29, 1, 0},
    {"T46", "[ivysaur]", "", "E_SYNTAX", 1, 10, 1, 0},
    {"T47", "captura a = [ivysaur] = [venusaur];", "", "E_SYNTAX", 1, 23, 1, 0},
    {"T48", ";", "", "E_SYNTAX", 1, 1, 1, 0},
    {"T49", "liberar [ivysaur];", "", "E_SYNTAX", 1, 9, 1, 0},
    {"T50", "captura energia [ivysaur];", "", "E_SYNTAX", 1, 17, 1, 0},
    {"T51", "captura plusle = [ivysaur];", "", "E_SYNTAX", 1, 9, 1, 0},
    /** Una sentencia fallida no confirma cambios; la siguiente puede continuar. */
    {"T52", "captura energia = [ivysaur]; captura energia = energia tandemaus [charmeleon mew mew mew mew mew]; energia;", "[ivysaur] (2)\n", "E_RANGE", 1, 56, 1, 2},
    {"T53", "[pikachu] plusle [ivysaur]; [venusaur];", "[venusaur] (3)\n", "E_MISSINGNO", 1, 1, 1, 1},
    {"T54", "captura energia = [ivysaur]; captura energia = [pikachu]; energia;", "[ivysaur] (2)\n", "E_MISSINGNO", 1, 48, 1, 2},
    {"T55", "captura energia = [ivysaur]; liberar energia plusle [venusaur]; energia;", "[ivysaur] (2)\n", "E_SYNTAX", 1, 46, 1, 2},
    {"T56", "captura energia = [ivysaur]; @; energia;", "[ivysaur] (2)\n", "E_LEXICAL", 1, 30, 1, 2},
    {"T57", "captura energia = [ivysaur]", "", "E_SYNTAX", 1, 28, 1, 0},
    /** Extremos del dominio y rechazo del desborde en literales y operaciones. */
    {"T58", "[blastoise blastoise blastoise blastoise blastoise blastoise];", "[blastoise blastoise blastoise blastoise blastoise blastoise] (999999)\n", "", 0, 0, 0, 1},
    {"T59", "minun [blastoise blastoise blastoise blastoise blastoise blastoise];", "minun [blastoise blastoise blastoise blastoise blastoise blastoise] (-999999)\n", "", 0, 0, 0, 1},
    {"T60", "[bulbasaur mew mew mew mew mew mew];", "", "E_RANGE", 1, 1, 1, 0},
    {"T61", "[blastoise blastoise blastoise blastoise blastoise blastoise] plusle [bulbasaur];", "", "E_RANGE", 1, 63, 1, 0},
    {"T62", "minun [blastoise blastoise blastoise blastoise blastoise blastoise] minun [bulbasaur];", "", "E_RANGE", 1, 69, 1, 0},
    {"T63", "[charmeleon mew mew mew mew mew] tandemaus [ivysaur];", "", "E_RANGE", 1, 34, 1, 0},
    {"T64", "ditto([charmeleon mew mew mew mew mew]);", "", "E_RANGE", 1, 1, 1, 0},
    {"T65", "[charmeleon mew mew mew mew mew] plusle [charmeleon mew mew mew mew mew] minun [charmeleon mew mew mew mew mew];", "", "E_RANGE", 1, 34, 1, 0},
    {"T66", "captura energia = [ivysaur]; captura energia = [bulbasaur mew mew mew mew mew mew]; energia;", "[ivysaur] (2)\n", "E_RANGE", 1, 48, 1, 2},
    /** Límites de identificadores, CRLF, comentarios y recuperación de literales. */
    {"T67", "captura abcdefghijklmnopqrstuvwxyzabcde = [ivysaur]; abcdefghijklmnopqrstuvwxyzabcde;", "[ivysaur] (2)\n", "", 0, 0, 0, 2},
    {"T68", "captura abcdefghijklmnopqrstuvwxyzabcdef = [ivysaur];", "", "E_LIMIT", 1, 9, 1, 0},
    {"T69", "captura energia =\r\n[ivysaur] plusle\r\n[venusaur];\r\nenergia;", "[charmeleon] (5)\n", "", 0, 0, 0, 2},
    {"T70", "captura energia = [ivysaur]; # ; ignorado\nenergia;", "[ivysaur] (2)\n", "", 0, 0, 0, 2},
    {"T71", "[ivysaur\nvenusaur]; [venusaur];", "[venusaur] (3)\n", "E_MISSINGNO", 1, 1, 1, 1},
    {"T72", "[ivysaur # comentario]; [venusaur];", "[venusaur] (3)\n", "E_MISSINGNO", 1, 1, 1, 1},
    /** Palabras parecidas a reservadas, ditto anidado, precedencia y bytes UTF-8. */
    {"T73", "captura mewtwo = [ivysaur]; mewtwo;", "[ivysaur] (2)\n", "", 0, 0, 0, 2},
    {"T74", "captura squirtle2 = [venusaur]; squirtle2;", "[venusaur] (3)\n", "", 0, 0, 0, 2},
    {"T75", "captura plusle_total = [charmander]; plusle_total;", "[charmander] (4)\n", "", 0, 0, 0, 2},
    {"T76", "[mewtwo]; [ivysaur];", "[ivysaur] (2)\n", "E_MISSINGNO", 1, 1, 1, 1},
    {"T77", "ditto [ivysaur]; [venusaur];", "[venusaur] (3)\n", "E_SYNTAX", 1, 7, 1, 1},
    {"T78", "ditto(); [venusaur];", "[venusaur] (3)\n", "E_SYNTAX", 1, 7, 1, 1},
    {"T79", "ditto(ditto([ivysaur]));", "[wartortle] (8)\n", "", 0, 0, 0, 1},
    {"T80", "minun [ivysaur] tandemaus [venusaur];", "minun [charizard] (-6)\n", "", 0, 0, 0, 1},
    {"T81", "minun [bulbasaur] plusle [ivysaur];", "[bulbasaur] (1)\n", "", 0, 0, 0, 1},
    {"T83", "[iv\303\251]; [venusaur];", "[venusaur] (3)\n", "E_MISSINGNO", 1, 1, 1, 1},
    {"T84", "[pikachu] @ [missingno] 12; [venusaur];", "[venusaur] (3)\n", "E_MISSINGNO", 1, 1, 1, 1},
};
#endif
