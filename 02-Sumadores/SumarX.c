#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define BASE 151
#define MAX_DIGITS 64

/* ---------- Pokédex (Orden Nacional Dex 1-151) ---------- */
/* Lista de nombres de Pokémon (1ra generación) en orden de índice.
 * El índice 0 corresponde a Mew, el índice 150 a Mewtwo.
*/
static const char *const pokedex[151] = {
    "Mew", "Bulbasaur", "Ivysaur", "Venusaur", "Charmander", "Charmeleon", "Charizard", "Squirtle",
    "Wartortle", "Blastoise", "Caterpie", "Metapod", "Butterfree", "Weedle", "Kakuna", "Beedrill",
    "Pidgey", "Pidgeotto", "Pidgeot", "Rattata", "Raticate", "Spearow", "Fearow", "Ekans", "Arbok",
    "Pikachu", "Raichu", "Sandshrew", "Sandslash", "Nidoran♀", "Nidorina", "Nidoqueen", "Nidoran♂", 
    "Nidorino", "Nidoking", "Clefairy", "Clefable", "Vulpix", "Ninetales", "Jigglypuff", "Wigglytuff", 
    "Zubat", "Golbat", "Oddish", "Gloom", "Vileplume", "Paras", "Parasect", "Venonat", "Venomoth",
    "Diglett", "Dugtrio", "Meowth", "Persian", "Psyduck", "Golduck", "Mankey", "Primeape", "Growlithe",
    "Arcanine", "Poliwag", "Poliwhirl", "Poliwrath", "Abra", "Kadabra", "Alakazam", "Machop", "Machoke",
    "Machamp", "Bellsprout", "Weepinbell", "Victreebel", "Tentacool", "Tentacruel", "Geodude", "Graveler",
    "Golem", "Ponyta", "Rapidash", "Slowpoke", "Slowbro", "Magnemite", "Magneton", "Farfetch'd", "Doduo",
    "Dodrio", "Seel", "Dewgong", "Grimer", "Muk", "Shellder", "Cloyster", "Gastly", "Haunter", "Gengar",
    "Onix", "Drowzee", "Hypno", "Krabby", "Kingler", "Voltorb", "Electrode", "Exeggcute", "Exeggutor",
    "Cubone", "Marowak", "Hitmonlee", "Hitmonchan", "Lickitung", "Koffing", "Weezing", "Rhyhorn", "Rhydon",
    "Chansey", "Tangela", "Kangaskhan", "Horsea", "Seadra", "Goldeen", "Seaking", "Staryu", "Starmie",
    "Mr.Mime","Scyther","Jynx","Electabuzz","Magmar","Pinsir","Tauros","Magikarp","Gyarados","Lapras",
    "Ditto","Eevee","Vaporeon","Jolteon","Flareon","Porygon","Omanyte","Omastar","Kabuto","Kabutops",
    "Aerodactyl","Snorlax","Articuno","Zapdos","Moltres","Dratini","Dragonair","Dragonite", "Mewtwo"
};

/* ---------- Número en base 151 ---------- */
typedef struct {
    int digits[MAX_DIGITS];   /* digits[0] = menos significativo */
    int size;                 /* cantidad de dígitos significativos */
} PokemonNumber;

static void number_init(PokemonNumber *n){
    n->digits[0] = 0;
    n->size = 1;
}

/* Suma un valor (0..150) al número, propagando el carry */
static void number_add(PokemonNumber *n, int value){
    int carry = value;
    int i = 0;

    while (carry > 0 || i < n->size) {
        if (i >= MAX_DIGITS) {
            fprintf(stderr, "ERROR: overflow de pokemons !\n");
            exit(1);
        }

        int sum = (i < n->size ? n->digits[i] : 0) + carry;
        n->digits[i] = sum % BASE;
        carry = sum / BASE;
        i++;
    }

    n->size = i;
}

/* Imprime el número como secuencia de Pokémon (MSD → LSD) */
static void number_print(const PokemonNumber *n){
    for (int i = n->size - 1; i >= 0; i--) {
        printf("%s", pokedex[n->digits[i]]);
        if (i > 0)
            putchar(' ');
    }
    putchar('\n');
}

/* ---------- GetNextX (máquina de estados) ---------- */

static int pokemon_to_value(const char *name){
    for (int i = 0; i < BASE; i++) {
        if (strcmp(name, pokedex[i]) == 0)
            return i;
    }
    return -1;
}

typedef enum {
    S_START,
    S_READING,
    S_DONE,
    S_ERROR
} State;

bool GetNextX(FILE *s, int *p){
    State state = S_START;
    char  buf[32];
    size_t len = 0;
    int c;

    while ((c = fgetc(s)) != EOF) {
        switch (state) {
        case S_START:
            if (isspace(c))
                break;
            if (len < sizeof(buf) - 1)
                buf[len++] = (char)c;
            state = S_READING;
            break;

        case S_READING:
            if (isspace(c) || c == '\n') {
                ungetc(c, s);
                state = S_DONE;
            } else if (len < sizeof(buf) - 1) {
                buf[len++] = (char)c;
            } else {
                state = S_ERROR;
            }
            break;

        default:
            break;
        }

        if (state == S_DONE || state == S_ERROR)
            break;
    }

    if (state == S_READING)
        state = S_DONE;

    if (state != S_DONE || len == 0)
        return false;

    buf[len] = '\0';

    int value = pokemon_to_value(buf);
    if (value < 0)
        return false;

    *p = value;
    return true;
}

/* ---------- main ---------- */

int main(void){
    PokemonNumber total;
    number_init(&total);

    int n;
    while (GetNextX(stdin, &n)) {
        number_add(&total, n);
        number_print(&total);          /* imprime el total parcial en base 151 */
    }

    return 0;
}