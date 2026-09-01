#include "Pokedex.h"
#include <string.h>
#include <stdint.h>

/*
 * Tabla con los primeros 151 Pokémon.
 * Incluye a los pokémon desde el 0 hasta el 150 para representar dígitos en base 151.
 * 
 * 0 = Mew -> Por ser el padre de los pokémon 
 * 
 * Ojo: el orden es según la pokédex de primera edición, no por orden alfabético.
 */

 /* Tabla de los primeros 151 Pokémon */
static const char *pokedex[POKEDEX_SIZE] = {
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

/*
 * Convierte un nombre de un Pokémon a su valor de entrada en la pokédex (0–150).
 * Devuelve el infame INVALID_POKEMON si no existe.
 */
int16_t pokemon_to_value(const char *name) {
    if (name == NULL || name[0] == '\0') {  // Si el nombre es nulo o el primer caracter es el salto de línea
        return INVALID_POKEMON;
    }

    /* MissingNo. es inválido explícitamente */
    if (strcmp(name, "MissingNo.") == 0) {  // Si le mandan a MissigNo.
        return INVALID_POKEMON;
    }

    // Esto no es lo más performante del mundo, pero sirve por ahora...
    for (int16_t i = 0; i < POKEDEX_SIZE; i++) {    // Recorre la pokédex para devolver en índice de entrada
        if (strcmp(pokedex[i], name) == 0) {
            return i;
        }
    }

    return INVALID_POKEMON; // Si no encuentra nada, devuelve error
}

/*
 * Convierte un valor desde el 0 al 150 al nombre de un Pokémon.
 * Devuelve NULL si el valor es inválido.
 */
const char *value_to_pokemon(int16_t value) {
    if (value < 0 || value >= POKEDEX_SIZE) {
        return NULL;
    }
    return pokedex[value];
}
