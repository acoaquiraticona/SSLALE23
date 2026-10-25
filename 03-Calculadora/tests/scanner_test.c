/* Pruebas unitarias del scanner; stdin contiene bytes exactos. */
#include "Scanner.h"
#include "test_support.h"

static void Input(const char *source) {
    TestWriteFile(".build/scanner-input.txt", source);
    assert(freopen(".build/scanner-input.txt", "rb", stdin) != NULL);
    ScannerReset();
}

static Token Next(TipoToken tipo, int linea, int columna) {
    Token t = GetNextToken();
    assert(t.tipo == tipo);
    assert(t.linea == linea && t.columna == columna);
    return t;
}

int main(void) {
    Input("\t# \303\251 ;\r\ncaptura a=\r[ivysaur]\n;\r\n");
    Next(TOKEN_CAPTURA, 2, 1);
    TestEqual(Next(TOKEN_IDENTIFIER, 2, 9).lexema, "a");
    Next(TOKEN_EQUAL, 2, 10);
    assert(Next(TOKEN_NATURAL, 3, 1).valor == 2);
    Next(TOKEN_SEMICOLON, 4, 1);
    Next(TOKEN_EOF, 5, 1);
    Next(TOKEN_EOF, 5, 1);

    Input("mewtwo squirtle2 plusle_total falinks mew plusle minun tandemaus ditto captura liberar");
    for (int i = 0; i < 4; ++i) assert(GetNextToken().tipo == TOKEN_IDENTIFIER);
    assert(GetNextToken().diagnostico.codigo == E_MISSINGNO);
    TipoToken tipos[] = {TOKEN_PLUSLE, TOKEN_MINUN, TOKEN_TANDEMAUS, TOKEN_DITTO, TOKEN_CAPTURA, TOKEN_LIBERAR};
    for (size_t i = 0; i < sizeof tipos / sizeof tipos[0]; ++i) assert(GetNextToken().tipo == tipos[i]);
    assert(GetNextToken().tipo == TOKEN_EOF);

    Input("[mew] [bulbasaur] [ivysaur] [venusaur] [charmander] [charmeleon] [charizard] [squirtle] [wartortle] [blastoise]");
    for (int i = 0; i < 10; ++i) {
        Token t = GetNextToken();
        assert(t.tipo == TOKEN_NATURAL && t.valor == i && !t.fuera_de_rango);
    }
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

    char source[24000];
    strcpy(source, "[");
    for (int i = 0; i < 4000; ++i) strcat(source, "mew ");
    strcat(source, "bulbasaur];");
    Input(source);
    Token longNatural = GetNextToken();
    assert(longNatural.tipo == TOKEN_NATURAL && longNatural.valor == 1);
    assert(!longNatural.fuera_de_rango && longNatural.lexema_recortado);
    assert(strstr(longNatural.lexema, "(recortado)") != NULL);
    assert(GetNextToken().tipo == TOKEN_SEMICOLON);
    strcpy(source, "[");
    for (int i = 0; i < 1000; ++i) strcat(source, "blastoise ");
    strcat(source, "];");
    Input(source);
    Token huge = GetNextToken();
    assert(huge.tipo == TOKEN_NATURAL && huge.fuera_de_rango);
    assert(GetNextToken().tipo == TOKEN_SEMICOLON);

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

    /* NUL y 0xFF son bytes, no EOF ni caracteres que se puedan ignorar. */
    FILE *f = fopen(".build/scanner-input.txt", "wb");
    assert(f != NULL);
    assert(fputc(0, f) == 0 && fputc(255, f) == 255);
    assert(fclose(f) == 0);
    assert(freopen(".build/scanner-input.txt", "rb", stdin) != NULL);
    ScannerReset();
    assert(Next(TOKEN_ERROR, 1, 1).diagnostico.codigo == E_LEXICAL);
    assert(Next(TOKEN_ERROR, 1, 2).diagnostico.codigo == E_LEXICAL);
    Next(TOKEN_EOF, 1, 3);
    puts("Scanner: assert OK");
    return 0;
}
