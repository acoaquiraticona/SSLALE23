/**
 * @file scanner_test.c
 * @brief Pruebas del reconocimiento léxico con entrada de bytes controlados.
 *
 * Cada escenario reemplaza stdin y reinicia el estado privado del scanner.
 * Se comprueban clase, atributos, posición, recuperación ante entradas inválidas
 * y la diferencia entre recortar el texto guardado y reconocer el token completo.
 */
#include "Scanner.h"
#include "test_support.h"

/**
 * @brief Prepara un nuevo flujo de entrada e inicia una sesión léxica aislada.
 * @param source Texto que se escribe sin traducir finales de línea.
 *
 * Reabrir stdin posiciona el archivo al comienzo; ScannerReset limpia solamente
 * el estado del scanner, por lo que ambas operaciones son necesarias.
 */
static void Input(const char *source) {
    TestWriteFile(".build/scanner-input.txt", source);
    assert(freopen(".build/scanner-input.txt", "rb", stdin) != NULL);
    ScannerReset();
}

/**
 * @brief Consume un token y exige la clase y posición indicadas.
 * @param tipo Clase de token esperada.
 * @param linea Número de línea esperado, comenzando en uno.
 * @param columna Número de columna esperado, comenzando en uno.
 * @return Token leído para permitir comprobar sus atributos en el llamador.
 */
static Token Next(TipoToken tipo, int linea, int columna) {
    Token token = GetNextToken();
    assert(token.tipo == tipo);
    assert(token.linea == linea && token.columna == columna);
    return token;
}

/**
 * @brief Ejecuta los casos léxicos normales, de error y de límites.
 * @return Cero si todas las aserciones se cumplen.
 */
int main(void) {
    /** Espacios y comentarios no producen tokens; CR, LF y CRLF actualizan la
     * posición de manera coherente. EOF repetido conserva su misma ubicación. */
    Input("\t# \303\251 ;\r\ncaptura a=\r[ivysaur]\n;\r\n");
    Next(TOKEN_CAPTURA, 2, 1);
    TestEqual(Next(TOKEN_IDENTIFIER, 2, 9).lexema, "a");
    Next(TOKEN_EQUAL, 2, 10);
    assert(Next(TOKEN_NATURAL, 3, 1).valor == 2);
    Next(TOKEN_SEMICOLON, 4, 1);
    Next(TOKEN_EOF, 5, 1);
    Next(TOKEN_EOF, 5, 1);

    /** Las reservadas se reconocen por palabra completa; los nombres comunes son
     * identificadores y un nombre de dígito aislado exige corchetes. */
    Input("mewtwo squirtle2 plusle_total entrenador mew plusle minun tandemaus ditto captura liberar");
    for (int i = 0; i < 4; ++i) {
        assert(GetNextToken().tipo == TOKEN_IDENTIFIER);
    }
    assert(GetNextToken().diagnostico.codigo == E_MISSINGNO);
    TipoToken tipos[] = {TOKEN_PLUSLE, TOKEN_MINUN, TOKEN_TANDEMAUS, TOKEN_DITTO, TOKEN_CAPTURA, TOKEN_LIBERAR};
    for (size_t i = 0; i < sizeof tipos / sizeof tipos[0]; ++i) {
        assert(GetNextToken().tipo == tipos[i]);
    }
    assert(GetNextToken().tipo == TOKEN_EOF);

    /** La tabla de nombres debe corresponder exactamente a los dígitos 0 a 9. */
    Input("[mew] [bulbasaur] [ivysaur] [venusaur] [charmander] [charmeleon] [charizard] [squirtle] [wartortle] [blastoise]");
    for (int i = 0; i < 10; ++i) {
        Token token = GetNextToken();
        assert(token.tipo == TOKEN_NATURAL && token.valor == i && !token.fuera_de_rango);
    }
    /** Un literal incompleto deja disponible el delimitador para recuperar la
     * lectura; dentro de los corchetes # no inicia un comentario válido. */
    Input("[ivysaur;[venusaur]");
    assert(Next(TOKEN_ERROR, 1, 1).diagnostico.codigo == E_MISSINGNO);
    Next(TOKEN_SEMICOLON, 1, 9);
    assert(Next(TOKEN_NATURAL, 1, 10).valor == 3);
    Input("[ivysaur\r\n[venusaur];");
    assert(Next(TOKEN_ERROR, 1, 1).diagnostico.codigo == E_MISSINGNO);
    assert(Next(TOKEN_NATURAL, 2, 1).valor == 3);
    Input("[ivysaur # x];");
    assert(GetNextToken().diagnostico.codigo == E_MISSINGNO);
    assert(GetNextToken().tipo == TOKEN_SEMICOLON);

    /** Miles de ceros iniciales siguen formando el valor uno aunque su lexema
     * se recorte. Un valor grande conserva clase NATURAL y marca fuera_de_rango
     * para que la capa semántica decida el diagnóstico correspondiente. */
    char source[24000];
    strcpy(source, "[");
    for (int i = 0; i < 4000; ++i) {
        strcat(source, "mew ");
    }
    strcat(source, "bulbasaur];");
    Input(source);
    Token longNatural = GetNextToken();
    assert(longNatural.tipo == TOKEN_NATURAL && longNatural.valor == 1);
    assert(!longNatural.fuera_de_rango && longNatural.lexema_recortado);
    assert(strstr(longNatural.lexema, "(recortado)") != NULL);
    assert(GetNextToken().tipo == TOKEN_SEMICOLON);
    strcpy(source, "[");
    for (int i = 0; i < 1000; ++i) {
        strcat(source, "blastoise ");
    }
    strcat(source, "];");
    Input(source);
    Token huge = GetNextToken();
    assert(huge.tipo == TOKEN_NATURAL && huge.fuera_de_rango);
    assert(GetNextToken().tipo == TOKEN_SEMICOLON);

    /** Un identificador demasiado largo se consume completo y conserva la
     * posición del token siguiente. Se verifican ambos lados del límite de 31
     * caracteres y se conserva el texto UTF-8 inválido en el diagnóstico. */
    memset(source, 'a', 4000);
    strcpy(source + 4000, ";[ivysaur]");
    Input(source);
    Token longId = GetNextToken();
    assert(longId.diagnostico.codigo == E_LIMIT && longId.lexema_recortado);
    Next(TOKEN_SEMICOLON, 1, 4001);
    assert(Next(TOKEN_NATURAL, 1, 4002).valor == 2);
    Input("abcdefghijklmnopqrstuvwxyzabcde abcdefghijklmnopqrstuvwxyzabcdef;");
    assert(GetNextToken().tipo == TOKEN_IDENTIFIER);
    assert(GetNextToken().diagnostico.codigo == E_LIMIT);
    assert(GetNextToken().tipo == TOKEN_SEMICOLON);
    Input("[iv\303\251];");
    Token unicode = GetNextToken();
    assert(unicode.diagnostico.codigo == E_MISSINGNO);
    assert(strstr(unicode.diagnostico.detalle, "\303\251") != NULL);

    /** NUL y 0xFF se escriben como bytes para evitar el terminador de las cadenas:
     * ambos deben producir error léxico y avanzar una columna antes de EOF. */
    FILE *archivoEntrada = fopen(".build/scanner-input.txt", "wb");
    assert(archivoEntrada != NULL);
    assert(fputc(0, archivoEntrada) == 0 && fputc(255, archivoEntrada) == 255);
    assert(fclose(archivoEntrada) == 0);
    assert(freopen(".build/scanner-input.txt", "rb", stdin) != NULL);
    ScannerReset();
    assert(Next(TOKEN_ERROR, 1, 1).diagnostico.codigo == E_LEXICAL);
    assert(Next(TOKEN_ERROR, 1, 2).diagnostico.codigo == E_LEXICAL);
    Next(TOKEN_EOF, 1, 3);
    puts("Scanner: assert OK");
    return 0;
}
